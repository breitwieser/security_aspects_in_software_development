///
/// \file
/// \brief Main header file for the tinyasm assembler.
///
#ifndef TINYVM_ASSEMBLER_IMP_H
#define TINYVM_ASSEMBLER_IMP_H 1

// Public API
#include "assembler.h"

// Additional headers
#include <assert.h>

//----------------------------------------------------------------------
/// \brief Maximum value of a 4-bit unsigned integer.
#define ASM_UINT4_MAX  (15)

/// \brief Name of the main function in a bytecode program.
#define ASM_MAIN_FUNCTION "main"

//----------------------------------------------------------------------
// Helper macros for initializing byte arrays
// (in big endian or little endian byte order)
//

#define ASM_INIT_GETBYTE(_v,_pos)		\
  ((uint8_t) ((uint32_t) (_v) >> (_pos)))

#define ASM_INIT_HALF(_v,_isbig)					\
  ((_isbig) ? ASM_INIT_GETBYTE(_v, 8) : ASM_INIT_GETBYTE(_v, 0)),	\
  ((_isbig) ? ASM_INIT_GETBYTE(_v, 0) : ASM_INIT_GETBYTE(_v, 8))	\

#define ASM_INIT_WORD(_v,_isbig)					\
  ((_isbig) ? ASM_INIT_GETBYTE(_v, 24) : ASM_INIT_GETBYTE(_v, 0)),	\
  ((_isbig) ? ASM_INIT_GETBYTE(_v, 16) : ASM_INIT_GETBYTE(_v, 8)),	\
  ((_isbig) ? ASM_INIT_GETBYTE(_v, 8)  : ASM_INIT_GETBYTE(_v, 16)),	\
  ((_isbig) ? ASM_INIT_GETBYTE(_v, 0)  : ASM_INIT_GETBYTE(_v, 24))	\

//----------------------------------------------------------------------
// Simple buffers
//

/// Simple dynamic byte bufferfor use by the assembler.
///
/// This data structure wraps your byte buffer implementation for use
/// within the assembler. The wrapper structure simplifies error handling
/// (in particular within parser code) and adds an endianess marker needed
/// within the assembler.
///
typedef struct AsmBuffer {
  /// \brief Byte endianess of this buffer.
  ///
  /// This field selects the byte endianess (little vs. bug-endian) of this
  /// buffer. It is used by the \ref AsmBufferAppendWord and
  /// \ref AsmBufferAppendOffset functions.
  ///
  /// Users can query the byte endianess of the buffer using the
  /// \ref AsmBufferIsBigEndian function.
  ///
  /// \brief Endianess marker of this byte buffer object.
  bool isBigEndian;

  /// \brief Storage for the buffer contents.
  ///
  /// We use the buffer abstraction from the utility library (\c libutil.a) as
  /// storage backend.
  Buffer *storage;
} AsmBuffer;

/// Initializes a byte buffer for use.
///
/// \param[out] buf Byte buffer structure to be initialized.
/// \param is_bigendian Indicates if this buffer will hold big-endian data.
void AsmBufferInit(AsmBuffer *buf, bool is_bigendian);

/// Clears a byte buffer.
///
/// \param[out] buf Byte buffer structure to be cleared.
void AsmBufferClear(AsmBuffer *buf);

/// Appends arbitrary (binary) data to a byte buffer.
bool AsmBufferAppend(AsmBuffer *buf, const void *data, size_t length);

/// Appends repeated copies of an arbitrary byte to a byte buffer.
bool AsmBufferSkip(AsmBuffer *buf, unsigned char value, size_t length);

/// Appends a single byte to a byte buffer.
bool AsmBufferAppendByte(AsmBuffer *buf, unsigned char c);

/// Appends a 24-bit offset to a byte buffer.
bool AsmBufferAppendOffset(AsmBuffer *buf, uint32_t value);

/// Appends a 32-bit word a byte buffer.
bool AsmBufferAppendWord(AsmBuffer *buf, uint32_t value);

/// Returns a pointer to the contents of a byte buffer.
void* AsmBufferBytes(AsmBuffer *buf);

/// Returns the length (in bytes) of the contents of a byte buffer.
size_t AsmBufferLength(const AsmBuffer *buf);

/// Tests if a buffer uses big or little endian byte-ordering.
bool AsmBufferIsBigEndian(const AsmBuffer *buffer);

