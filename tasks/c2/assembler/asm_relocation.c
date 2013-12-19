///
/// \file
/// \brief Relocation records
///
#include "assembler_imp.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
struct AsmRelocation {
  /// Next relocation
  AsmRelocation *next;

  /// Type of relocation
  AsmRelocationType type;

  /// Offset into the bytecode buffer
  size_t offset;

  /// Name of the target symbol to resolve
  char symbol[];
};

//----------------------------------------------------------------------
AsmRelocation* AsmRelocationNew(AsmRelocationType type, size_t offset,
				const char *symbol)
{
  if (!symbol) {
    return NULL; // Bad arguments (NULL pointer)
  }

  if (type != ASM_RELOC_LOCAL && type != ASM_RELOC_JUMP &&
      type != ASM_RELOC_HANDLE && type != ASM_RELOC_LABEL) {
    return NULL; // Bad relocation type
  }

  // Determine the structure size
  size_t struct_size = sizeof(AsmRelocation) + 1;
  size_t symbol_name_len = strlen(symbol);
  if (SIZE_MAX - struct_size < symbol_name_len) {
    return NULL; // Integer overflow
  }

  struct_size += symbol_name_len;

  // Allocate
  AsmRelocation *reloc = malloc(struct_size);
  if (reloc == NULL) {
    return NULL; // Out of memory
  }

  reloc->next = NULL;
  reloc->type = type;
  reloc->offset = offset;

  if (symbol_name_len > 0) {
    strncpy(reloc->symbol, symbol, symbol_name_len);
  }

  reloc->symbol[symbol_name_len] = '\0';

  return reloc;
}

//----------------------------------------------------------------------
void AsmRelocationDelete(AsmRelocation *reloc)
{
  if (reloc) {
    memset(reloc, 0, sizeof(AsmRelocation));
    free(reloc);
  }
}

//----------------------------------------------------------------------
void AsmRelocationsDeleteChain(AsmRelocation *first)
{
  while (first != NULL) {
    AsmRelocation *next = first->next;
    AsmRelocationDelete(first);
    first = next;
  }
}

//----------------------------------------------------------------------
AsmRelocationType AsmRelocationGetType(const AsmRelocation *reloc)
{
  if (!reloc) {
    return ASM_RELOC_JUMP; // Some sane default
  }

  return reloc->type;
}

//----------------------------------------------------------------------
const char* AsmRelocationGetSymbol(const AsmRelocation *reloc)
{
  if (!reloc) {
    return NULL;
  }

  return reloc->symbol;
}

//----------------------------------------------------------------------
size_t AsmRelocationGetCodeOffset(const AsmRelocation *reloc)
{
  if (!reloc) {
    return 0;
  }

 return reloc->offset;
}

//----------------------------------------------------------------------
void AsmRelocationSetCodeOffset(AsmRelocation *reloc, size_t offset)
{
  if (reloc) {
    reloc->offset = offset;
  }
}

//----------------------------------------------------------------------
AsmRelocation* AsmRelocationGetNext(AsmRelocation *reloc)
{
  if (!reloc) {
    return NULL;
  }

  return reloc->next;
}

//----------------------------------------------------------------------
size_t AsmRelocationGetSize(const AsmRelocation *reloc)
{
  if (!reloc) {
    return 0;
  }

  switch (reloc->type) {
  case ASM_RELOC_LABEL:
    return 0; // Label (does not occupy space)

  case ASM_RELOC_LOCAL:
    return 1; // 8-bit operand

  case ASM_RELOC_JUMP:
    return 2; // 16-bit signed jump offset

  case ASM_RELOC_HANDLE:
    return 4; // 32-bit operand

  default:
    // Unknown relocation type (assume zero size)
    return 0;
  }
}

//----------------------------------------------------------------------
AsmRelocation* AsmRelocationsJoin(AsmRelocation *head, AsmRelocation *tail)
{
  if (!head) {
    // Trivial case: Join result is tail chain
    return tail;

  } else if (!tail) {
    // Trivial case: Join result is head chain
    return head;

  } else {
    // Find the end of the head chain
    AsmRelocation *current = head;
    AsmRelocation *last = head;

    while (current != NULL) {
      last = current;
      current = current->next;
    }

    assert (last != NULL);
    assert (last->next == NULL);

    // Append the tail
    last->next = tail;
    return head;
  }
}

//----------------------------------------------------------------------
AsmRelocation* AsmRelocationsFindLabel(AsmRelocation *head, const char* label)
{
  if (!label) {
    return NULL;
  }

  while (head != NULL) {
    if (head->type == ASM_RELOC_LABEL && strcmp(head->symbol, label) == 0) {
      return head;
    }

    head = head->next;
  }

  return NULL;
}
