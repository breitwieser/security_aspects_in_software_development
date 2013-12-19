///
/// \file
/// \brief Writes assembler output to a byte buffer.
///
#include "assembler_imp.h"

//----------------------------------------------------------------------
static bool AsmWriteBufferCallback(void *p_buffer, const void *data,
                                   size_t len)
{
  return BufferAppend(p_buffer, data, len);
}

//----------------------------------------------------------------------
bool AsmWriteToBuffer(Buffer *dest, Assembler *assembler)
{
  if (!dest || !assembler) {
    return false;
  }

  return AsmWriteOutput(assembler, AsmWriteBufferCallback, dest);
}
