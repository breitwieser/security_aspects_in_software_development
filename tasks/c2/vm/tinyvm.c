///
/// \file
/// \brief Main program of the standalone tinyvm tool
///
#include "tinyvm.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

//----------------------------------------------------------------------
typedef struct VmCmdLineInfo {
  /// \brief Initial image to load.
  const char *inputFileName;

  /// \brief Log level
  VmLogLevel logLevel;

  /// \brief Stack slots
  size_t maxStack;

  /// \brief Trace stack useage
  bool traceStack;

  /// \brief Enable input forwarding (VMC_READ)
  bool forwardInput;

  /// \brief Enable input forwarding (VMC_WRITE)
  bool forwardOutput;
} VmCmdLineInfo;

//----------------------------------------------------------------------
static void VmMainShowUsage(const char *progname)
{
  static const char HELP_TEXT[] =
    "TinyVM Virtual Machine (vm bytecode format: v%u.%u)\n"
    "======================================================================\n"
    "Usage: %s [options] -i <infile>\n\n"
    "-i <infile>     - Load bytecode and objects from input file '<infile>'\n"
    "-s <maxstack>   - Set the maximum size of the operand stack (default: 1024 slots)\n"
    "-v              - Verbose output (repeat for increased verbosity)\n"
    "-t              - Trace stack usage\n"
    "-q              - Quiet output (reset verbosity level to 'error')\n"
    "-I              - Forward stdin to the VMC_READ virtual machine calls\n"
    "-O              - Forward VMC_WRITE virtual machine calls to stdout\n"
    "\n";

  // Skip past the last trailing '/'
  const char *last_slash = strrchr(progname, '/');
  if (last_slash) {
    progname = last_slash + 1;
  }

  // And print the help text
  fprintf(stdout, HELP_TEXT, VM_VERSION_MAJOR, VM_VERSION_MINOR, progname);
}

