///
/// \file
/// \brief Implementation details of the tiny virtual machine.
///
/// \note The data structures and functions defined in this file
///  are implementation details of the tinyvm virtual machine. You
///  are modify them as you want. The existing implementation should
///  provide sane default.
///
#ifndef TINYVM_INTERNAL_H
#define TINYVM_INTERNAL_H

// Include the definitions from the public API
#include "tinyvm.h"

// Additional headers
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//----------------------------------------------------------------------
/// \brief Callback function type for bytecode handlers.
///
/// This function pointer type is used in bytecode handler callbacks.
/// The tables in \c bytecodes.c are used to define the mapping between
/// bytecodes and the handler function. Decoding and dispatching of
/// bytecodes is done by as part of the \ref VmStep function.
///
/// \param[in,out] vm is the virtual machine context executing the bytecode.
///
/// \param[in] pc is the program counter location of the first byte of this
///              bytecode.
///
/// \param[in] imm is the immediate operand of the bytecode. The value of
///            this operand depends on the decoded bytecode format.
///
/// \return +1 if the bytecode was executed successfully (and the virtual
///            machine should terminate gracefully).
///
/// \return 0 if the bytecode was executed successfully.
///
/// \return -1 if execution of the bytecode failed.
///
typedef int (*VmByteCodeCallback)(VmContext *vm, uint32_t pc, uint32_t imm);

typedef enum VmByteCodeFormat {
  /// \brief Single byte instruction.
  ///
  /// Single byte instructions just consist of the opcode without
  /// any immediate operands.
  VM_FORMAT_SIMPLE = 0,

  /// \brief Instruction with a single-byte immediate operand.
  ///
  /// This instruction format consists of an opcode byte and a
  /// single operand byte. The operand byte will be passed as \c imm
  /// value to the bytecode callback.
  VM_FORMAT_BYTE = 1,

  /// \brief Instruction with a half-word (16-bit) immediate operand.
  ///
  /// This instruction format consists of an opcode byte and a
  /// single 16-bit immediate operand. The immediate operand will be
  /// passed as \c imm value to the bytecode callback.
  VM_FORMAT_HALF = 2,

  /// \brief Instruction with a word (32-bit) immediate operand.
  ///
  /// This instruction format consists of an opcode byte and a
  /// single 32-bit immediate word. The 32-bit immediate will be
  /// passed as \c imm value to the bytecode callback.
  VM_FORMAT_WORD = 3,
} VmByteCodeFormat;

/// \brief Decode information for a virtual machine bytecode.
///
/// The \c vm_bytecodes.c file contains a decode table with one entry
/// for each defined virtual machine bytecode.
///
typedef struct VmDecodeInfo {
  /// \brief Human readable name of the bytecode. (for debugging)
  const char *name;

  /// \brief Format of this bytecode.
  ///
  /// The format of a bytecode determines the length and the
  /// immdiate operands of a bytecode.
  VmByteCodeFormat format;

  /// \brief Handler function for the bytecode.
  ///
  /// This field defines the callback function called by a bytecode
  /// handler.
  VmByteCodeCallback handler;
} VmDecodeInfo;

//----------------------------------------------------------------------
struct VmContext
{
  /// \brief Log stream of this virtual machine instance.
  FILE *logStream;

  /// \brief Log level filter of this virtual machine instance.
  VmLogLevel logLevel;

  /// \brief The object with the currently active function.
  VmObject* function;

  /// \brief The virtual program counter.
  uint32_t pc;

  /// \brief The virtual frame pointer.
  uint32_t fp;

  /// \brief I/O read callback (for implementing \ref VMC_READ)
  VmIoRead ioRead;

  /// \brief I/O write callback (for implementing \ref VMC_WRITE)
  VmIoWrite ioWrite;

  /// \brief Application data pointer returned by \ref VmGetAppData.
  void* appdata;

  /// \todo Add additional fields for your VM state.
  /// You will need at least:
  /// \li A data structure to manage your VmObject instances
  ///     (for example a linked list or array)
  ///
  /// \li A data structure to manage your operand stack
  ///     (for example an array)
  ///
  /// \li Fields to hold your virtual stack pointer, virtual program
  ///     counter, ...

  uint32_t *stack;
  size_t stack_size;
  size_t max_stack;

  VmObject **objs;
  size_t objs_size;

};

//----------------------------------------------------------------------
struct VmObject
{
  /// \brief Parent virtual machine context
  VmContext *parent;

  /// \brief The handle of this object.
  uint32_t handle;

  /// \brief Qualifiers of this object.
  VmQualifiers qualifiers;

  /// \todo Add the fields specific to your implementation here.

  Buffer* buffer;
  bool big_endian;
};

/// \todo Add your own helper functions, data structures, constants, ...

/// \internal
/// \brief Prepares the stack frame for a call to a given function.
///
/// This function prepares a virtual machine stackframe for a call
/// to a given (virtual) function.
///
/// \param vm is the target virtual machine for the call setup.
///
/// \param function is the target function to be called.
///
/// \return \c true if the call was prepared successfully. (The
///   next \ref VmStep will execute the first instruction of the
///   new function).
///
/// \return \c false if the call could not be prepared.
bool VmSetupCall(VmContext *vm, uint32_t function);

/// \internal
/// \brief Gets information about a function.
///
/// \param[out] num_args is an optional output argument receiving
///     the number of arguments of the function.
///
/// \param[out] num_result is an optional output argument receiving
///     the number of results of the function.
///
/// \param[out] num_locals is an optional output argument receiving
///     the number of local variables of the function.
///
/// \param[in] target is the function object to parse.
///
/// \return \c true if the information could be retrieved.
///
/// \return \c false on errror.
bool VmGetFunctionHeader(uint8_t *num_args, uint8_t *num_results,
                         uint8_t *num_locals, VmObject *target);

//----------------------------------------------------------------------
// Bytecode handlers
//

int VmBcImm32(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcHandle(VmContext *vm, uint32_t pc, uint32_t imm);

int VmBcCall(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcJmp(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcBt(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcBf(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcRet(VmContext *vm, uint32_t pc, uint32_t imm);

int VmBcPop(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcPeek(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcSwap(VmContext *vm, uint32_t pc, uint32_t imm);

int VmBcCmpEq(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcCmpNe(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcUCmpLt(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcUCmpLe(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcUCmpGe(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcUCmpGt(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcSCmpLt(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcSCmpLe(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcSCmpGe(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcSCmpGt(VmContext *vm, uint32_t pc, uint32_t imm);

int VmBcLdVar(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcStVar(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcLdW(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcStW(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcLdB(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcStB(VmContext *vm, uint32_t pc, uint32_t imm);

int VmBcAdd(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcSub(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcMul(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcUDiv(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcSDiv(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcShl(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcShr(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcAsr(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcRor(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcAnd(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcOr(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcXor(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcNot(VmContext *vm, uint32_t pc, uint32_t imm);
int VmBcNeg(VmContext *vm, uint32_t pc, uint32_t imm);

int VmBcVmc(VmContext *vm, uint32_t pc, uint32_t imm);
#endif // TINYVM_INTERNAL_H
