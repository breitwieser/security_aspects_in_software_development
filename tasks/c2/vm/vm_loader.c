///
/// \file
/// \brief Bytecode image loader.
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
/// \brief Loads a file into a byte buffer.
///
/// This helper function opens a normal file, and tries to load the
/// entire contents into a byte buffer. It only works for regular files
/// and symlinks to regular files. Trying to open a special files (like
/// device files) will fail, if the special file does not support seeking.
///
/// \param vm is the virtual machine context to be used for logging.
///
/// \param filename is the name of the input file.
///
/// \return A new byte buffer containing the file content.
///
/// \return NULL in case of an error (for example if an out of memory
///   condition occurred, or if file I/O failed).
static Buffer* VmLoadFileToBuffer(VmContext *vm, const char *filename)
{
  assert (vm != NULL);
  assert (filename != NULL);

  // Open the input file
  FILE *file = fopen(filename, "rb");
  if (!file) {
    VmLogError(vm, "bytecode: failed to open file '%s' for reading", filename);
    return false;
  }

  // Seek to the end of the input file
  bool success = true;
  if (fseek(file, 0, SEEK_END) == -1) {
    VmLogError(vm, "bytecode: failed to seek to end of input file '%s'", filename);
    success = false;
  }

  // Determine the file length
  long length = 0;
  if (success) {
    length = ftell(file);
    if (length < 0) {
      VmLogError(vm, "bytecode: failed to determine length of input file '%s'", filename);
      success = false;
    }
  }

  // Create a buffer for holding the file content
  void *dst = NULL;

  Buffer *buffer = BufferCreate();
  if (BufferSkip(buffer, 0, length)) {
    dst = BufferGetBytes(buffer, 0, length);
  }

  if (!dst) {
    VmLogError(vm, "bytecode: failed to allocate a %u byte memory buffer for input file '%s'.",
               (unsigned) length, filename);
    success = false;
  }

  // Load the file content into our buffer
  if (success) {
    // Rewind the input file
    rewind(file);

    // And load it
    if (fread(dst, length, 1, file) != 1) {
      VmLogError(vm, "bytecode: failed to read %u bytes from input file '%s' into the memory buffer.",
                 (unsigned) length, filename);
      success = false;
    }
  }

  // Dispose the buffer on error
  if (!success && buffer) {
    BufferFree(buffer);
    buffer = NULL;
  }

  // Close input file
  if (file) {
    fclose(file);
  }

  return buffer;
}

//----------------------------------------------------------------------
bool VmLoadFile(VmContext *vm, const char *filename)
{
  if (!vm || !filename) {
    return false;
  }

  // Load the file blob (using the LoadFileToBuffer helper)
  Buffer *buffer = VmLoadFileToBuffer(vm, filename);
  if (!buffer) {
    return false;
  }

  // Parse the bytecodes and instantiate objects
  bool success = VmLoadByteCode(vm, buffer);

  // Dispose the temporary buffer
  BufferFree(buffer);
  return success;
}


//----------------------------------------------------------------------
bool VmLoadByteCode(VmContext *vm, Buffer *source)
{
  /// \brief Implement your byte code loader
  ///
  /// Your loader should parse the given byte buffer and create virtual
  /// code and data machine objects. The format of the bytecode input is
  /// discussed in \c bytecodes.h. To avoid code duplication it is
  /// advisable to use the \ref VmCreateObject function to instantiate
  /// the objects.
  ///
  /// Hint: Endianess of the incoming bytecode blob can be determined
  /// by loading the first word and checking if you read \ref VM_TAG_UNIT
  /// or its byte-order reversed version.
  ///
  /// Hint: You can use the VmSetupCall function (in \c vm_call.c) to
  /// setup the stack frame for the "main" function of the loaded program.
  /// (The handle of object containing the function is stored in the
  ///  "entry" field of the bytecode header; cf. \c bytecodes.h)
  return false;
}
