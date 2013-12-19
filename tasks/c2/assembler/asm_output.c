///
/// \file
/// \brief Bytecode output formatter
///
#include "assembler_imp.h"

//----------------------------------------------------------------------
static bool AsmWritePrologue(Assembler *assembler,
			     AsmObjectWriteCallback write_cb,
			     void *cb_data)
{
  assert (assembler != NULL);
  assert (write_cb != NULL);

  // Try to lookup the entrypoint
  uint32_t ep_handle = VM_NULL_HANDLE;
  AsmObject *entrypoint = AsmObjectsLookup(assembler->objects, ASM_MAIN_FUNCTION);

  if (entrypoint) {
    ep_handle = AsmObjectGetHandle(entrypoint);

    // Show the handle (we explicitly allow data objects as entrypoints,
    // to assist with VM testing)
    AsmInfo(assembler, NULL, "entrypoint '%s' is at 0x%08X", ASM_MAIN_FUNCTION,
	    (unsigned) ep_handle);

  } else {
    AsmInfo(assembler, NULL, "no '%s' function. setting entrypoint to null handle.",
	    ASM_MAIN_FUNCTION);
  }


  // And construct the prologue
  uint8_t prologue[] = {
    ASM_INIT_WORD(VM_TAG_UNIT, assembler->isBigEndian),      // Magic bytes (unit tag)
    ASM_INIT_HALF(VM_VERSION_MAJOR, assembler->isBigEndian), // Major VM version
    ASM_INIT_HALF(VM_VERSION_MINOR, assembler->isBigEndian), // Major VM version
    ASM_INIT_WORD(ep_handle, assembler->isBigEndian),        // Entrypoint handle
  };

  return write_cb(cb_data, prologue, sizeof(prologue));
}

//----------------------------------------------------------------------
static bool AsmWriteEpilogue(Assembler *assembler,
			     AsmObjectWriteCallback write_cb,
			     void *cb_data)
{
  assert (assembler != NULL);
  assert (write_cb != NULL);

  uint8_t epilogue[] = {
    ASM_INIT_WORD(VM_TAG_END, assembler->isBigEndian) // Magic bytes (end tag)
  };

  return write_cb(cb_data, epilogue, sizeof(epilogue));

  return true;
}

//----------------------------------------------------------------------
bool AsmWriteOutput(Assembler *assembler, AsmObjectWriteCallback write_cb,
		    void *cb_data)
{
  if (!assembler || !write_cb) {
    return false; // Bad arguments
  }

  // Write the prologue
  if (!AsmWritePrologue(assembler, write_cb, cb_data)) {
    AsmError(assembler, NULL, "failed to write prologue");
    return false;
  }

  // Write all objects in declaration order
  for (AsmObject *obj = assembler->objects; obj != NULL;
       obj = AsmObjectGetNext(obj)) {

    if (!AsmObjectWrite(obj, write_cb, cb_data)) {
      const AsmToken *token = AsmObjectGetToken(obj);
      AsmError(assembler, token, "failed to write object '%s'",
	       AsmTokenGetText(token));
      return false;
    }
  }

  // Write the epilogue
  if (!AsmWriteEpilogue(assembler, write_cb, cb_data)) {
    AsmError(assembler, NULL, "failed to write epilogue");
    return false;
  }

  // Done
  return true;
}
