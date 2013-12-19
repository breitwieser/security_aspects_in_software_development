///
/// \file
/// \brief Code buffers
///
#include "assembler_imp.h"
#include "util/buffer.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
struct AsmCodeBuffer
{
  // Local variables
  AsmVariable *locals;

  // Head of relocation chain
  AsmRelocation *relocations;

  // Bytecodes stored in this code buffer.
  AsmBuffer bytes;
};

//----------------------------------------------------------------------
AsmCodeBuffer* AsmCodeNew(bool is_bigendian)
{
  AsmCodeBuffer *code = malloc(sizeof(AsmCodeBuffer));
  if (!code) {
    return NULL;
  }

  AsmBufferInit(&code->bytes, is_bigendian);
  code->relocations = NULL;
  code->locals = NULL;

  return code;
}

//----------------------------------------------------------------------
bool AsmCodeAddRelocation(AsmCodeBuffer *code, AsmRelocationType type,
			  const char *symbol)
{
  if (!code) {
    return false; // Bad parameters
  }

  // Find the current offset
  size_t offset = AsmBufferLength(&code->bytes);
  AsmRelocation *reloc = AsmRelocationNew(type, offset, symbol);
  if (!reloc) {
    return false; // Out of memory (or bad relocation data)
  }

  // Reserve the code bytes
  size_t size = AsmRelocationGetSize(reloc);
  if (!AsmBufferSkip(&code->bytes, 0, size)) {
    AsmRelocationDelete(reloc);
    return false; // Out of memory during skip
  }

  assert (AsmBufferLength(&code->bytes) == offset + size);

  // Now join the relocations array
  code->relocations = AsmRelocationsJoin(code->relocations, reloc);

  // Success
  return true;
}

//----------------------------------------------------------------------
void AsmCodeDelete(AsmCodeBuffer *code)
{
  if (code) {
    AsmBufferClear(&code->bytes);
    AsmVariablesDeleteChain(code->locals);
    AsmRelocationsDeleteChain(code->relocations);

    memset(code, 0, sizeof(AsmCodeBuffer));
    free(code);
  }
}

//----------------------------------------------------------------------
AsmBuffer* AsmCodeGetBytes(AsmCodeBuffer *code)
{
  if (!code) {
    return NULL;
  }

  return &code->bytes;
}

//----------------------------------------------------------------------
AsmVariable* AsmCodeLookupVariable(AsmCodeBuffer *code, const char *name)
{
  if (!code) {
    return NULL;
  }

  return AsmVariablesLookup(code->locals, name);
}

//----------------------------------------------------------------------
AsmVariable* AsmCodeGetVariables(AsmCodeBuffer *code)
{
  if (!code) {
    return NULL;
  }

  return code->locals;
}

//----------------------------------------------------------------------
bool AsmCodeIsBigEndian(const AsmCodeBuffer *code)
{
  if (!code) {
    return NULL;
  }

  return AsmBufferIsBigEndian(&code->bytes);
}

//----------------------------------------------------------------------
AsmRelocation* AsmCodeFindLabel(AsmCodeBuffer *code, const char *name)
{
  if (!code || !name) {
    return NULL;
  }

  return AsmRelocationsFindLabel(code->relocations, name);
}

//----------------------------------------------------------------------
AsmRelocation* AsmCodeGetRelocations(AsmCodeBuffer *code)
{
  if (!code) {
    return NULL;
  }

  return code->relocations;
}

//----------------------------------------------------------------------
bool AsmCodeAddVariables(AsmCodeBuffer *code, AsmVariable *vars)
{
  if (!code || !vars) {
    return false; // Bad parameters
  }

  // TODO: Check vars for duplicates?
  code->locals = AsmVariablesJoin(code->locals, vars);
  return true;
}

//----------------------------------------------------------------------
static bool AsmCodeCanAdjustRelocations(AsmRelocation *first,
					size_t delta)
{
  size_t offset_limit = SIZE_MAX - delta;
  for (AsmRelocation *reloc = first; reloc != NULL;
       reloc = AsmRelocationGetNext(reloc)) {

    size_t reloc_offset = AsmRelocationGetCodeOffset(reloc);
    if (offset_limit < reloc_offset) {
      return false; // Integer overflow in relocation offset
    }
  }

  // Now overflows in relocation offset
  return true;
}

//----------------------------------------------------------------------
static void AsmCodeAdjustRelocations(AsmRelocation *first,
				     size_t delta)
{
  for (AsmRelocation *reloc = first; reloc != NULL;
       reloc = AsmRelocationGetNext(reloc)) {

    size_t offset = AsmRelocationGetCodeOffset(reloc);
    AsmRelocationSetCodeOffset(reloc, offset + delta);
  }
}

