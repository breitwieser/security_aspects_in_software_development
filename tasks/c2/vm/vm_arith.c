///
/// \file
/// \brief Arithmetic, logic and compare operations
///
#include "tinyvm_imp.h"

//----------------------------------------------------------------------
/// \brief Operand fetch helper for binary operations.
///
static bool VmGetBinaryOperands(uint32_t *a, uint32_t *b, VmContext *vm)
{
  assert (vm != NULL);
  assert (a != NULL);
  assert (b != NULL);

  // Fetch the operands (args[0]=b, args[1]=a)
  uint32_t args[2];
  if (!VmStackPop(args, vm, 2)) {
    return false;
  }

  // Note: Operands are "reversed" on stack (a is pushed first / popped last)
  *a = args[1];
  *b = args[0];
  return true;
}

//----------------------------------------------------------------------
/// \brief Common implementation for all compares
static int VmCmp(VmContext *vm, unsigned type)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: ucmp: failed to fetch operands.");
    return -1;
  }

  // To the actual compare
  bool result = false;

  switch (type) {
    // Equality compares
  case VM_OP_CMPEQ: result = (a == b); break;
  case VM_OP_CMPNE: result = (a != b); break;

    // Unsigned compares
  case VM_OP_UCMPLT: result = (a < b); break;
  case VM_OP_UCMPLE: result = (a <= b); break;
  case VM_OP_UCMPGE: result = (a >= b); break;
  case VM_OP_UCMPGT: result = (a > b); break;

    // Signed compares
  case VM_OP_SCMPLT: result = ((int32_t) a < (int32_t) b); break;
  case VM_OP_SCMPLE: result = ((int32_t) a <= (int32_t) b); break;
  case VM_OP_SCMPGE: result = ((int32_t) a >= (int32_t) b); break;
  case VM_OP_SCMPGT: result = ((int32_t) a > (int32_t) b); break;

  default: // Should never happen in production
    VmLogError(vm, "bc: ucmp: unsupported compare (type %u)", type);
    return -1;
  }

  // And store the result
  uint32_t value = result ? 1 : 0;
  if (!VmStackPush(vm, &value, 1)) {
    VmLogError(vm, "bc: ucmp: failed to push result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcCmpEq(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_CMPEQ);
}

//----------------------------------------------------------------------
int VmBcCmpNe(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_CMPNE);
}

//----------------------------------------------------------------------
int VmBcUCmpLt(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_UCMPLT);
}

//----------------------------------------------------------------------
int VmBcUCmpLe(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_UCMPLE);
}

//----------------------------------------------------------------------
int VmBcUCmpGe(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_UCMPGE);
}

//----------------------------------------------------------------------
int VmBcUCmpGt(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_UCMPGT);
}

//----------------------------------------------------------------------
int VmBcSCmpLt(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_SCMPLT);
}

//----------------------------------------------------------------------
int VmBcSCmpLe(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_SCMPLE);
}

//----------------------------------------------------------------------
int VmBcSCmpGe(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_SCMPGE);
}

//----------------------------------------------------------------------
int VmBcSCmpGt(VmContext *vm, uint32_t pc, uint32_t imm)
{
  return VmCmp(vm, VM_OP_SCMPGT);
}

