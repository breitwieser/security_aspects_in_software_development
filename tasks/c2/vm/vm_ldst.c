///
/// \file
/// \brief Load and store bytecodes
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
/// \brief Returns the stack location of a local variable.
///
/// This function computes the stack index of a local variable or
/// function argument and uses \ref VmStackAt to get a pointer to the
/// stack location.
///
/// \param vm is the parent virtual machine.
///
/// \param index is the stack index of the local variable or argument.
///    (indices between 0 and 127 refer to local variables, 128 to 256
///     refer to arguments).
///
/// \return A pointer to requested stack slot.
///
/// \return NULL on error.
static uint32_t* VmGetLocal(VmContext *vm, uint8_t index)
{
  // Fetch information about the active function
  uint8_t num_locals, num_args;
  if (!VmGetFunctionHeader(&num_args, NULL, &num_locals, vm->function)) {
    VmLogError(vm, "bc: getlocal: failed to the function header.");
    return NULL;
  }

  uint32_t *value = NULL;

  if (index < VM_MAX_LOCALS) {
    // Index refers to a local variable (local variables start and [fp+3])
    if (index < num_locals) {
      value = VmStackAt(vm, vm->fp + 3 + index);
    }

  } else if (index >= VM_MAX_LOCALS && index <= (VM_MAX_LOCALS + VM_MAX_ARGS)) {
    // Index refers to an argument (arguments and at [fp-1])
    index -= VM_MAX_LOCALS;
    if (index < num_args) {
      value = VmStackAt(vm, vm->fp - num_args + index);
    }
  }

  return value;
}

//----------------------------------------------------------------------
int VmBcLdVar(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t *value = VmGetLocal(vm, imm);
  if (!value) {
    VmLogError(vm, "bc: ldvar: failed to get stack slot for variable %u",
               (unsigned) imm);
    return -1;
  }

  if (!VmStackPush(vm, value, 1)) {
    VmLogError(vm, "bc: ldvar: failed to load variable %u to stack",
               (unsigned) imm);
    return -1;

  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcStVar(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t *value = VmGetLocal(vm, imm);
  if (!value) {
    VmLogError(vm, "bc: stvar: failed to get stack slot for variable %u",
               (unsigned) imm);
    return -1;
  }

  if (!VmStackPop(value, vm, 1)) {
    VmLogError(vm, "bc: stvar: failed to store stack operand to variable %u",
               (unsigned) imm);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
static bool VmDerefObject(MemView *view, size_t *offset, VmContext *vm)
{
  assert (view != NULL);
  assert (offset != NULL);
  assert (vm != NULL);

  // Load stack operands (handle, index)
  uint32_t args[2];
  if (!VmStackPop(args, vm, 2)) {
    VmLogError(vm, "bc: deref: failed to load handle and/or index from stack");
    return false;
  }

  // Dereference the handle (in args[0]) to an object
  VmObject *obj = VmGetObject(vm, args[0]);
  if (!obj) {
    VmLogError(vm, "bc: deref: failed to resolve handle 0x%08x",
               (unsigned) args[0]);
    return false;
  }

  // Setup the memory view for the object
  if (!VmAccessObject(view, obj)) {
    VmLogError(vm, "bc: deref: failed to access object 0x%08x (native: %p)",
               (unsigned) args[0], (void *) obj);
    return false;
  }

  // Our offset is the index from the stack
  *offset = args[1];

  VmLogDebug(vm, "bc: deref: dereferenced object 0x%08x (native: %p)",
             (unsigned) args[0], (void *) obj);
  return true;
}

//----------------------------------------------------------------------
/// \brief Common implementation for LDW/LDB
static int VmLd(VmContext *vm, bool word)
{
  // Dereference the object
  MemView view;
  size_t offset;
  if (!VmDerefObject(&view, &offset, vm)) {
    VmLogError(vm, "bc: ld: failed to dereference source object");
    return -1;
  }

  /// \todo Bytecode programs should not be able to read the content
  /// of access protected objects. (objects with a VM_QUALIFIER_PROTECTED
  ///  qualifier)

  // Load the value
  uint32_t value = 0;

  if (word) {
    // Word access
    if (!MemGetWord(&value, &view, offset)) {
      VmLogError(vm, "bc: ld: failed to load word at offset %u",
                 (unsigned) offset);
      return -1;
    }

  } else {
    // Byte access
    uint8_t tmp;
    if (!MemGetByte(&tmp, &view, offset)) {
      VmLogError(vm, "bc: ld: failed to load byte at offset %u",
                 (unsigned) offset);
      return -1;
    }

    value = tmp;
  }

  // Push the result
  if (!VmStackPush(vm, &value, 1)) {
    VmLogError(vm, "bc: ld: failed to push the result");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
/// \brief Common implementation for STW/STB
static int VmSt(VmContext *vm, bool word)
{
  // Dereference the object
  MemView view;
  size_t offset;
  if (!VmDerefObject(&view, &offset, vm)) {
    VmLogError(vm, "bc: st: failed to dereference source object");
    return -1;
  }

  /// \todo Bytecode programs should not be able to write the content
  /// of inmutable objects. (objects with a VM_QUALIFIER_INMUTABLE
  /// qualifier)

  // Get the value to store
  uint32_t value;
  if (!VmStackPop(&value, vm, 1)) {
    VmLogError(vm, "bc: st: failed to fetch the stack operand");
    return -1;
  }

  if (word) {
    // Word access
    if (!MemSetWord(&view, offset, value)) {
      VmLogError(vm, "bc: st: failed to store word at offset %u",
                 (unsigned) offset);
      return -1;
    }

  } else {
    // Byte access (truncate to 8 bit)
    if (!MemSetByte(&view, offset, (value & 0x000000FFU))) {
      VmLogError(vm, "bc: st: failed to store byte at offset %u",
                 (unsigned) offset);
    }
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcLdW(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmLd(vm, true);
}

//----------------------------------------------------------------------
int VmBcStW(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmSt(vm, true);
}

//----------------------------------------------------------------------
int VmBcLdB(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmLd(vm, false);
}

//----------------------------------------------------------------------
int VmBcStB(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmSt(vm, false);
}
