///
/// \file
/// \brief Simple memory view abstraction
///
#include "memview.h"

//----------------------------------------------------------------------
bool MemInit(MemView *view, MemViewFlags flags,
             Buffer *buffer, size_t base, size_t length)
{
  /// \todo Add your own implementation
  return false;
}

//----------------------------------------------------------------------
size_t MemGetLength(const MemView *view)
{
  /// \todo Add your own implementation
  return 0;
}

//----------------------------------------------------------------------
bool MemIsReadOnly(const MemView *view)
{
  if (!view) {
    return false;
  }

  return (view->flags & MEM_VIEW_READONLY) == MEM_VIEW_READONLY;
}

//----------------------------------------------------------------------
bool MemIsBigEndian(const MemView *view)
{
  if (!view) {
    return false;
  }

  return (view->flags & MEM_VIEW_BIGENDIAN) == MEM_VIEW_BIGENDIAN;
}

//----------------------------------------------------------------------
bool MemSetBigEndian(MemView *view, bool enable)
{
  if (!view) {
    return false;
  }

  if (enable) {
    view->flags |= MEM_VIEW_BIGENDIAN;
  } else {
    view->flags &= ~MEM_VIEW_BIGENDIAN;
  }

  return true;
}

//----------------------------------------------------------------------
unsigned char* MemGetBytes(MemView *view, size_t offset, size_t length)
{
  /// \todo Add your own implementation
  return NULL;
}

//----------------------------------------------------------------------
bool MemGetByte(uint8_t *z, MemView *view, size_t offset)
{
  /// \todo Add your own implementation
  return false;
}

//----------------------------------------------------------------------
bool MemSetByte(MemView *view, size_t offset, uint8_t value)
{
  /// \todo Add your own implementation
  return false;
}

//----------------------------------------------------------------------
bool MemGetHalf(uint16_t *z, MemView *view, size_t offset)
{
  /// \todo Add your own implementation
  return false;
}

//----------------------------------------------------------------------
bool MemSetHalf(MemView *view, size_t offset, uint16_t value)
{
  /// \todo Add your own implementation
  return false;
}

//----------------------------------------------------------------------
bool MemGetWord(uint32_t *z, MemView *view, size_t offset)
{
  /// \todo Add your own implementation
  return false;
}

//----------------------------------------------------------------------
bool MemSetWord(MemView *view, size_t offset, uint32_t value)
{
  /// \todo Add your own implementation
  return false;
}
