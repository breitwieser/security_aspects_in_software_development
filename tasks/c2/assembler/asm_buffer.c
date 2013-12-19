///
/// \file
/// \brief Assembler "wrapper" for byte buffers
///
#include "assembler_imp.h"

//----------------------------------------------------------------------
void AsmBufferInit(AsmBuffer *buf, bool is_bigendian)
{
  if (buf) {
    buf->isBigEndian = is_bigendian;
    buf->storage = NULL;
  }
}

//----------------------------------------------------------------------
void AsmBufferClear(AsmBuffer *buf)
{
  if (buf) {
    BufferFree(buf->storage);
    buf->storage = NULL;
    buf->isBigEndian = false;
  }
}

//----------------------------------------------------------------------
static bool AsmBufferPrepare(AsmBuffer *buf)
{
  if (!buf) {
    return false; // Bad arguments
  }

  // Initialize the storage (if needed)
  if (!buf->storage) {
    buf->storage = BufferCreate();
    if (!buf->storage) {
      return false; // Out of memory
    }
  }

  return true;
}

//----------------------------------------------------------------------
bool AsmBufferAppend(AsmBuffer *buf, const void *data, size_t length)
{
  if (!AsmBufferPrepare(buf)) {
    return false; // Failed to prepare the storage buffer
  }

  // Append to the buffer
  return BufferAppend(buf->storage, data, length);
}

//----------------------------------------------------------------------
bool AsmBufferSkip(AsmBuffer *buf, unsigned char value, size_t length)
{
  if (!AsmBufferPrepare(buf)) {
    return false; // Failed to prepare the storage buffer
  }

  // Grow the buffer
  return BufferSkip(buf->storage, value, length);
}

//----------------------------------------------------------------------
bool AsmBufferAppendByte(AsmBuffer *buf, unsigned char c)
{
  return AsmBufferAppend(buf, &c, 1);
}

//----------------------------------------------------------------------
bool AsmBufferAppendOffset(AsmBuffer *buf, uint32_t value)
{
  if (!buf) {
    return false;
  }

  uint8_t data[2] = {
    ASM_INIT_HALF(value, buf->isBigEndian)
  };

  return AsmBufferAppend(buf, data, sizeof(data));
}

//----------------------------------------------------------------------
bool AsmBufferAppendWord(AsmBuffer *buf, uint32_t value)
{
  if (!buf) {
    return false;
  }

  uint8_t data[4] = {
    ASM_INIT_WORD(value, buf->isBigEndian),
  };

  return AsmBufferAppend(buf, data, sizeof(data));
}


//----------------------------------------------------------------------
void* AsmBufferBytes(AsmBuffer *buf)
{
  size_t length = AsmBufferLength(buf);
  if (!length) {
    return NULL; // Empty buffer (or bad arguments)
  }

  // Return the raw buffer data
  return BufferGetBytes(buf->storage, 0, length);
}

//----------------------------------------------------------------------
size_t AsmBufferLength(const AsmBuffer *buf)
{
  if (!buf) {
    return 0;
  }

  return BufferGetLength(buf->storage);
}

//----------------------------------------------------------------------
bool AsmBufferIsBigEndian(const AsmBuffer *buffer)
{
  if (!buffer) {
    return false;
  }

  return buffer->isBigEndian;
}