//----------------------------------------------------------------------
int VmBcAdd(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: add: failed to fetch operands.");
    return -1;
  }

  uint32_t z = a + b;
  if (!VmStackPush(vm, &z, 1)) {
    VmLogError(vm, "bc: add: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcSub(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: sub: failed to fetch operands.");
    return -1;
  }

  uint32_t z = a - b;
  if (!VmStackPush(vm, &z, 1)) {
    VmLogError(vm, "bc: sub: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcMul(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: mul: failed to fetch operands.");
    return -1;
  }

  uint32_t z = a * b;
  if (!VmStackPush(vm, &z, 1)) {
    VmLogError(vm, "bc: mul: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcUDiv(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: udiv: failed to fetch operands.");
    return -1;
  }

  /// \todo Add to missing checks, to prevent this bytecode from crashing
  ///   the virtual machine for certain inputs. (let the function
  ///   gracefully fail with -1 if you encounter such problematic inputs)
  if(b==0){
	  VmLogError(vm, "bc: udiv: division by 0 detected.");
	  return -1;
  }

  uint32_t z = a / b;

  if (!VmStackPush(vm, &z, 1)) {
    VmLogError(vm, "bc: udiv: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcSDiv(VmContext *vm, uint32_t pc, uint32_t imm)
{
  int32_t a, b;
  if (!VmGetBinaryOperands((uint32_t *) &a, (uint32_t *) &b, vm)) {
    VmLogError(vm, "bc: sdiv: failed to fetch operands.");
    return -1;
  }

  /// \todo Add to missing checks, to prevent this bytecode from crashing
  ///   the virtual machine for certain inputs. (let the function
  ///   gracefully fail with -1 if you encounter such problematic inputs)
  ///
  /// Note: In comparison to VmBcUDiv there is at least one more input
  /// which causes problems
  if(b==0){
  	  VmLogError(vm, "bc: sdiv: division by 0 detected.");
  	  return -1;
  // explanation: why INT_MIN/-1 will overflow
  // let's assume an int is 8bit long -> signed int: -128 -> 127
  // -128/-1 -> 128: can't be stored in an 8bit integer
  }else if(a==INT32_MIN && b == -1){
  	  VmLogError(vm, "bc: sdiv: division overflow detected (INT32_MIN/-1.");
	  return -1;
  }

  int32_t z = a / b;

  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: sdiv: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcShl(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: shl: failed to fetch operands.");
    return -1;
  }

    // Unsigned (logic) shift
  uint32_t z = (b < 32) ? (a << b) : 0;
  if (!VmStackPush(vm, &z, 1)) {
    VmLogError(vm, "bc: shl: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcShr(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: shr: failed to fetch operands.");
    return -1;
  }

  // Unsigned (logic) shift
  uint32_t z = (b < 32) ? (a >> b) : 0;
  if (!VmStackPush(vm, &z, 1)) {
    VmLogError(vm, "bc: shr: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcAsr(VmContext *vm, uint32_t pc, uint32_t imm)
{
  int32_t a;
  uint32_t b;
  if (!VmGetBinaryOperands((uint32_t *) &a, &b, vm)) {
    VmLogError(vm, "bc: asr: failed to fetch operands.");
    return -1;
  }

  // Signed (arihemtic) shift (a is signed!)
  int32_t z = (b < 32) ? (a >> b) : (a < 0) ? -1 : 0;
  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: asr: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcRor(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: ror: failed to fetch operands.");
    return -1;
  }

  uint32_t z = a;

  b %= 32;
  if (b) {
    z = (z >> b) | (z << (32 - b));
  }

  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: ror: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcAnd(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: and: failed to fetch operands.");
    return -1;
  }

  uint32_t z = a & b;
  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: and: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcOr(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: and: failed to fetch operands.");
    return -1;
  }

  uint32_t z = a | b;
  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: and: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcXor(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a, b;
  if (!VmGetBinaryOperands(&a, &b, vm)) {
    VmLogError(vm, "bc: xor: failed to fetch operands.");
    return -1;
  }

  uint32_t z = a ^ b;
  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: xor: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcNot(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a;

  if (!VmStackPop(&a, vm, 1)) {
    VmLogError(vm, "bc: not: failed to pop the operand.");
    return -1;
  }

  uint32_t z = ~a;
  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: not: failed to push the result.");
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int VmBcNeg(VmContext *vm, uint32_t pc, uint32_t imm)
{
  uint32_t a;

  if (!VmStackPop(&a, vm, 1)) {
    VmLogError(vm, "bc: neg: failed to pop the operand.");
    return -1;
  }

  uint32_t z = -a;
  if (!VmStackPush(vm, (uint32_t *) &z, 1)) {
    VmLogError(vm, "bc: neg: failed to push the result.");
    return -1;
  }

  return 0;
}
