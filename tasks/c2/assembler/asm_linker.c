///
/// \file
/// \brief Linker
///
#include "assembler_imp.h"

#ifndef NDEBUG
// Show handle allocation and relocations in release mode
# define ASM_LINK_SHOW_HANDLES
# define ASM_LINK_SHOW_RELOCS
#endif

//----------------------------------------------------------------------
AsmObject* AsmGetObjectByHandle(Assembler *assembler, uint32_t handle)
{
  for (AsmObject *obj = assembler->objects; obj != NULL;
       obj = AsmObjectGetNext(obj)) {

    if (AsmObjectGetHandle(obj) == handle) {
      return obj;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------
uint32_t AsmFindFreeHandle(Assembler *assembler,
			   uint32_t start, uint32_t limit)
{
  if (!assembler || limit < start) {
    return VM_NULL_HANDLE;
  }


  AsmObject *obj = NULL;

  // Probe first handle in range
  uint32_t handle = start;
  obj = AsmGetObjectByHandle(assembler, handle);

  // Scan until we find a free handle
  while (handle < limit && obj != NULL) {
    handle += 1;
    obj = AsmGetObjectByHandle(assembler, handle);
  }

  if (obj != NULL) {
    return VM_NULL_HANDLE; // No free handles found
  }

  return handle;
}

//----------------------------------------------------------------------
bool AsmAssignHandles(Assembler *assembler)
{
  uint32_t next_data = VM_DATA_HANDLE_FIRST;
  uint32_t next_code = VM_CODE_HANDLE_FIRST;

  bool success = true;

  for (AsmObject *obj = assembler->objects; obj != NULL;
       obj = AsmObjectGetNext(obj)) {

    const AsmToken *hint = AsmObjectGetToken(obj);
    const char *objname = AsmTokenGetText(hint);

    uint32_t handle = AsmObjectGetHandle(obj);

    // Skip objects which already have a handle ...
    if (handle != VM_NULL_HANDLE) {
#ifdef ASM_LINK_SHOW_HANDLES
      AsmInfo(assembler, hint, "object '%s': keeping user-supplied handle 0x%08X",
	      objname, (unsigned) handle);
#endif
      continue;
    }

    // ... find a free handle for other objects
    VmQualifiers type = AsmObjectGetQualifiers(obj)
      & (VM_QUALIFIER_CODE | VM_QUALIFIER_DATA);

    if (type == VM_QUALIFIER_CODE) {
      handle = AsmFindFreeHandle(assembler, next_code, VM_CODE_HANDLE_LAST);
      if (handle != VM_NULL_HANDLE && handle != VM_CODE_HANDLE_LAST) {
	next_code = handle + 1;
      }

    } else if (type == VM_QUALIFIER_DATA) {
      handle = AsmFindFreeHandle(assembler, next_data, VM_DATA_HANDLE_LAST);
      if (handle != VM_NULL_HANDLE && handle != VM_DATA_HANDLE_LAST) {
	next_data = handle + 1;
      }

    } else {
      AsmError(assembler, hint, "object '%s': unsupported object type (bad qualifiers)",
	       objname);
      handle = VM_NULL_HANDLE;
    }

    // Allocation failure? (out of handles?)
    if (handle == VM_NULL_HANDLE) {
      AsmError(assembler, hint, "object '%s': failed to allocate a static handle",
	       objname);
      success = false;
      continue;
    }

    // Allocation succeeded!
#ifdef ASM_LINK_SHOW_HANDLES
    AsmInfo(assembler, hint, "object '%s': allocating static handle 0x%08X",
	    objname, (unsigned) handle);
#endif
    AsmObjectSetHandle(obj, handle);
  }

  return success;
}

//----------------------------------------------------------------------
bool AsmLinkObject(Assembler *assembler, AsmObject *obj)
{
  if (!assembler || !obj) {
    return false;
  }

  const AsmToken *hint = AsmObjectGetToken(obj);
  const char *objname = AsmTokenGetText(hint);
  AsmCodeBuffer *code = AsmObjectGetContent(obj);
  bool success = true;

  for (AsmRelocation *reloc = AsmCodeGetRelocations(code);
       reloc != NULL; reloc = AsmRelocationGetNext(reloc)) {

    const char *symbol = AsmRelocationGetSymbol(reloc);
    AsmRelocationType type = AsmRelocationGetType(reloc);
    size_t offset = AsmRelocationGetCodeOffset(reloc);
    uint32_t value = 0;

    if (type == ASM_RELOC_HANDLE) {
      // 32-bit handle value
      AsmObject *dest = AsmObjectsLookup(assembler->objects, symbol);
      if (!dest) {
	AsmError(assembler, hint, "object '%s': unresolved static handle reference "
		 "to symbol '%s'", objname, symbol);
	success = false;
	continue;
      }

      value = AsmObjectGetHandle(dest);


    } else if (type == ASM_RELOC_JUMP) {
      // 24-bit jump offset
      AsmRelocation *dest = AsmCodeFindLabel(code, symbol);
      if (!dest) {
	AsmError(assembler, hint, "object '%s': unresolved label reference "
		 "to symbol '%s'", objname, symbol);
	success = false;
	continue;
      }

      // TODO: Do we want to check for overflows here?
      size_t dest_offset = AsmRelocationGetCodeOffset(dest);
      int32_t delta = (int32_t) dest_offset - (int32_t) offset;

      if (delta < VM_MIN_JUMP || delta > VM_MAX_JUMP) {
	AsmError(assembler, hint, "object '%s': label reference "
		 "to symbol '%s' not within a 16-bit boundary", objname, symbol);
	success = false;
	continue;
      }

      // Clip to 16-bits
      value = (uint32_t) (delta & 0x0000FFFFU);


    } else if (type == ASM_RELOC_LOCAL) {
      // 8-bit local variable index
      AsmVariable *dest = AsmCodeLookupVariable(code, symbol);
      if (!dest) {
	AsmError(assembler, hint, "object '%s': unresolved local variable reference "
		 "to symbol '%s'", objname, symbol);
	success = false;
	continue;
      }

      // Assign 8-bit local index
      value = AsmVariableGetIndex(dest);

    } else if (type == ASM_RELOC_LABEL) {
      // Nothing to do for relocations of type none and label
      continue;

    } else {
      AsmError(assembler, hint, "object '%s': relocation type %u is not supported "
	       "by the linker", objname, (unsigned) type);
      success = false;
      continue;
    }

    // Apply the relocation
    if (!AsmCodeApplyRelocation(code, reloc, value)) {
      AsmError(assembler, hint, "object '%s': failed to apply type %u relocation "
	       "at code offset %u with value 0x%X", objname, (unsigned) type,
	       (unsigned) offset, (unsigned) value);
      success = false;
      continue;
    }

#ifdef ASM_LINK_SHOW_RELOCS
      AsmInfo(assembler, hint, "object '%s': applied type %u relocation at offset %u "
	      " with value 0x%X", objname, (unsigned) type,
	      (unsigned) offset, (unsigned) value);
#endif
  }

  return success;
}

//----------------------------------------------------------------------
bool AsmLink(Assembler *assembler)
{
  // Pass 1: Assign handles to all objects handles
  bool success = AsmAssignHandles(assembler);

  // Pass 2: Process relocations (and resolve symbols)
  if (success) {
    for (AsmObject *obj = assembler->objects; obj != NULL;
	 obj = AsmObjectGetNext(obj)) {

      if (!AsmLinkObject(assembler, obj)) {
	success = false;
      }
    }
  }

  return success;
}
