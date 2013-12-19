///
/// \file
/// \brief Object handling
///
#include "tinyvm_imp.h"


//----------------------------------------------------------------------
bool VmCreateObject(VmContext *vm, uint32_t handle, VmQualifiers qualifiers,
                    const unsigned char *content, size_t length)
{
  /// \todo Add your implementation of the object creation function
  ///       here.
  return false;
}

//----------------------------------------------------------------------
uint32_t VmGetHandle(VmObject *obj)
{
  if (!obj) {
    return VM_NULL_HANDLE;
  }

  return obj->handle;
}

//----------------------------------------------------------------------
VmContext* VmGetContext(VmObject *obj)
{
  if (!obj) {
    return NULL;
  }

  return obj->parent;
}

//----------------------------------------------------------------------
bool VmAccessObject(MemView *view, VmObject *obj)
{
  /// \todo Add your own implementation here.
  return false;
}
