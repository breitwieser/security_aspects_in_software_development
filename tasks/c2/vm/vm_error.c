///
/// \file
/// \brief Virtual machine error reporting
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
static const char *const VM_LOG_LEVELS[] = {
  [VM_LOG_ERROR]   = "error",
  [VM_LOG_WARNING] = "warning",
  [VM_LOG_INFO]    = "info",
  [VM_LOG_TRACE]   = "trace",
  [VM_LOG_DEBUG]   = "debug",
};

//----------------------------------------------------------------------
void VmLog(VmContext *vm, VmLogLevel level, const char *message, ...)
{
  // Get the log stream and level
  FILE *stream = VmGetLogStream(vm);
  VmLogLevel limit = VmGetLogLevel(vm);

  // Check log level and stream
  if (stream && level <= limit) {

    // Find the log level prefix
    const char *prefix = NULL;
    if (level <= (sizeof(VM_LOG_LEVELS) / sizeof(VM_LOG_LEVELS[0]))) {
      prefix = VM_LOG_LEVELS[level];
    }

    // Fallback (bad or unknown level)
    if (!prefix) {
      prefix = "(unknown)";
    }

    // Print the prefix
    fprintf(stream, "vm: %s: ", prefix);

    // Print the main log message
    va_list ap;
    va_start(ap, message);
    vfprintf(stream, message, ap);
    va_end(ap);

    // Print a newline
    fputc('\n', stream);

    // And flush the log stream for more severe levels
    if (level <= VM_LOG_WARNING) {
      fflush(stream);
    }
  }
}

//----------------------------------------------------------------------
FILE* VmGetLogStream(VmContext *vm)
{
  if (!vm) {
    // No VM context, assume stderr
    return stderr;
  }

  // Return the actual log stream (may be NULL to completly disable
  // logging)
  return vm->logStream;
}

//----------------------------------------------------------------------
bool VmSetLogStream(VmContext *vm, FILE *stream)
{
  if (!vm) {
    return false;
  }

  // Set the new log stream of the VM (the log stream can be NULL
  // to completely disable logging)
  vm->logStream = stream;
  return true;
}

//----------------------------------------------------------------------
VmLogLevel VmGetLogLevel(VmContext *vm)
{
  if (!vm) {
    // No VM context, assume VM_LOG_WARNING
    return VM_LOG_WARNING;
  }

  // Return the actual log level
  return vm->logLevel;
}

//----------------------------------------------------------------------
bool VmSetLogLevel(VmContext *vm, VmLogLevel level)
{
  if (!vm || (level < VM_LOG_ERROR) || (level > VM_LOG_DEBUG)) {
    return false;
  }

  // Set the new log level
  vm->logLevel = level;
  return true;
}