//----------------------------------------------------------------------
/// Deletes a token and clears the token pointer.
///
/// This function is a wrapper around \ref AsmTokenDelete, which first
/// deletes the token referenced by \c *ptoken and then clears the token
/// reference.
///
/// \note This helper function is used by the parser implementation
///  to discard tokens.
///
/// \param[in,out] ptoken Pointer to the token pointer.
/// \see AsmTokenDelete
static inline void AsmTokenDiscard(AsmToken **ptoken)
{
  assert(ptoken != NULL);
  AsmTokenDelete(*ptoken);
  *ptoken = NULL;
}

//----------------------------------------------------------------------
// Named virtual Machine call ID handling
//

/// \brief Static keyword information structure
typedef struct AsmVmCallInfo {
  /// \brief Name (text) of this virtual machine call
  const char *name;

  /// \brief Virtual machine call ID
  uint8_t callId;
} AsmVmCallInfo;


/// \brief Lookup function for well-known virtual machine calls.
const AsmVmCallInfo* AsmVmCallLookup(const char *call_name, size_t len);

//----------------------------------------------------------------------
// Relocation records
//

/// \brief An assembler relocation record.
///
/// Relocation records track information about movable parts of code
/// buffers (such as jump labels) and unresolved variables, as well as
/// object handles.
typedef struct AsmRelocation AsmRelocation;

/// \brief Type of a relocation record.
typedef enum AsmRelocationType {
  ASM_RELOC_LABEL,  ///< Label (marker only, does not occupy any space)
  ASM_RELOC_LOCAL,  ///< Unsigned 8-bit variable/argument index.
  ASM_RELOC_JUMP,   ///< Signed 16-bit offset (JMP, ...)
  ASM_RELOC_HANDLE, ///< Unsigned 32-bit object handle
} AsmRelocationType;

/// \brief Creates a new relocation record.
AsmRelocation* AsmRelocationNew(AsmRelocationType type, size_t offset,
				const char *symbol);

/// \brief Get the type of a reloaction record.
AsmRelocationType AsmRelocationGetType(const AsmRelocation *reloc);

/// \brief Gets the size (in bytes) of the target of a relocation record.
size_t AsmRelocationGetSize(const AsmRelocation *reloc);

/// \brief Gets the name of the (optional) symbol associated with a relocation record.
const char* AsmRelocationGetSymbol(const AsmRelocation *reloc);

/// \brief Gets the code buffer offset of the target of a relocation record.
size_t AsmRelocationGetCodeOffset(const AsmRelocation *reloc);

/// \brief Sets the code buffer offset of the target of a relocation record.
void AsmRelocationSetCodeOffset(AsmRelocation *reloc, size_t offset);

/// \brief Deletes a single relocation record.
void AsmRelocationDelete(AsmRelocation *reloc);

/// \brief Gets the next relocation record in a chain.
AsmRelocation* AsmRelocationGetNext(AsmRelocation *reloc);

/// \brief Finds a label in a relocation record chain.
AsmRelocation* AsmRelocationsFindLabel(AsmRelocation *head, const char* label);

/// \brief Deletes all relocation records in the chain starting at \c first.
void AsmRelocationsDeleteChain(AsmRelocation *first);

/// \brief Joins the relocation chains \c head and \c tail.
AsmRelocation* AsmRelocationsJoin(AsmRelocation *head, AsmRelocation *tail);

//----------------------------------------------------------------------
// Code buffers
//

/// \brief Code buffer with support for local variables and relocations.
typedef struct AsmCodeBuffer AsmCodeBuffer;

/// \brief Local variable or function argument.
typedef struct AsmVariable AsmVariable;

/// \brief Creates a new code buffer.
AsmCodeBuffer* AsmCodeNew(bool is_bigendian);

/// \brief Gets the raw content of a code buffer.
AsmBuffer* AsmCodeGetBytes(AsmCodeBuffer *code);

/// \brief Finds a local variable declared within the scope of a code buffer.
AsmVariable* AsmCodeLookupVariable(AsmCodeBuffer *code, const char *name);

/// \brief Gets The first local variable declared within the scope of a code buffer.
AsmVariable* AsmCodeGetVariables(AsmCodeBuffer *code);

/// \brief Tests if a code buffer uses big-endian byte order.
bool AsmCodeIsBigEndian(const AsmCodeBuffer *code);

/// \brief Resolves a loabel relocation record in context of a given code buffer.
AsmRelocation* AsmCodeFindLabel(AsmCodeBuffer *code, const char *name);

/// \brief Gets the first relocation record associated with a given code buffer.
AsmRelocation* AsmCodeGetRelocations(AsmCodeBuffer *code);