//----------------------------------------------------------------------
static bool VmMainParseCmdLine(VmCmdLineInfo *info, int argc, char **argv)
{
  if (!info || argc < 1 || !argv) {
    return false; // Bad parameters
  }

  // Default setup
  info->inputFileName = NULL;
  info->logLevel = VM_LOG_INFO;
  info->maxStack = 1024;
  info->traceStack = false;
  info->forwardInput = false;
  info->forwardOutput = false;

  // Parse the command line (using getopt)
  int c;

  while ((c = getopt(argc, argv, "hi:vqs:tIO")) != -1) {
    switch (c) {
    case 'h': // Show usage help
      VmMainShowUsage(argv[0]);
      return false;

    case 'i': // Input bytecode file
      if (info->inputFileName) {
        VmLogError(NULL, "bytecode input file (-i) has already been given.");
        return false;
      }

      info->inputFileName = optarg;
      break;

    case 'v': // Increase log verbosity
      if (info->logLevel < VM_LOG_DEBUG) {
        info->logLevel += 1;
      }
      break;

    case 'q': // Reset log verbosity
      info->logLevel = VM_LOG_ERROR;
      break;

    case 's': // Set maximum stack size
      {
        // Parse the value
        errno = 0;
        long long val = atoll(optarg);
        if (errno != 0 || val < 0 || val > SIZE_MAX) {
          VmLogError(NULL, "invalid maximum stack size");
          return false;
        }

        // Set the new maximum stack size
        info->maxStack = val;
      }
      break;

    case 't': // Trace stack
      {
        info->traceStack = true;
      }
      break;

    case 'I': // Enable stdin forwarding
      {
        info->forwardInput = true;
      }
      break;

    case 'O': // Enable stdout forwarding
      {
        info->forwardOutput = true;
      }
      break;

    default:
      VmLogError(NULL, "unsupported command line options "
                 "(invoke with '-h' for help)");
      return false;
    }
  }

  // Do we have extra arguments?
  if (optind != argc) {
    VmLogError(NULL, "unexpected extra arguments at end of command line.");
    return false;
  }

  // Fail if no input file is given
  if (!info->inputFileName) {
    VmLogError(NULL, "no bytecode input file (-i) has been specified.");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
static int VmMainDoIoRead(VmContext *vm)
{
  assert (vm != NULL);

  if (ferror(stdin)) {
    return -1; // Stream error pending
  }

  if (feof(stdin)) {
    return -2; // EOF pending
  }

  // Read the next character
  int ret = getchar();
  if (ret == EOF) {
    // End of file or error
    return ferror(stdin)? -1 : -2;
  }

  // Valid byte
  assert (0 <= ret && ret <= 255);
  return ret;
}

//----------------------------------------------------------------------
static bool VmMainDoIoWrite(VmContext *vm, unsigned char c)
{
  if (putchar(c) == EOF) {
    // Failed
    return false;
  }

  // Flush the stream
  fflush(stdout);

  return true;
}

//----------------------------------------------------------------------
static VmContext* VmMainCreate(VmCmdLineInfo *info)
{
  bool success = true;
  assert (info != NULL);

  // Setup input/output forwarding
  VmIoRead ioread = NULL;
  if (info->forwardInput) {
    ioread = VmMainDoIoRead;
  }

  VmIoWrite iowrite = NULL;
  if (info->forwardOutput) {
    iowrite = VmMainDoIoWrite;
  }

  // Create the virtual machine
  VmContext *vm = VmCreate(info->maxStack, NULL, ioread, iowrite);
  if (!vm) {
    VmLogError(NULL, "failed to create a virtual machine");
    success = false;
  }

  // Set the log level and log stream
  if (success) {
    if (!VmSetLogStream(vm, stderr) || !VmSetLogLevel(vm, info->logLevel)) {
      VmLogError(vm, "failed to set log stream and/or log level");
      success = false;
    }
  }

  VmLogInfo(vm, "main: bytecode: tinyvm version %u.%u",
            (unsigned) VM_VERSION_MAJOR, (unsigned) VM_VERSION_MINOR);
  VmLogInfo(vm, "main: i/o setup: stdin:%sforwarded stdout:%sforwarded",
            info->forwardInput ? "" : "not ",
            info->forwardOutput ? "" : "not ");

  // Initialize the input file
  if (success) {
    success = VmLoadFile(vm, info->inputFileName);
  }

  // Delete on errror
  if (!success) {
    VmLogError(vm, "vm initialization failed");
    VmDelete(vm);
    return NULL;
  }

  // Return the new VM
  VmLogTrace(vm, "virtual machine initialization done");
  return vm;
}

//----------------------------------------------------------------------
static void VmTraceStack(VmContext *vm, const char *prefix)
{
  VmRegisters regs;
  bool regs_valid = VmGetRegs(&regs, vm);
  size_t depth = VmStackGetUsage(vm);

  fprintf(stderr, "stack: %s ", prefix);
  if (regs_valid) {
    fprintf(stderr, "(f:0x%08x pc:0x%04x) ",
            (unsigned) regs.function, (unsigned) regs.pc);
  }

  fputc('[', stderr);

  for (size_t n = 0; n < depth; ++n) {
    if (n > 0) {
      fputc(' ', stderr);
    }

    // Mark the active frame
    if (regs_valid && n == regs.fp) {
      fputc('*', stderr);
    }

    uint32_t *value = VmStackAt(vm, n);
    if (value) {
      fprintf(stderr, "0x%x", (unsigned) *value);
    } else {
      fputs("-err-", stderr);
    }
  }

  fprintf(stderr, "]\n");
}

//----------------------------------------------------------------------
static bool VmMainRun(VmContext *vm, bool trace_stack)
{
  if (trace_stack) {
    VmTraceStack(vm, "initial");
  }

  // Main execute loop
  int result;
  do {
    // Execute a single step
    result = VmStep(vm);
    if (trace_stack) {
      VmTraceStack(vm, "step");
    }

  } while (result == 0);

  // Report the exit status
  if (result == 1) {
    VmLogInfo(vm, "vm: program terminated normally.");
    return true;

  } else {
    VmLogError(vm, "vm: program terminated abnormally (status %d)",
               result);
    return false;
  }
}

//----------------------------------------------------------------------
int main(int argc, char **argv)
{

  // Parse the command line
  VmCmdLineInfo cmdline;
  if (!VmMainParseCmdLine(&cmdline, argc, argv)) {
    return false;
  }

  // Create a new VM
  VmContext *vm = VmMainCreate(&cmdline);

  // Run the main VM
  bool success = false;
  if (vm) {
    success = VmMainRun(vm, cmdline.traceStack);
  }

  // Cleanup
  if (vm) {
    VmLogTrace(vm, "deleting the virtual machine");
    VmDelete(vm);
  }

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