//----------------------------------------------------------------------
bool AsmCodeAddMetadata(AsmCodeBuffer *code, const void *data, size_t length)
{
  if (!code || (!data && length > 0)) {
    return false;
  }

  if (length == 0) {
    return true; // Trivial case: Empty metadata
  }

  if (!AsmCodeCanAdjustRelocations(code->relocations, length)) {
    return false; // Relocations would overflow
  }

  size_t old_length = AsmBufferLength(&code->bytes);

  // Grow the code buffer
  if (!AsmBufferSkip(&code->bytes, 0, length)) {
    return false;
  }

  // Prepend the meta-data
  uint8_t *raw = AsmBufferBytes(&code->bytes);
  memmove(raw + length, raw, old_length); // Move bytecodes
  memcpy(raw, data, length);              // Prepend meta-data

  // Adjust the relocations
  AsmCodeAdjustRelocations(code->relocations, length);
  return true;
}

//----------------------------------------------------------------------
static bool AsmCodeCanMerge(AsmCodeBuffer *head, AsmCodeBuffer *tail)
{
  // Can't merge with NULL code buffers
  if (!head || !tail) {
    return false;
  }

  // Check for unmergable variables
  for (AsmVariable *var = tail->locals; var != NULL;
       var = AsmVariableGetNext(var)) {

    const char *varname = AsmVariableGetName(var);
    if (AsmVariablesLookup(head->locals, varname)) {
      return false; // Duplicate variables exist
    }
  }

  // Check for unmergable relocations
  size_t offset = AsmBufferLength(&head->bytes);
  if (!AsmCodeCanAdjustRelocations(tail->relocations, offset)) {
    return false;
  }

  // Check for duplicate labels
  for (AsmRelocation *reloc = tail->relocations; reloc != NULL;
       reloc = AsmRelocationGetNext(reloc)) {

    const char *symbol = AsmRelocationGetSymbol(reloc);

    if (AsmRelocationGetType(reloc) == ASM_RELOC_LABEL &&
	AsmRelocationsFindLabel(head->relocations, symbol)) {
      // Duplicate label
      return false;
    }
  }

  // Preconditions for the merge are satisfied
  return true;
}

//----------------------------------------------------------------------
bool AsmCodeMerge(AsmCodeBuffer *head, AsmCodeBuffer *tail)
{
  if (!AsmCodeCanMerge(head, tail)) {
    return false; // Preconditions are not satisfied
  }

  // Store offset adjustment for merged relocations
  size_t reloc_delta = AsmBufferLength(&head->bytes);

  // Append the code bytes from the tail buffer
  const uint8_t *tail_bytes = AsmBufferBytes(&tail->bytes);
  size_t tail_size = AsmBufferLength(&tail->bytes);
  if (!AsmBufferAppend(&head->bytes, tail_bytes, tail_size)) {
    return false; // Append failed
  }

  AsmBufferClear(&tail->bytes);

  // Join the variables of the tail buffer
  head->locals = AsmVariablesJoin(head->locals, tail->locals);
  tail->locals = NULL;

  // Adjust the relocations in the tail buffer
  AsmCodeAdjustRelocations(tail->relocations, reloc_delta);

  // Join the relocations of the tail buffer
  head->relocations = AsmRelocationsJoin(head->relocations, tail->relocations);
  tail->relocations = NULL;

  // Done
  return true;
}

//----------------------------------------------------------------------
bool AsmCodeApplyRelocation(AsmCodeBuffer *code, const AsmRelocation *reloc,
			    uint32_t value)
{
  if (!code || !reloc) {
    return false;
  }

  // Encode the operand
  uint8_t encoded[] = {
    ASM_INIT_WORD(value, AsmBufferIsBigEndian(&code->bytes))
  };


  size_t size = AsmRelocationGetSize(reloc);
  if (size == 0 || size > sizeof(encoded)) {
    return false; // Invalid relocation size (can not apply eg. labels)
  }

  size_t code_length = AsmBufferLength(&code->bytes);
  size_t offset = AsmRelocationGetCodeOffset(reloc);
  if (offset >= code_length || (code_length - offset) < size) {
    return false; // Invalid offset/size combination
  }

  uint8_t *data = AsmBufferBytes(&code->bytes);
  assert (data != NULL); // Implied by offset test above (code_length=0)

  if (AsmBufferIsBigEndian(&code->bytes)) {
    // Big-endian bytecode format
    // Copy the encoded operand
    memcpy(data + offset, encoded + (sizeof(encoded) - size), size);

  } else {
    // Little-endian bytecode format (experimental)
    // Copy the encoded operand
    memcpy(data + offset, encoded, size);
  }

  return true;
}