/// \brief Appends a new relocation record to a code buffer.
bool AsmCodeAddRelocation(AsmCodeBuffer *code, AsmRelocationType type, const char *symbol);

/// \brief Appends variable declarations to a code buffer.
bool AsmCodeAddVariables(AsmCodeBuffer *code, AsmVariable *vars);

/// \brief Prepends metadata (e.g. a function header) to a code buffer.
bool AsmCodeAddMetadata(AsmCodeBuffer *code, const void *data, size_t length);

/// \brief Deletes a code buffer.
void AsmCodeDelete(AsmCodeBuffer *code);

/// \brief Appends the \c tail code buffer to the \c head code buffer and merges variables
///   and relocations.
bool AsmCodeMerge(AsmCodeBuffer *head, AsmCodeBuffer *tail);

/// \brief Applies a relocation to a code buffer (the target of the relocation is
///   updated with the given value).
bool AsmCodeApplyRelocation(AsmCodeBuffer *code, const AsmRelocation *reloc,
			    uint32_t value);
//----------------------------------------------------------------------
// Variables
//

/// \brief Creates a new variable object.
AsmVariable* AsmVariableNew(AsmToken *name);

/// \brief Deletes a single variable object.
void AsmVariableDelete(AsmVariable* variable);

/// \brief Gets the token associated with the declaration of a variable.
const AsmToken* AsmVariableGetToken(const AsmVariable *variable);

/// \brief Gets the name of a variable.
const char *AsmVariableGetName(const AsmVariable *variable);

/// \brief Gets the next variable in a chain.
AsmVariable* AsmVariableGetNext(AsmVariable *variable);

/// \brief Gets the local (stack) index of a variable.
uint8_t AsmVariableGetIndex(const AsmVariable *variable);

/// \brief Sets the local (stack) index of a variable.
void AsmVariableSetIndex(AsmVariable* variable, uint8_t index);

/// \brief Deletes all variables in the chain starting at \c variable.
void AsmVariablesDeleteChain(AsmVariable *variable);

/// \brief splits the head variables from the chain starting at \c head.
AsmVariable* AsmVariablesSplit(AsmVariable *head);

/// \brief Joins the \c head and \c tail variable chains.
AsmVariable* AsmVariablesJoin(AsmVariable *head, AsmVariable *tail);

/// \brief Searches a variable in the chain starting at \c head.
AsmVariable* AsmVariablesLookup(AsmVariable *head, const char *name);

//----------------------------------------------------------------------
// Assembler objects
//

/// \brief Assembler view of a virtual machine object.
typedef struct AsmObject AsmObject;

/// \brief Write callback for dumping virtual machine objects.
typedef bool (*AsmObjectWriteCallback)(void *cb_data, const void *bytes, size_t length);

/// \brief Creates a new assembler object.
AsmObject* AsmObjectNew(AsmToken *token, VmQualifiers qualifiers,
			AsmCodeBuffer *content);

/// \brief Deletes an assembler object.
void AsmObjectDelete(AsmObject *object);

/// \brief Gets the content code buferr of an assembler object.
AsmCodeBuffer* AsmObjectGetContent(const AsmObject *object);

/// \brief Gets the object handle of an assembler object.
uint32_t AsmObjectGetHandle(const AsmObject *object);

/// \brief Sets the object handle of an assembler object.
void AsmObjectSetHandle(AsmObject *object, uint32_t handle);

/// \brief Gets the qualifiers of an asembler object.
VmQualifiers AsmObjectGetQualifiers(const AsmObject *object);

/// \brief Gets the token associated with the declaration of an assembler object.
const AsmToken* AsmObjectGetToken(const AsmObject *object);

/// \brief Gets the next assembler object in a chain.
AsmObject* AsmObjectGetNext(const AsmObject *object);

/// \brief Writes an assembler object to an output medium.
bool AsmObjectWrite(const AsmObject *object, AsmObjectWriteCallback write_cb,
		    void *cb_data);

/// \brief Deletes all assembler objects in the chain starting at \c first.
void AsmObjectsDeleteChain(AsmObject *first);

/// \brief Joins the \¢ head and \c tail object chains.
AsmObject* AsmObjectsJoin(AsmObject *head, AsmObject *tail);

/// \brief Finds a named object in the chain starting at \¢ head.
AsmObject* AsmObjectsLookup(AsmObject *head, const char *name);


//----------------------------------------------------------------------
// Assembler details
//

struct Assembler {
  /// \brief Number of errors reported by \ref AsmError.
  unsigned errors;

