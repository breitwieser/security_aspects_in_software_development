///
/// \file
/// \brief Virtual machine context handling
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
VmContext* VmCreate(size_t max_stack, void *appdata,
                    VmIoRead ioread, VmIoWrite iowrite)
{
  size_t alloc_size = sizeof(VmContext);

  // Allocate
  VmContext *vm = malloc(alloc_size);
  if (!vm) {
    return NULL;
  }

  // Initialize
  vm->logStream = VmGetLogStream(NULL);
  vm->logLevel = VmGetLogLevel(NULL);

  // I/O callbacks (and context) for VMC_READ/VMC_WRITE
  vm->appdata = appdata;
  vm->ioRead = ioread;
  vm->ioWrite = iowrite;

  // Currently no calls pending
  vm->function = NULL;
  vm->pc = 0;
  vm->fp = 0;

  /// \todo Add your own initialization code

  return vm;
}

//----------------------------------------------------------------------
void* VmGetAppData(VmContext *vm)
{
  if (!vm) {
    return NULL;
  }

  return vm->appdata;
}


//----------------------------------------------------------------------
void VmDelete(VmContext *vm)
{
  if (vm) {
    /// \todo Add your cleanup code here

    memset(vm, 0, sizeof(VmContext));
    free(vm);
  }
}

//----------------------------------------------------------------------
VmObject* VmGetObject(VmContext *vm, uint32_t handle)
{
  if (!vm || handle == VM_NULL_HANDLE) {
    return NULL;
  }

  /// \todo Add your own code to lookup an object (by handle) in the
  ///   given VM context
  return NULL;
}

//----------------------------------------------------------------------
bool VmGetRegs(VmRegisters *regs, VmContext *vm)
{
  if (!vm || !regs) {
    return false;
  }

  // Return the execution state
  regs->function = VmGetHandle(vm->function);
  regs->pc = vm->pc;
  regs->fp = vm->fp;
  return true;
}

//----------------------------------------------------------------------
bool VmSetRegs(VmContext *vm, const VmRegisters *regs)
{
  if (!vm || !regs) {
    return false;
  }

  // Set the new execution state
  vm->function = VmGetObject(vm, regs->function);
  vm->pc = regs->pc;
  vm->fp = regs->fp;
  return true;
}
