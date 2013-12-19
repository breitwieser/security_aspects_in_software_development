///
/// \file
/// \brief Byte-code handling for the tinyvm virtual machine
///
#include "tinyvm_imp.h"
#include "bytecodes.h"

//----------------------------------------------------------------------
/// \brief Opcode decoding table.
///
/// This table contains the decode information record for the
/// bytecode instruction. The table is indexed by the opcode value.
/// Undefined instruction have an invalid (zero-initialized) decode
/// record.
static const VmDecodeInfo OPCODES[256] = {
  // Constants and handles
  [VM_OP_IMM32]  = { "IMM32",  VM_FORMAT_WORD,   VmBcImm32  },
  [VM_OP_HANDLE] = { "HANDLE", VM_FORMAT_WORD,   VmBcHandle },

  // Calls and branches
  [VM_OP_CALL]   = { "CALL",   VM_FORMAT_WORD,   VmBcCall   },
  [VM_OP_JMP]    = { "JMP",    VM_FORMAT_HALF,   VmBcJmp    },
  [VM_OP_BT]     = { "BT",     VM_FORMAT_HALF,   VmBcBt     },
  [VM_OP_BF]     = { "BF",     VM_FORMAT_HALF,   VmBcBf     },
  [VM_OP_RET]    = { "RET",    VM_FORMAT_SIMPLE, VmBcRet    },

  // Stack manipulation
  [VM_OP_POP]    = { "POP",    VM_FORMAT_BYTE,   VmBcPop    },
  [VM_OP_PEEK]   = { "PEEK",   VM_FORMAT_BYTE,   VmBcPeek   },
  [VM_OP_SWAP]   = { "SWAP",   VM_FORMAT_SIMPLE, VmBcSwap   },

  // Compares
  [VM_OP_CMPEQ]  = { "CMPEQ",  VM_FORMAT_SIMPLE, VmBcCmpEq  },
  [VM_OP_CMPNE]  = { "CMPNE",  VM_FORMAT_SIMPLE, VmBcCmpNe  },
  [VM_OP_UCMPLT] = { "UCMPLT", VM_FORMAT_SIMPLE, VmBcUCmpLt },
  [VM_OP_UCMPLE] = { "UCMPLE", VM_FORMAT_SIMPLE, VmBcUCmpLe },
  [VM_OP_UCMPGE] = { "UCMPGE", VM_FORMAT_SIMPLE, VmBcUCmpGe },
  [VM_OP_UCMPGT] = { "UCMPGT", VM_FORMAT_SIMPLE, VmBcUCmpGt },
  [VM_OP_SCMPLT] = { "SCMPLT", VM_FORMAT_SIMPLE, VmBcSCmpLt },
  [VM_OP_SCMPLE] = { "SCMPLE", VM_FORMAT_SIMPLE, VmBcSCmpLe },
  [VM_OP_SCMPGE] = { "SCMPGE", VM_FORMAT_SIMPLE, VmBcSCmpGe },
  [VM_OP_SCMPGT] = { "SCMPGT", VM_FORMAT_SIMPLE, VmBcUCmpGt },

  // Loads and stores
  [VM_OP_LDVAR]  = { "LDVAR", VM_FORMAT_BYTE,    VmBcLdVar },
  [VM_OP_STVAR]  = { "STVAR", VM_FORMAT_BYTE,    VmBcStVar },
  [VM_OP_LDW]    = { "LDW",   VM_FORMAT_SIMPLE,  VmBcLdW   },
  [VM_OP_STW]    = { "STW",   VM_FORMAT_SIMPLE,  VmBcStW   },
  [VM_OP_LDB]    = { "LDB",   VM_FORMAT_SIMPLE,  VmBcLdB   },
  [VM_OP_STB]    = { "STB",   VM_FORMAT_SIMPLE,  VmBcStB   },

  // Arithmetic and logic
  [VM_OP_ADD]    = { "ADD",   VM_FORMAT_SIMPLE,  VmBcAdd   },
  [VM_OP_SUB]    = { "SUB",   VM_FORMAT_SIMPLE,  VmBcSub   },
  [VM_OP_MUL]    = { "MUL",   VM_FORMAT_SIMPLE,  VmBcMul   },
  [VM_OP_UDIV]   = { "UDIV",  VM_FORMAT_SIMPLE,  VmBcUDiv  },
  [VM_OP_SDIV]   = { "SDIV",  VM_FORMAT_SIMPLE,  VmBcSDiv  },
  [VM_OP_SHL]    = { "SHL",   VM_FORMAT_SIMPLE,  VmBcShl   },
  [VM_OP_SHR]    = { "SHR",   VM_FORMAT_SIMPLE,  VmBcShr   },
  [VM_OP_ASR]    = { "ASR",   VM_FORMAT_SIMPLE,  VmBcAsr   },
  [VM_OP_ROR]    = { "ROR",   VM_FORMAT_SIMPLE,  VmBcRor   },
  [VM_OP_AND]    = { "AND",   VM_FORMAT_SIMPLE,  VmBcAnd   },
  [VM_OP_OR]     = { "OR",    VM_FORMAT_SIMPLE,  VmBcOr    },
  [VM_OP_XOR]    = { "XOR",   VM_FORMAT_SIMPLE,  VmBcXor   },
  [VM_OP_NOT]    = { "NOT",   VM_FORMAT_SIMPLE,  VmBcNot   },
  [VM_OP_NEG]    = { "NEG",   VM_FORMAT_SIMPLE,  VmBcNeg   },

  // Virtual Machine Calls
  [VM_OP_VMC]    = { "VMC",   VM_FORMAT_BYTE,    VmBcVmc   },
};

