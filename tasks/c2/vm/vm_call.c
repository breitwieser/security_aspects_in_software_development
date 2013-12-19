///
/// \file
/// \brief Subroutine calls and branches.
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
bool VmGetFunctionHeader(uint8_t *num_args, uint8_t *num_results,
                         uint8_t *num_locals, VmObject *target)
{
  assert (target != NULL);

  VmContext *vm = VmGetContext(target);

 // Extract function details from the header
  MemView view;
  if (!VmAccessObject(&view, target)) {
    VmLogError(vm, "call: function 0x%08x: inaccessible bytcode",
               (unsigned) VmGetHandle(target));
  }

  // Extract the argument count
  if (num_args) {
    if (!MemGetByte(num_args, &view, VM_FUNCTION_NUM_ARGS_OFFSET)) {
      VmLogError(vm, "call: function 0x%08x: failed to get argument count",
                 (unsigned) VmGetHandle(target));
      return false;
    }
  }

  // Extract the results count
  if (num_results) {
    if (!MemGetByte(num_results, &view, VM_FUNCTION_NUM_RESULTS_OFFSET)) {
      VmLogError(vm, "call: function 0x%08x: failed to get argument count",
                 (unsigned) VmGetHandle(target));
      return false;
    }
  }

  // Extract the local variables count
  if (num_locals) {
    if (!MemGetByte(num_locals, &view, VM_FUNCTION_NUM_LOCALS_OFFSET)) {
      VmLogError(vm, "call: function 0x%08x: failed to get argument count",
                 (unsigned) VmGetHandle(target));
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------
bool VmSetupCall(VmContext *vm, uint32_t function)
{
  assert (vm != NULL);

  // Lookup the target function
  VmObject *target = VmGetObject(vm, function);
  if (!target) {
    VmLogError(vm, "call: unresolved function 0x%08x", (unsigned) function);
    return false;
  }

  // Grab the current register context
  VmRegisters old_regs;
  if (!VmGetRegs(&old_regs, vm)) {
    VmLogError(vm, "call: failed to get current register context");
    return false;
  }

  // Extract the header information of the new function
  uint8_t num_args, num_locals;
  if (!VmGetFunctionHeader(&num_args, NULL, &num_locals, target)) {
    return false;
  }

  // The new frame pointer will be the current top of stack
  size_t new_fp = VmStackGetUsage(vm);

  // The caller has already pushed the arguments. Check the stack size.
  if (new_fp < num_args) {
    VmLogError(vm, "call: stack underflow (too few arguments for the call)");
    return false;
  }

  // Push the old function handle, program counter and frame pointer
  uint32_t saved_regs[3] = {
    old_regs.function,  // [fp+0] = old function handle
    old_regs.pc,        // [fp+1] = old program counter
    old_regs.fp         // [fp+2] = old frame pointer
  };

  if (!VmStackPush(vm, saved_regs, 3)) {
    VmLogError(vm, "call: failed to push the old register context.");
    return false;
  }

  // Reserve additional stack space for local variables (zero initialized)
  if (!VmStackPush(vm, NULL, num_locals)) {
    VmLogError(vm, "call: failed to reserve stack space for local variables");
    return false;
  }

  // Prepare the new register context
  VmRegisters new_regs = {
    .function = function,
    .pc = 0,
    .fp = new_fp
  };

  if (!VmSetRegs(vm, &new_regs)) {
    VmLogError(vm, "call: failed to setup the register context for the call");
    return false;
  }

  // Setup done
  VmLogTrace(vm, "call: prepared call to function 0x%08x (fp:0x%08x sp:0x%08x)",
             (unsigned) function, (unsigned) new_fp,
             (unsigned) VmStackGetUsage(vm));
  return true;
}

//----------------------------------------------------------------------
int VmBcCall(VmContext *vm, uint32_t pc, uint32_t imm)
{
  // Setup the new call
  if (!VmSetupCall(vm, imm)) {
    VmLogError(vm, "bc: call: failed to setup a new call");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcJmp(VmContext *vm, uint32_t pc, uint32_t imm)
{
  // The branch offset is encoded in a 16-bit signed value. We exploit
  // the int16_t cast to do the sign extension for us.
  int32_t offset = (int16_t) imm;

  /// \todo Check that the jump does not overflow the program counter.

  // Adjust the target PC
  vm->pc = pc + 1 + offset;
  return 0;
}

//----------------------------------------------------------------------
int VmBcBt(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t test;
  if (!VmStackPop(&test, vm, 1)) {
    VmLogError(vm, "vm: bt: failed to fetch the test operand.");
    return -1;
  }

  // Evalute the test condition
  if (test != 0) {
    // Tail-call to VmBcJmp (if branch is taken)
    return VmBcJmp(vm, pc, imm);
  }

  // Branch not taken
  VmLogDebug(vm, "vm: bt: pc:0x%08x: branch not taken", (unsigned) pc);
  return 0;
}

//----------------------------------------------------------------------
int VmBcBf(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t test;
  if (!VmStackPop(&test, vm, 1)) {
    VmLogError(vm, "vm: bf: failed to fetch the test operand.");
    return -1;
  }

  // Evalute the test condition
  if (test == 0) {
    // Tail-call to VmBcJmp (if branch is taken)
    return VmBcJmp(vm, pc, imm);
  }

  // Branch not taken
  VmLogDebug(vm, "vm: bf: pc:0x%08x: branch not taken", (unsigned) pc);
  return 0;
}

//----------------------------------------------------------------------
static bool VmGetCallersRegs(VmRegisters *regs, VmContext *vm)
{
  // Get the stack locations for the saved frame-pointer,
  // function handle and program counter
  uint32_t *old_function = VmStackAt(vm, vm->fp + 0);
  uint32_t *old_pc = VmStackAt(vm, vm->fp + 1);
  uint32_t *old_fp = VmStackAt(vm, vm->fp + 2);
  if (!old_function || !old_pc || !old_fp) {
    return false;
  }

  // Save them
  regs->function = *old_function;
  regs->pc = *old_pc;
  regs->fp = *old_fp;

  VmLogDebug(vm, "bc: getcaller: function:0x%08x pc:0x%08x fp:0x%08x",
             (unsigned) regs->function, (unsigned) regs->pc,
             (unsigned) regs->fp);
  return true;
}

//----------------------------------------------------------------------
int VmBcRet(VmContext *vm, uint32_t pc, uint32_t imm)
{
  // Determine the caller's context
  VmRegisters caller;
  if (!VmGetCallersRegs(&caller, vm)) {
    VmLogError(vm, "bc: ret: failed to get caller registers.");
    return -1;
  }

  // Grab the active stack frame size
  size_t sp = VmStackGetUsage(vm);
  if (sp < vm->fp || (sp - vm->fp) < 3) {
    VmLogError(vm, "bc: ret: stack frame corruption (bad fp/sp)");
    return -1;
  }

  // Load the active function header
  uint8_t num_args, num_locals, num_results;
  if (!VmGetFunctionHeader(&num_args, &num_results, &num_locals, vm->function)) {
    VmLogError(vm, "bc: ret: failed to load active function header.");
    return -1;
  }

  // Copy the results into place
  if (num_results > 0) {
    size_t result_top = sp - (num_args + num_locals + 3);

    for (size_t n = 0; n < num_results; ++n) {
      uint32_t *src = VmStackAt(vm, sp - 1 - n);
      if (!src) {
        VmLogError(vm, "bc: ret: failed to get result source slot %u", n);
        return -1;
      }

      uint32_t *dst = VmStackAt(vm, result_top - 1 - n);
      if (!dst) {
        VmLogError(vm, "bc: ret: failed to get result desination slot %u", n);
        return -1;
      }

      // Assign (and trace)
      VmLogDebug(vm, "bc: ret: result 0x%08x [%u] <= [%u]",
                 *src, result_top - 1 - n, sp - 1 - n);
      *dst = *src;
    }
  }

  // Dispose the excess stack frames
  if (!VmStackPop(NULL, vm, num_args + num_locals + 3)) {
    VmLogError(vm, "bc: ret: failed to dispose excess stack slots.");
    return -1;
  }

  // Restore the caller context
  if (!VmSetRegs(vm, &caller)) {
    VmLogError(vm, "bc: ret: failed to restore the caller frame.");
    return -1;
  }

  VmLogDebug(vm, "bc: ret: to caller: function:0x%08x pc:0x%08x fp:0x%08x sp:0x%08x",
             (unsigned) VmGetHandle(vm->function),
             (unsigned) vm->pc, (unsigned) vm->fp,
             (unsigned) VmStackGetUsage(vm));
  return 0;
}
