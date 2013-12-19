///
/// \file
/// \brief Miscellaneous functions of the assembler API
///
#include "assembler_imp.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
Assembler* AsmNew(AsmVerbosityLevel verbosity, bool is_bigendian)
{
  Assembler *assembler = malloc(sizeof(Assembler));
  if (!assembler) {
    return NULL;
  }

  assembler->errors = 0;
  assembler->objects = NULL;
  assembler->verbosityLevel = verbosity;
  assembler->isBigEndian = is_bigendian;
  return assembler;
}

//----------------------------------------------------------------------
void AsmDelete(Assembler *assembler)
{
  if (assembler) {
    AsmObjectsDeleteChain(assembler->objects);
    memset(assembler, 0, sizeof(Assembler));
    free(assembler);
  }
}

//----------------------------------------------------------------------
static bool AsmCanAddObjects(Assembler *assembler, AsmObject *head)
{
  bool success = true;

  // Check each object for consistency. We report errors for all objects
  // found in the list starting at "head".
  for (AsmObject *current = head; current != NULL;
       current = AsmObjectGetNext(current)) {

    const AsmToken *hint = AsmObjectGetToken(current);
    const char *objname = AsmTokenGetText(hint);
    VmQualifiers qualifiers = AsmObjectGetQualifiers(current);

    // Object must qualified as code (VM_QUALIFIER_CODE) or data (VM_QUALIFIER_DATA)
    VmQualifiers objtype = qualifiers & (VM_QUALIFIER_CODE | VM_QUALIFIER_DATA);
    if (objtype != VM_QUALIFIER_CODE && objtype != VM_QUALIFIER_DATA) {
      AsmError(assembler, hint, "object '%s' must be qualified either as 'code' or as 'data'",
	       objname);
      success = false;
    }

    // Object must not have any reserved qualifiers
    VmQualifiers reserved = qualifiers & ~(VM_QUALIFIER_CODE | VM_QUALIFIER_DATA |
					    VM_QUALIFIER_INMUTABLE | VM_QUALIFIER_PROTECTED);
    if (reserved != 0) {
      AsmError(assembler, hint, "object '%s' has reserved qualifiers (reserved value 0x%08x)",
	       objname, (unsigned) reserved);
      success = false;
    }

    // Fail on duplicate object definitions
    AsmObject *duplicate = AsmObjectsLookup(assembler->objects, objname);
    if (duplicate) {
      const AsmToken *first_decl = AsmObjectGetToken(duplicate);

      AsmError(assembler, hint, "multiple declarations of object '%s' "
	       "(first declaration is in line %u, column %u)", objname,
	       AsmTokenGetLine(first_decl), AsmTokenGetColumn(first_decl));
      success = false;
    }
  }

  return  success;
}

//----------------------------------------------------------------------
bool AsmAddObjects(Assembler *assembler, AsmObject *head)
{
  if (!assembler || !head) {
    return false;
  }

  // Check preconditions for adding the objects (and report errors)
  if (!AsmCanAddObjects(assembler, head)) {
    return false;
  }

  // Join the objects
  assembler->objects = AsmObjectsJoin(assembler->objects, head);
  return true;
}
