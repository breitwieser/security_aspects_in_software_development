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
	if(!vm || !qualifiers || (!content && length != 0) || handle == VM_NULL_HANDLE) {
		return false;
	}

	//does object with the same handle already exist?
	if(VmGetObject(vm, handle) != NULL){
		return false;
	}

	//create object
	VmObject* o = (VmObject*) malloc(sizeof(VmObject));
	if(!o)
		return false;

	Buffer* b = BufferCreate();
	if(!b){
		free(o);
		return false;
	}
	if(!BufferAppend(b, content, length)){
		free(b);
		free(o);
		return false;
	}

	o->buffer=b;
	o->parent=vm;
	o->handle=handle;
	o->qualifiers=qualifiers;
	o->big_endian=false;

	//alloc memory in vmcontext to store object
	if (SIZE_MAX - 1 < vm->objs_size || SIZE_MAX / sizeof(VmObject*) < vm->objs_size + 1) {
		return false;
	}
	VmObject **tmp = (VmObject**) realloc(vm->objs, (vm->objs_size + 1) * sizeof(VmObject*));
	if (!tmp) {
		free(b);
		free(o);
		return false;
	}
	vm->objs = tmp;
	vm->objs_size++;

	vm->objs[vm->objs_size-1] = o;

  return true;
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
	if(!obj || !view){
		return false;
	}
	MemViewFlags flags = 0;
	flags |= obj->big_endian ? MEM_VIEW_BIGENDIAN : MEM_VIEW_NORMAL;
	flags |= obj->qualifiers == VM_QUALIFIER_CODE || obj->qualifiers == VM_QUALIFIER_INMUTABLE || obj->qualifiers == VM_QUALIFIER_PROTECTED ? MEM_VIEW_READONLY : 0;
	return MemInit(view, flags, obj->buffer, 0, BufferGetLength(obj->buffer));
}
