///
/// \file
/// \brief Virtual machine stack handling.
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
bool VmStackPush(VmContext *vm, const uint32_t *values, size_t count)
{
  /// \todo Implement your stack push operation
	if(!vm){
		return false;
	}
	if(count == 0){
		return true;
	}

	size_t new_size = vm->stack_size + count;
	//overflow check
	if(SIZE_MAX - count < vm->stack_size || new_size>vm->max_stack){
		return false;
	}

	uint32_t* start_p = vm->stack+vm->stack_size;
	if(values){
		//printf("values pointer %p\n", values);
		for(size_t i = 0; i < count; i++)
			start_p[i] = values[i];
	}else{
	    for(size_t i = 0; i < count; i++)
			start_p[i] = 0;
	}
	vm->stack_size = new_size;
	return true;
}

//----------------------------------------------------------------------
bool VmStackPop(uint32_t *values, VmContext *vm, size_t count)
{
  /// \todo Implement your stack push operation
	if(!vm){
		return false;
	}

	if(count > vm->stack_size){
		return false;
	}

	if(count==0){
		return true;
	}

	//copy values
	uint32_t* start_p = vm->stack+vm->stack_size-1;
	if(values){
		for(size_t i=0;i < count;i++){
			values[i] = *(start_p-i);
		}
	}

	vm->stack_size = vm->stack_size-count;

  return true;
}

//----------------------------------------------------------------------
size_t VmStackGetUsage(VmContext *vm)
{
  /// \todo Implement your stack usage function.
	if(!vm){
		return 0;
	}
	return vm->stack_size;
}

//----------------------------------------------------------------------
uint32_t* VmStackAt(VmContext *vm, size_t index)
{
  /// \todo Implement your stack "peek" function.
  ///  (note that index=0 is the bottom of the stack)
	if(!vm){
		return NULL;
	}
	if(index < vm->stack_size){
		return (vm->stack+index);
	}
	return NULL;
}


//----------------------------------------------------------------------
int VmBcImm32(VmContext *vm, uint32_t pc, uint32_t imm)
{
  if (!VmStackPush(vm, &imm, 1)) {
    VmLogError(vm, "bc: imm32: failed to push constant value");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcHandle(VmContext *vm, uint32_t pc, uint32_t imm)
{
  // Sanity check: Are we loading a valid handle?
  if (imm != VM_NULL_HANDLE && !VmGetObject(vm, imm)) {
    VmLogError(vm, "bc: handle: refusing to load undefined handle 0x%08x",
               (unsigned) imm);
    return -1;
  }

  // We can reuse the VmBcImm32 implementation here
  return VmBcImm32(vm, pc, imm);
}

//----------------------------------------------------------------------
int VmBcPop(VmContext *vm, uint32_t pc, uint32_t imm)
{
  if (!VmStackPop(NULL, vm, 1)) {
    VmLogError(vm, "bc: pop: failed to discard the top of stack value");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcPeek(VmContext *vm, uint32_t pc, uint32_t imm)
{
  // Peek counts its index relative to the top of stack
  size_t stack_depth = VmStackGetUsage(vm);
  if (stack_depth <= imm) {
    VmLogError(vm, "bc: peek: bad stack index");
    return -1;
  }

  size_t peek_index = stack_depth - imm - 1;
  uint32_t *value = VmStackAt(vm, peek_index);
  if (!value) {
    VmLogError(vm, "bc: peek: inaccessible source location.");
    return -1;
  }

  // Push the resulting value
  if (!VmStackPush(vm, value, 1)) {
    VmLogError(vm, "bc: peek: failed to push the result");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcSwap(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t tmp[2];

  // Pop the two top of stack values
  if (!VmStackPop(tmp, vm, 2)) {
    VmLogError(vm, "bc: swap: failed to get the old top of stack values");
    return -1;
  }

  // Push the new top of stack values
  // (the "swap" operation is implied by the definition of push and pop)
  if (!VmStackPush(vm, tmp, 2)) {
    VmLogError(vm, "bc: swap: failed to push the new top of stack values");
    return -1;
 }

  return 0;
}