  /// Byte endianess used by the assembler
  bool isBigEndian;

  /// \brief Output verbosity level of the assembler.
  ///
  /// This field controls the amount of output produced by the assembler
  /// during normal operation.
  AsmVerbosityLevel verbosityLevel;


  /// \brief All objects known (defined) by this assembler
  AsmObject *objects;
};

// Public assembler API
bool AsmAddObjects(Assembler *assembler, AsmObject *head);
bool AsmLink(Assembler* assembler);
bool AsmAssignHandles(Assembler *assembler);
bool AsmWriteOutput(Assembler *assembler, AsmObjectWriteCallback write_cb,
		    void *cb_data);
uint32_t AsmFindFreeHandle(Assembler *assembler,
			   uint32_t start, uint32_t limit);
AsmObject* AsmGetObjectByHandle(Assembler *assembler, uint32_t handle);

//----------------------------------------------------------------------
// Parser helpers
//

/// \brief Forward reference information.
///
/// This structure is used by the parser to describe forward references to
/// unresolved objects and labels.
typedef struct AsmForwardRef {
  /// \brief Token (optional) with the symbol name.
  AsmToken *token;

  /// \brief Absolute value of the reference.
  int64_t value;

  /// \brief Unsigned 32-bit offset relative to the symbol
  uint32_t offset;
} AsmForwardRef;

/// \brief Initializes a forward reference.
void AsmForwardRefInit(AsmForwardRef *ref, AsmToken *token, int64_t value, uint32_t offset);

/// \brief Clear a forward reference.
void AsmForwardRefClear(AsmForwardRef *ref);

/// \brief Creates a new code buffer (parser helper).
AsmCodeBuffer* AsmUtilNewCodeBuffer(Assembler *assembler, AsmToken *hint);

/// \brief Appends bytes to a code buffer (parser helper).
AsmCodeBuffer* AsmUtilAppendCodeBytes(Assembler *assembler, AsmToken *hint,
				      AsmCodeBuffer *code,
				      const void *bytes, size_t len);

/// \brief Appends a single byte to a code buffer (parser helper).
AsmCodeBuffer* AsmUtilAppendCodeByte(Assembler *assembler, AsmToken *hint,
				     AsmCodeBuffer *code,
				     uint8_t byte);

/// \brief Appends a single 32-bit word to a code buffer (parser helper).
AsmCodeBuffer* AsmUtilAppendCodeWord(Assembler *assembler, AsmToken *hint,
				     AsmCodeBuffer *code, uint32_t word);

/// \brief Skips (reserves) bytes in a code buffer (parser helper).
AsmCodeBuffer* AsmUtilSkipCodeBytes(Assembler *assembler, AsmToken *hint,
				    AsmCodeBuffer *code,
				    unsigned char value, size_t len);

/// \brief Appends a relocation record to a code buffer (parser helper).
AsmCodeBuffer* AsmUtilAppendRelocation(Assembler *assembler, AsmToken *hint,
				       AsmCodeBuffer *code,
				       AsmRelocationType type, AsmForwardRef *ref);


AsmCodeBuffer* AsmUtilAppendImm8(Assembler *assembler, AsmToken *hint,
				 AsmCodeBuffer *code, bool is_signed,
				 int32_t value);

AsmCodeBuffer* AsmUtilAppendImm16(Assembler *assembler, AsmToken *hint,
				  AsmCodeBuffer *code, bool is_signed,
				  int32_t value);

AsmCodeBuffer* AsmUtilAppendImm32(Assembler *assembler, AsmToken *hint,
				 AsmCodeBuffer *code, int64_t value);


AsmCodeBuffer* AsmUtilMergeCode(Assembler *assembler, AsmToken *hint,
				AsmCodeBuffer *head,
				AsmCodeBuffer *tail);

AsmCodeBuffer* AsmUtilDeclareVariables(Assembler *assembler,
				       AsmCodeBuffer *code,
				       AsmVariable *vars);


int64_t AsmUtilParseInteger(Assembler *assembler, AsmToken *token,
			    int64_t min_value, int64_t max_value);

bool AsmUtilDefineFunction(Assembler *assembler, AsmToken *name,
			   size_t num_results, AsmVariable *argument,
			   AsmCodeBuffer *code, uint32_t handle);

bool AsmUtilDefineObject(Assembler *assembler, AsmToken *name,
			 VmQualifiers qualifiers, AsmCodeBuffer *code,
			 uint32_t handle);


#endif // TINYVM_ASSEMBLER_IMP_H

