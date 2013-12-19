///
/// \file
/// \brief Definition of function objects.
///
#include "assembler_imp.h"

#include <string.h>

//----------------------------------------------------------------------
static bool AsmUtilProcessLocals(size_t *num_locals, size_t *num_args,
				 Assembler *assembler, AsmToken *name,
				 AsmVariable *arguments, AsmCodeBuffer *code)
{
  assert (num_locals != NULL);
  assert (num_args != NULL);

  bool success = true;

  *num_locals = 0;
  *num_args = 0;

  // Assign local variable indices
  for (AsmVariable *local = AsmCodeGetVariables(code); local != NULL;
       local = AsmVariableGetNext(local)) {

    // Normal variables need one slot
    if (VM_MAX_LOCALS - *num_locals < 1) {
      AsmError(assembler, name, "not enough variable slots to allocate "
	       "local variable '%s'", AsmVariableGetName(local));
      success = false;
      break;
    }

    // And assign
    AsmVariableSetIndex(local, *num_locals);
    *num_locals += 1;
  }

  // Assign argument indices
  if (success) {
    for (AsmVariable *arg = arguments; arg != NULL;
	 arg = AsmVariableGetNext(arg)) {
      if (VM_MAX_ARGS - *num_args < 1) {
	AsmError(assembler, name, "not enough argument slots to "
		 " allocate argument '%s'", AsmVariableGetName(arg));
	success = false;
	break;
      }

      // And assign
      AsmVariableSetIndex(arg, VM_MAX_LOCALS + *num_args);
      *num_args += 1;
    }
  }

  // Merge the local arguments
  if (success && *num_args > 0) {
    success = AsmCodeAddVariables(code, arguments);
    if (success) {
      // Ownership transfer succeeded
      arguments = NULL;

    } else {
      // Error (need to dispose arguments later)
      AsmError(assembler, name, "internal error: failed to append "
	       " arguments to function '%s'", AsmTokenGetText(name));
    }
  }

  // Done
  if (!success) {
    AsmVariablesDeleteChain(arguments);
  }

  return success;
}

//----------------------------------------------------------------------
bool AsmUtilDefineFunction(Assembler *assembler, AsmToken *name,
			   size_t num_results, AsmVariable *arguments,
			   AsmCodeBuffer *code, uint32_t handle)
{
  bool success = true;

  if (!assembler || !name || !code) {
    success = false; // Bad arguments (likely an earlier parser error)
  }

  // Check number of results
  if (num_results > VM_MAX_RESULTS) {
    AsmError(assembler, name, "number of results of function '%s' "
	     "exceeds maximum limit (%u).",
	     AsmTokenGetText(name), (unsigned) VM_MAX_RESULTS);
    success = false;
  }

  // Assign variable and argument symbols
  size_t num_locals = 0;
  size_t num_args = 0;
  if (success) {
    success = AsmUtilProcessLocals(&num_locals, &num_args, assembler, name,
				   arguments, code);

    // Ownership of "arguments" has been transferred to "code"
    // (or "arguments" has been disposed)
    arguments = NULL;
  }

  // Prepend the function meta-data
  if (success) {
    // Meta-data header
    uint8_t metadata[] = {
      (uint8_t) 0,                    // (reserved for flags; should be zero)
      (uint8_t) num_results,          // Number of results
      (uint8_t) num_args,             // Number of arguments
      (uint8_t) num_locals,           // Number of locals
    };

    success = AsmCodeAddMetadata(code, metadata, sizeof(metadata));
    if (!success) {
      AsmError(assembler, name, "failed to add metadata to function '%s'",
	       AsmTokenGetText(name));
    }
  }

  if (!success) {
    // We had errors, need to cleanup remainings
    AsmVariablesDeleteChain(arguments);
    AsmCodeDelete(code);
    AsmTokenDelete(name);

  } else {
    // Success, delegate actual function object definition to define object.
    success = AsmUtilDefineObject(assembler, name,
				  VM_QUALIFIER_CODE | VM_QUALIFIER_INMUTABLE,
				  code, handle);
  }

  return success;
}
