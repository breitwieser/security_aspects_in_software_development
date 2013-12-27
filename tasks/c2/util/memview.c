///
/// \file
/// \brief Simple memory view abstraction
///
#include "memview.h"

//----------------------------------------------------------------------
bool MemInit(MemView *view, MemViewFlags flags,
             Buffer *buffer, size_t base, size_t length)
{
  if(view == NULL || buffer == NULL)
    return false;
  if(BufferGetLength(buffer) <= base + length)
    return false;
  view->flags = flags;
  view->buffer = buffer;
  view->base = base;
  view->length = length;
  return true;
}

//----------------------------------------------------------------------
size_t MemGetLength(const MemView *view)
{
  if (!view) {
    return 0;
  }
  return view->length;
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
  return BufferGetBytes(view->buffer, view->base+offset, length);
}

//----------------------------------------------------------------------
bool MemGetByte(uint8_t *z, MemView *view, size_t offset)
{
  uint8_t* c = BufferGetBytes(view->buffer, view->base+offset, 1);
  if(c == NULL)
    return false;
  *z = *c;
  return true;
}

//----------------------------------------------------------------------
bool MemSetByte(MemView *view, size_t offset, uint8_t value)
{
  if(MemIsReadOnly(view))
    return false;
  uint8_t* c = BufferGetBytes(view->buffer, view->base+offset, 1);
  if(c == NULL)
    return false;
  *c = value;
  return true;
}

//----------------------------------------------------------------------
bool MemGetHalf(uint16_t *z, MemView *view, size_t offset)
{
  uint8_t* c = BufferGetBytes(view->buffer, view->base+offset, 2);
  if(c == NULL)
    return false;
  if(MemIsBigEndian(view))
    *z = (*c) | *(c+1) << 8;
  else
    *z = (*c) << 8 | *(c+1);
  return true;
}

//----------------------------------------------------------------------
bool MemSetHalf(MemView *view, size_t offset, uint16_t value)
{
  if(MemIsReadOnly(view))
    return false;
  uint8_t* c = BufferGetBytes(view->buffer, view->base+offset, 2);
  if(c == NULL)
    return false;
  if(MemIsBigEndian(view))
  {
    *c = (uint8_t)value;
    *(c+1) = (value & (255 << 8)) >> 8;
  }
  else
  {
    *c = (value & (255 << 8)) >> 8;
    *(c+1) = value;
  }
  return true;
}

//----------------------------------------------------------------------
bool MemGetWord(uint32_t *z, MemView *view, size_t offset)
{
  uint8_t* c = BufferGetBytes(view->buffer, view->base+offset, 4);
  if(c == NULL)
    return false;
  if(MemIsBigEndian(view))
    *z = (*c) | *(c+1) << 8 | *(c+2) << 16 | *(c+3) << 24;
  else
    *z = (*c) << 24 | (*c+1) << 16 | (*c+2) << 8 | *(c+3);
  return true;
}

//----------------------------------------------------------------------
bool MemSetWord(MemView *view, size_t offset, uint32_t value)
{
  if(MemIsReadOnly(view))
    return false;
  uint8_t* c = BufferGetBytes(view->buffer, view->base+offset, 2);
  if(c == NULL)
    return false;
  if(MemIsBigEndian(view))
  {
    *c     = (value);
    *(c+1) = (value & (255 << 8)) >> 8;
    *(c+2) = (value & (255 << 16)) >> 16;
    *(c+3) = (value & (255 << 24)) >> 24;
  }
  else
  {
    *c     = (value & (255 << 24)) >> 24;
    *(c+1) = (value & (255 << 16)) >> 16;
    *(c+2) = (value & (255 << 8)) >> 8;
    *(c+3) = (value);
  }
  return true;
}
