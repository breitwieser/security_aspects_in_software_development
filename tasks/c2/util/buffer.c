///
///
/// \file
/// \brief Implementation of the byte buffer.
///
#include "buffer_imp.h"

//----------------------------------------------------------------------
Buffer* BufferCreate(void)
{
  /// \todo Add your own implementation
  return NULL;
}

//----------------------------------------------------------------------
void BufferFree(Buffer* buffer)
{
  /// \todo Add your own implementation
}

//----------------------------------------------------------------------
bool BufferAppend(Buffer* buffer, const void *data, size_t len)
{
  /// \todo Add your own implementation here. Be aware that data/len may
  ///  refers to a (sub)slice of the buffer given in buffer ....
  return false;
}

//----------------------------------------------------------------------
bool BufferSkip(Buffer* buffer, unsigned char filler, size_t len)
{
  /// \todo Add your own implementation here. Be aware that data/len may
  ///  refers to a (sub)slice of the buffer given in buffer ....
  return false;
}

//----------------------------------------------------------------------
unsigned char* BufferGetBytes(Buffer* buffer, size_t offset, size_t len)
{
  /// \todo Add your own implementation here.
  return NULL;
}

//----------------------------------------------------------------------
size_t BufferGetLength(const Buffer* buffer)
{
  /// \todo Add your own implementation here.
  return 0;
}