//----------------------------------------------------------------------
int VmStep(VmContext *vm)
{
  if (!vm) {
    return -1;
  }

  // Anything to execute?
  if (!vm->function) {
    VmLogError(vm, "step: no function to execute");
    return -1;
  }

  // Open the current function
  MemView view;
  if (!VmAccessObject(&view, vm->function)) {
    VmLogError(vm, "step: active function is not accessible");
    return -1;
  }

  // Adjust the actual fetch address to skip the virtual program counter
  if ((SIZE_MAX - VM_FUNCTION_BYTECODE_OFFSET - 1) < vm->pc) {
    VmLogError(vm, "step: impossible virtual program counter value (integer overflow)");
    return -1;
  }

  size_t fetch_offset = VM_FUNCTION_BYTECODE_OFFSET + vm->pc;

  // Fetch the opcode byte
  uint8_t opcode;
  if (!MemGetByte(&opcode, &view, fetch_offset + 0)) {
    VmLogError(vm, "step: pc:0x%08x: failed to fetch the opcode byte.",
               (unsigned) fetch_offset);
    return -1;
  }

  // Lookup the decode information, check for name and handler
  assert (0 <= opcode && opcode < sizeof(OPCODES)/sizeof(OPCODES[0]));
  const VmDecodeInfo *const info = &OPCODES[opcode];

  if (!info->name || !info->handler) {
    VmLogError(vm, "step: pc:0x%08x: unimplemented opcode 0x%02x [%-8s]",
               (unsigned) vm->pc, (unsigned) opcode,
               info->name ? info->name : "undefined");
    return -1;
  }

  // Format specific handling
  uint32_t imm;
  size_t len;

  if (info->format == VM_FORMAT_SIMPLE) {
    // Simple opcode without immediate
    VmLogTrace(vm, "step: pc:0x%08x: [%-8s]",
               (unsigned) vm->pc, info->name);
    imm = 0;
    len = 1;

  } else if (info->format == VM_FORMAT_BYTE) {
    // Opcode with single byte immediate
    uint8_t tmp;
    if (!MemGetByte(&tmp, &view, fetch_offset + 1)) {
          VmLogError(vm, "step: pc:0x%08x: [%-8s] failed to load 8-bit immediate",
                     (unsigned) vm->pc, info->name);
          return -1;
    }

    imm = tmp;
    len = 2;

    VmLogTrace(vm, "step: pc:0x%08x: [%-8s] imm8:0x%02x",
               (unsigned) vm->pc, info->name, (unsigned) imm);

  } else if (info->format == VM_FORMAT_HALF) {
    // Opcode with 16-bit half-word immediate
    uint16_t tmp;
    if (!MemGetHalf(&tmp, &view, fetch_offset + 1)) {
          VmLogError(vm, "step: pc:0x%08x: [%-8s] failed to load 16-bit immediate",
                     (unsigned) vm->pc, info->name);
          return -1;
    }

    imm = tmp;
    len = 3;

    VmLogTrace(vm, "step: pc:0x%08x: [%-8s] imm16:0x%04x",
               (unsigned) vm->pc, info->name, (unsigned) imm);

  } else if (info->format == VM_FORMAT_WORD) {
    // Opcode with 32-bit immediate
    if (!MemGetWord(&imm, &view, fetch_offset + 1)) {
          VmLogError(vm, "step: pc:0x%08x: [%-8s] failed to load 32-bit immediate",
                     (unsigned) vm->pc, info->name);
          return -1;
    }

    len = 5;

    VmLogTrace(vm, "step: pc:0x%08x: [%-8s] imm32:0x%08x",
               (unsigned) vm->pc, info->name, (unsigned) imm);
  }

  // Update the program counter for the next opcode (branching
  // opcodes can override the program counter again if desired)
  uint32_t old_pc = vm->pc;
  vm->pc += len;

  // Tail call to the handler
  assert (info->handler != NULL);
  return (info->handler)(vm, old_pc, imm);
}
