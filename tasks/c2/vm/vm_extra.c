///
/// \file
/// \brief Virtual machine calls (native utility functions)
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
static int VmcExit(VmContext *vm, uint32_t pc)
{
  // Report the exit
  VmLogInfo(vm, "vmc: pc:0x%08x: exit: bytecode program requested termination",
            (unsigned) pc);

  // Signal termination of the virtual machine
  return 1;
}

//----------------------------------------------------------------------
static int VmcAssert(VmContext *vm, uint32_t pc)
{
  // Fetch the test
  uint32_t test;
  if (!VmStackPop(&test, vm, 1)) {
    VmLogError(vm, "vmc: pc:0x%0x: assert failed to load the test operand.",
               (unsigned) pc);
    return -1;
  }

  // Fail the assertion if the test is zero
  if (test == 0) {
    VmLogError(vm, "vmc: pc:0x%08x: bytecode assertion failed.", (unsigned) pc);
    return -1;
  }

  // Let the program continue
  VmLogTrace(vm, "vmc: pc:0x%08x: bytecode assertion passed.", (unsigned) pc);
  return 0;
}

//----------------------------------------------------------------------
static int VmcRead(VmContext *vm, uint32_t pc)
{
  uint32_t inbyte = 0xFFFFFFFFU; // End of input

  // Invoke the read callback (if present)
  if (vm->ioRead) {
    int ret = (vm->ioRead)(vm);

    if (0 <= ret && ret <= 255) {
      // Got a valid input byte
      inbyte = ret;

    } else if (ret != -2) {
      // Unexpected result or error (neither a valid byte, nor an EOF)
      VmLogError(vm, "vmc: pc:0x%08x: read: i/o read callback failed (status %d)",
                 (unsigned) pc, ret);
      return -1;
    }
  }

  // Debug trace
  if (inbyte != 0xFFFFFFFFU) {
    assert (inbyte <= 255);
    VmLogDebug(vm, "vmc: pc:0x%08x: read: input 0x%02x",
               (unsigned) pc, (unsigned) inbyte);
  } else {
    VmLogDebug(vm, "vmc: pc:0x%08x: read: -end of file-",
               (unsigned) pc);
  }

  // Push the input byte
  if (!VmStackPush(vm, &inbyte, 1)) {
    VmLogError(vm, "vmc: pc:0x%08x: failed to push read result.",
               (unsigned) pc);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
static int VmcWrite(VmContext *vm, uint32_t pc)
{
  uint32_t output;
  if (!VmStackPop(&output, vm, 1)) {
    VmLogError(vm, "vmc: pc:0x%08x: failed to get output operand.",
               (unsigned) pc);
    return -1;
  }

  // Clip to 8-bit
  output &= 0xFF;
  VmLogDebug(vm, "vmc: pc:0x%08x: write: '%c'",
             (unsigned) pc,
             (0x20 <= output && output < 0x7F) ? output : '.');

  // Invoke the write callback (if present)
  if (vm->ioWrite) {
    bool ret = (vm->ioWrite)(vm, output);
    if (!ret) {
      VmLogError(vm, "vmc: pc:0x%08x: write: i/o write callback failed",
                 (unsigned) pc);
      return -1;
    }
  }

  return 0;
}

//----------------------------------------------------------------------
static int VmcAlloc(VmContext *vm, uint32_t pc)
{
  uint32_t size;
  if (!VmStackPop(&size, vm, 1)) {
    VmLogError(vm, "vmc: pc:0x%08x: alloc: failed to get object size.",
               (unsigned) pc);
    return -1;
  }

  // Search for a free object handle
  assert (VM_DYNAMIC_HANDLE_LAST < UINT32_MAX);

  uint32_t handle;
  for (handle = VM_DYNAMIC_HANDLE_FIRST; handle <= VM_DYNAMIC_HANDLE_LAST; ++handle) {
    if (!VmGetObject(vm, handle)) {
      break;
    }
  }

  if (handle == VM_DYNAMIC_HANDLE_LAST + 1) {
    VmLogError(vm, "vmc: pc:0x%08x: alloc: out of dynamic object handles.",
               (unsigned) pc);
    return -1;
  }

  // And allocate
  if (!VmCreateObject(vm, handle, VM_QUALIFIER_DATA, NULL, size)) {
    VmLogError(vm, "vmc: pc:0x%08x: alloc: failed to dynamically allocate %u byte.",
               (unsigned) pc, (unsigned) size);
    return -1;
  }

  VmLogDebug(vm, "vmc: pc:0x%08x: alloc: new object 0x%08x: size:%u",
             (unsigned) pc, (unsigned) handle, (unsigned) size);

  // Push the new handle
  if (!VmStackPush(vm, &handle, 1)) {
    VmLogError(vm, "vmc: pc:0x%08x: alloc: failed to push the result handle.",
               (unsigned) pc);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcVmc(VmContext *vm, uint32_t pc, uint32_t imm)
{
  int result;

  switch (imm) {
  case VMC_EXIT: result = VmcExit(vm, pc); break;
  case VMC_ASSERT: result = VmcAssert(vm, pc); break;
  case VMC_READ: result = VmcRead(vm, pc); break;
  case VMC_WRITE: result = VmcWrite(vm, pc); break;
  case VMC_ALLOC: result = VmcAlloc(vm, pc); break;

  default:
    VmLogError(vm, "vmc: unimplemented virtual machine call 0x%08x",
               (unsigned) imm);
    result = -1;
    break;
  }

  return result;
}
