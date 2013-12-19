///
/// \file
/// \brief Forward references
///
#include "assembler_imp.h"


//----------------------------------------------------------------------
void AsmForwardRefInit(AsmForwardRef *ref, AsmToken *token, int64_t value,
		       uint32_t offset)
{
  assert (ref != NULL);
  assert (token == NULL || value == 0);

  ref->token = token;
  ref->value = value;
  ref->offset = offset;
}

//----------------------------------------------------------------------
void AsmForwardRefClear(AsmForwardRef *ref)
{
  assert (ref != NULL);
  AsmTokenDiscard(&ref->token);
}

//----------------------------------------------------------------------
AsmCodeBuffer* AsmUtilAppendRelocation(Assembler *assembler, AsmToken *hint,
				       AsmCodeBuffer *code,
				       AsmRelocationType type, AsmForwardRef *ref)
{
  if (!ref || !code) {
    return code; // Bad parameters
  }

  if (ref->token != NULL) {
    const char *symbol_name = AsmTokenGetText(ref->token);

    // Check for duplciate labels
    if (type == ASM_RELOC_LABEL) {
      AsmRelocation *duplicate = AsmCodeFindLabel(code, symbol_name);
      if (duplicate) {
	// Emit an error (but create the duplicate label below)
	AsmError(assembler, hint, "multiple definitions of label '%s'",
		 symbol_name);
      }
    }

    // Emit a relocation record for the symbol
    if (!AsmCodeAddRelocation(code, type, symbol_name)) {
      AsmError(assembler, hint,
	       "failed to create a type %u relocation record for symbol '%s'",
	       (unsigned) type, symbol_name);

    }

  } else {
    // Emit an absolute symbol value
    AsmBuffer* bytes = AsmCodeGetBytes(code);

    switch (type) {
    case ASM_RELOC_LOCAL:
      if (!AsmBufferAppendByte(bytes, ref->value & 0x000000FFUL)) {
	AsmError(assembler, hint, "failed to emit 8-bit local index");
      }
      break;

    case ASM_RELOC_JUMP:
      if (!AsmBufferAppendOffset(bytes, ref->value & 0x0000FFFFUL)) {
	AsmError(assembler, hint, "failed to emit 16-bit jump offset value");
      }
      break;

    case ASM_RELOC_HANDLE:
      if (!AsmBufferAppendWord(bytes, ref->value & 0xFFFFFFFFUL)) {
	AsmError(assembler, hint, "failed to emit 32-bit handle value");
      }
      break;

    case ASM_RELOC_LABEL:
    default:
      AsmError(assembler, hint, "unsupported relocation type %u", (unsigned) type);
      break;
    }
  }

  return code;
}
