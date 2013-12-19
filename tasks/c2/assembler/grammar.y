//
// Lemon grammar of the tinyvm assembler language.
//
// See http://www.sqlite.org/src/doc/trunk/doc/lemon.html for
// more details on the lemon parser generator.
//
%include
{
#include "assembler_imp.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

}

%token_type       { AsmToken* }
%token_destructor { AsmTokenDiscard(&$$); }

%extra_argument   { Assembler* pAsm }
%name AsmLemonParse

%start_symbol unit
%token_prefix ASM_TOKEN_

// Non-terminal types
%type imm32             { int64_t  }
%type any8              { int16_t  }
%type arguments         { AsmVariable* }
%type code              { AsmCodeBuffer* }
%type handle            { AsmForwardRef }
%type i16               { int16_t }
%type i8                { int8_t  }
%type idx8              { AsmForwardRef }
%type init_bytes        { AsmCodeBuffer* }
%type init_words        { AsmCodeBuffer* }
%type object_init       { AsmCodeBuffer* }
%type object_qualifiers { VmQualifiers }
%type off16             { AsmForwardRef }
%type opt_object_handle { uint32_t }
%type opt_result_count  { uint32_t }
%type simm16            { int16_t }
%type simm8             { int8_t }
%type uimm8             { uint8_t }
%type u16               { uint16_t }
%type u32               { uint32_t }
%type u4                { uint8_t  }
%type u8                { uint8_t  }
%type variable          { AsmVariable* }
%type variables         { AsmVariable* }
%type vmcallhint_opt    { uint32_t }
%type vmcallid          { uint32_t }

// Non-terminal destructors
%destructor arguments    { (void) pAsm; AsmVariablesDeleteChain($$); }
%destructor handle       { (void) pAsm; AsmForwardRefClear(&$$); }
%destructor code         { (void) pAsm; AsmCodeDelete($$); }
%destructor idx8         { (void) pAsm; AsmForwardRefClear(&$$); }
%destructor init_bytes   { (void) pAsm; AsmCodeDelete($$); }
%destructor init_words   { (void) pAsm; AsmCodeDelete($$); }
%destructor object_init  { (void) pAsm; AsmCodeDelete($$); }
%destructor off16        { (void) pAsm; AsmForwardRefClear(&$$); }
%destructor variable     { (void) pAsm; AsmVariableDelete($$); }
%destructor variables    { (void) pAsm;  AsmVariablesDeleteChain($$); }

//----------------------------------------------------------------------
// Callbacks
//

%parse_failure
{
  // Parser detected an irrecoverable error.
  AsmError(pAsm, NULL, "Irrecoverable parser error. Giving up!");
}

%stack_overflow
{
  // Parser detected a parser stack overflow.
  AsmError(pAsm, NULL, "Irrecoverable parser stack overflow. Giving up!");
}

%syntax_error
{
#ifndef NDEBUG
  // Syntax error.
  //
  // Try to produce a sensible error message. Lemon provides the
  // major token code in the "yymajor" argument. The minor token
  // value (our AsmToken*) is availabel in "TOKEN".
  //
  const char *token_name = yyTokenName[yymajor];

  AsmError(pAsm, TOKEN, "Syntax error near unexpected token '%s' (%u).",
	   token_name, (unsigned) yymajor);

  //
  // Show all possible tokens with a shift action
  // (Idea based on http://stackoverflow.com/questions/11705737/expected-token-using-lemon-parser-generator)
  //
  size_t num_tokens = sizeof(yyTokenName) / sizeof(yyTokenName[0]);

  AsmInfo(pAsm, NULL, "Possible tokens are:");

  for (size_t i = 0; i < num_tokens; ++i) {
    YYACTIONTYPE a = yy_find_shift_action(yypParser, (YYCODETYPE) i);
    if (i == 0) {
      AsmInfo(pAsm, NULL, " end of file");
    } else if (a < YYNSTATE + YYNRULE) {
      AsmInfo(pAsm, NULL, " %s (%u)", yyTokenName[i], (unsigned) i);
    }
  }
#else
  // Don't have yyTokenNames in release mode ...
  AsmError(pAsm, TOKEN, "Syntax error!");
#endif
}

//----------------------------------------------------------------------
// A translation unit is the top-level grammar production in
// a tinyvm source file. It consists of zero or more declarations
// of objects and functions.
//
unit ::= declarations .

// Declaration of a function
declarations ::= declarations function .

// Declaration of a statically allocated objects
declarations ::= declarations object .

// Syntax error recovery (at declaration level)
declarations ::= declarations error .
{
  AsmInfo(pAsm, NULL, "Recovered from syntax syntax error in top-level declaration, "
	  "trying to continue.");
}

// Empty declaration
declarations ::= .

//----------------------------------------------------------------------
// Functions accept a fixed number of input parameters and can return
// zero or more result values. Internally the VM describes functions as
// objects with "code" attribute and a special header.
//
// Syntax example (handle will be automatically assigned by the assembler):
//  .function(1) foo(@a, @b)    // Function "foo" with 1 results and 2 arguments
//    .local @c                 // Declare local variable C
//
//    LDVAR(@a)                 // Load argument A
//    LDVAR(@b)                 // Load argument B
//    ADD                       // Add top of stack operands
//    STVAR(@c)                 // Store result in local variable C
//
//    LDVAR(@c)                 // Reload local variable C
//    RET(1)                    // Return (without 1 results)
//  .end
//
// Syntax example (handle is manually given as 0xC0DE0001):
//  .function[0xC0DE0001] (1) bar(@a, @b) // Function "bar" with handle 0xC0DE0001,
//                                        // 1 result and 2 arguments
//   ...
//  .end
//
// More syntax exampleS:
//  .function MyFunction() ... .end     // Function with 0 results (procedure)
//  .function (0) MyFunction() ... .end // Function with 0 results (alternate syntax)
//  .function (1) MyFunction() ... .end // Function with 1 result
//
function ::=
   DOT_FUNCTION opt_object_handle(Handle) opt_result_count(NumResults)
   IDENTIFIER(FunctionName) arguments(Arguments)
      code(Code)
   DOT_END  .
{
  // NOTE: "name", "arguments" and "code" are always disposed by AsmUtilDefineFunction.
  // (even in case of errors)
  AsmUtilDefineFunction(pAsm, FunctionName, NumResults, Arguments, Code, Handle);
}

/// Optional number of result
opt_result_count(Ret) ::= LPAREN u8(Value) RPAREN .
{
  Ret = Value;
}

opt_result_count(Ret) ::= .
{
  // No results
  Ret = 0;
}

/// Empty code buffer
code(Ret) ::= .
{
  Ret = AsmUtilNewCodeBuffer(pAsm, NULL);
}

/// Error recovery
code(Ret) ::= code(Code) error .
{
  AsmInfo(pAsm, NULL, "Recovered from syntax error in function body, "
	  "trying to continue.");

  Ret = Code;
}

// Declaration of local variables
//
// Local variable declarations can appear anywhere in the body
// of a subprogram.
//
code(Ret) ::= code(Code) DOT_LOCAL variables(Vars) .
{
  Ret = AsmUtilDeclareVariables(pAsm, Code, Vars);
}

// Function with one or more arguments
arguments(Ret) ::= LPAREN variables(Vars) RPAREN .
{
  Ret = Vars;
}

// Function with zero arguments
arguments(Ret) ::= LPAREN RPAREN . // Empty argument list
{
  Ret = NULL;
}

// Non-empty list of variable (or argument) declarations.
variables(Ret) ::= variables(Vars) COMMA variable(Next) .
{
  Ret = AsmVariablesJoin(Vars, Next);
}

variables(Ret) ::= variable(First) .
{
  Ret = First;
}

// Declaration of variable
//
// Variables (and arguments) must be declared exactly once before
// they can be used. It is an error to declare a local variable or
// argument which has the same name as another local variable
// or argument in the same subprogram.
//
variable(Ret) ::= LOCAL_NAME(Name) .
{
  // Create a new value variable.
  Ret = AsmVariableNew(Name);
  if (!Ret) {
    AsmError(pAsm, Name, "failed to create a new variable");
    AsmTokenDiscard(&Name);
  }
}

// Labels (for use with BT and BF instructions)
code(Ret) ::= code(Code) IDENTIFIER(LabelName) DOUBLECOLON .
{
  AsmForwardRef ref;
  AsmForwardRefInit(&ref, LabelName, 0, 0);
  Ret = AsmUtilAppendRelocation(pAsm, LabelName, Code, ASM_RELOC_LABEL, &ref);
  AsmForwardRefClear(&ref);
}

//----------------------------------------------------------------------
// Data (and code) objects
//
// Syntax Example (data objects):
//  .object data OtherData ... .end     // Standard data object
//
//  .object data MyData
//   .byte (0x02, 3, 15, -3) // 4 initialized bytes
//   .word (1, 2, 3, 4)      // 4 initialized wordss
//   .skip (120)             // Skip 120 bytes (fill with zero bytes)
//   .handle (MyData)        // Handle of object "MyData"
//   .handle (0x1234)        // Handle of object #0x1234
//   .skip (4, 0xFF)         // Skip 4 bytes (fill with 0xFF bytes)
//  .end
//
//  .object[0x3CF2] data MyData .. .end // Object with handle 0x00003CF2
//
// Syntax example (code objects):
//   // Functions are with special headers. See AsmUtilDefineFunction for
//   // the header format.
//   .object code rawfoo
//      // Function header
//     .byte (0) // Reserved (should be zero)
//     .byte (0) // Number of results (zero)
//     .byte (0) // Number of arguments (zero)
//     .byte (0) // Number of local variables (zero)
//
//     // Actual bytecode
//     .byte (0x17) // RET
//   .end

// Declaration of a statically initialized object
object ::=
   DOT_OBJECT opt_object_handle(Handle) object_qualifiers(Qualifiers) IDENTIFIER(Name)
     object_init(Image)
  DOT_END .
{
  // NOTE: "name" and "image" are always disposed by AsmUtilDefineObject
  // (even on errors)
  AsmUtilDefineObject(pAsm, Name, Qualifiers, Image, Handle);
}

// Optional object handle specification
opt_object_handle(Ret) ::= LBRACK u32(Value) RBRACK .
{
  Ret = Value;
}

// Assign default handle
opt_object_handle(Ret) ::= .
{
  Ret = VM_NULL_HANDLE;
}

//
// Object qualifiers
//


// INMUTABLE: Object is inmutable. (read-only)
//
// Inmutable object can not be written to by VM "store"
// opcodes (STW, STB).
//
object_qualifiers(Ret) ::= object_qualifiers(Qualifiers) INMUTABLE(Hint) .
{
  if (Qualifiers & VM_QUALIFIER_INMUTABLE) {
    AsmError(pAsm, Hint, "'inmutable' qualifier has been given more than once.");
  }

  Ret = Qualifiers | VM_QUALIFIER_INMUTABLE;
  AsmTokenDiscard(&Hint);
}

// PROTECTED: Object is protected. (not readable from bytecode)
//
// Protected objects can not be read by VM "load"
// opcodes (LDW, LDB).
//
object_qualifiers(Ret) ::= object_qualifiers(Qualifiers) PROTECTED(Hint) .
{
  if (Qualifiers & VM_QUALIFIER_PROTECTED) {
    AsmError(pAsm, Hint, "'protected' qualifier has been given more than once.");
  }

  Ret = Qualifiers | VM_QUALIFIER_PROTECTED;
  AsmTokenDiscard(&Hint);
}


// DATA: Object contains raw data.
//
// This qualifier must be the first qualifier of an object. It is
// mutually exclusive with the CODE qualifier.
object_qualifiers(Ret) ::= DATA .
{
  Ret = VM_QUALIFIER_DATA;
}

// CODE: Object is contains code (it is a function)
//
// This qualifier must be the first qualifier of an object. It is
// mutually exclusive with the DATA qualifier. Function objects have a
// fixed "header" structure describing the number of arguments, local
// variables and return values.
//
// The actual byte-code of the function immediately follows after
// the function header.
//
// The code qualifier allows virtual machine developers and testers,
// to construct arbitrary (possibly invalid) virtual machine functions, which
// can not be described with regular ".function" assembler directives.
object_qualifiers(Ret) ::= CODE .
{
  Ret = VM_QUALIFIER_CODE;
}

//
// Object initializers
//

/// Error recovery
object_init(Ret) ::= object_init(Image) error .
{
  AsmInfo(pAsm, NULL, "Recovered from syntax error in object initializer, "
	  "trying to continue.");
  Ret = Image;
}

// Initialized byte array (all bytes filled with user-defined values)
//
// Syntax example:
//  .byte (1)
//  .byte (1, -2, 0x3)
object_init(Ret) ::= object_init(Image) DOT_BYTE(Hint) LPAREN init_bytes(Bytes) RPAREN .
{
  Ret = AsmUtilMergeCode(pAsm, Hint, Image, Bytes);
  AsmTokenDiscard(&Hint);
}

init_bytes(Ret) ::= init_bytes(Bytes) COMMA(Hint) any8(Byte) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Bytes, Byte);
  AsmTokenDiscard(&Hint);
}

init_bytes(Ret) ::= any8(Byte) .
{
  // Start a new code buffer
  AsmToken *Hint = NULL; // TODO: Source hint?
  Ret = AsmUtilNewCodeBuffer(pAsm, Hint);
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Ret, Byte);
  AsmTokenDiscard(&Hint);
}

// Initialized byte array (all bytes filled with same value)
//
// Syntax example:
//  .skip (3)        // Skip 3 zero initialized bytes
//  .skip (3, 1)     // Skip 3 one initialized bytes
object_init(Ret) ::= object_init(Image) DOT_SKIP(Hint) LPAREN u32(Count) RPAREN .
{
  Ret = AsmUtilSkipCodeBytes(pAsm, Hint, Image, 0, Count);
  AsmTokenDiscard(&Hint);
}

object_init(Ret) ::= object_init(Image) DOT_SKIP(Hint) LPAREN u32(Count) COMMA any8(FillByte) RPAREN .
{
  Ret = AsmUtilSkipCodeBytes(pAsm, Hint, Image, FillByte, Count);
  AsmTokenDiscard(&Hint);
}


// Initialized 32-bit word array (all words filled with user-defined values)
//
// Syntax example:
//  .word (1)
//  .word (1, -2, 0x3)
object_init(Ret) ::= object_init(Image) DOT_WORD(Hint) LPAREN init_words(Words) RPAREN .
{
  Ret = AsmUtilMergeCode(pAsm, Hint, Image, Words);
  AsmTokenDiscard(&Hint);
}

init_words(Ret) ::= init_words(Words) COMMA(Hint) imm32(Word) .
{
  Ret = AsmUtilAppendCodeWord(pAsm, Hint, Words, (uint32_t) (Word & 0xFFFFFFFFU));
  AsmTokenDiscard(&Hint);
}

init_words(Ret) ::= imm32(Word) .
{
  AsmToken *Hint = NULL; // TODO: Source hint
  Ret = AsmUtilNewCodeBuffer(pAsm, Hint);
  Ret = AsmUtilAppendCodeWord(pAsm, Hint, Ret, (uint32_t) (Word & 0xFFFFFFFFU));
  AsmTokenDiscard(&Hint);
}

// Initialized handle
object_init(Ret) ::= object_init(Image) DOT_HANDLE(Hint) handle(Handle) .
{
  // Append the handle and the offset
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Image, ASM_RELOC_HANDLE, &Handle);
  AsmForwardRefClear(&Handle);
  AsmTokenDiscard(&Hint);
}

// Empty initializer (start of initializers list)
object_init(Ret) ::= .
{
  // Start with the empty code buffer
  Ret = AsmUtilNewCodeBuffer(pAsm, NULL);
}

//----------------------------------------------------------------------
// Opcodes: Constants
//

// IMM: Load a 32-bit immediate (value must be in 32-bit range)
//
// Stack: ... => ... imm
//
//
code(Ret) ::= code(Code) IMM(Hint) LPAREN imm32(Value) RPAREN .
{
  Ret = AsmUtilAppendImm32(pAsm, Hint, Code, Value);
  AsmTokenDiscard(&Hint);
}

// HANDLE: Load an object handle.
//
// Stack: ... => ... handle
//
// The HANDLE instruction loads the handle of an object (data or function) onto
// the stack.
//
// Syntax example:
//  HANDLE (foobar)           // Load handle of the "foobar" object
//  HANDLE (NULL)             // Load the NULL handle.
//  HANDLE (#27)              // Load the handle to object #27 (debug variant)
//  HANDLE (#0)               // Load the NULL handle (debug variant)

code(Ret) ::= code(Code) HANDLE(Hint) handle(Handle) .
{
  // Emit a HANDLE opcode
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_HANDLE);
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Ret, ASM_RELOC_HANDLE, &Handle);
  AsmForwardRefClear(&Handle);
  AsmTokenDiscard(&Hint);
}

//----------------------------------------------------------------------
// Call and branch instructions
//

// CALL: Call subroutine
//
// Stack: ... [arg0 [... argN]]  => ... [ret0 [... retN]]
//
// This pseudo-instructions opcode calls a named subroutine. The calling
// convention of the tinyvm virtual machine is based on Java's method
// conventions (or the "pascal" calling convetion known from C):
//
// * Input parameters are passed left-to-right.
//   (At the time of call the top of stack is the rightmost input
//    parameter)
//
// * Stack cleanup is performed by the callee as part of executing the
//   RET instruction.
//
// * Return value(s) are passed on the stack. The RET instruction
//   preserves the order of return values.
//
// Syntax example:
//   CALL (func1)         // Call function "func1"
//   CALL (#27)           // Call function #27 (debug variant)
//
code(Ret) ::= code(Code) CALL(Hint) handle(FunctionRef) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_CALL);
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Ret, ASM_RELOC_HANDLE, &FunctionRef);
  AsmForwardRefClear(&FunctionRef);
  AsmTokenDiscard(&Hint);
}

// JMP: Unconditional Jump (within current function)
//
// VM runtime errors:
//   VM_E_BAD_BRANCH  - Invalid branch target (branch outside current function)
//
// Stack: (no change)
code(Ret) ::= code(Code) JMP(Hint) off16(Label) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_JMP);
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Ret, ASM_RELOC_JUMP, &Label);
  AsmForwardRefClear(&Label);
  AsmTokenDiscard(&Hint);
}

// BT: Branch If True
//
// Stack: ... a/ptr => ...
//
// Branches if the top of stack value (or pointer) is non-zero (non-null).
//
// VM runtime errors:
//   VM_E_BAD_BRANCH  - Invalid branch target (branch outside current function)
//
// Syntax example:
//   BT (label1)
//
code(Ret) ::= code(Code) BT(Hint) off16(Label) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_BT);
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Ret, ASM_RELOC_JUMP, &Label);
  AsmForwardRefClear(&Label);
  AsmTokenDiscard(&Hint);
}

// BF: Branch If False
//
// Stack: ... a/ptr => ...
//
// Branches if the top of stack value (or pointer) is zero (null).
//
// VM runtime errors:
//   VM_E_BAD_BRANCH  - Invalid branch target (branch outside current function)
//
// Syntax example:
//   BF (label1)
//
code(Ret) ::= code(Code) BF(Hint) off16(Label) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_BF);
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Ret, ASM_RELOC_JUMP, &Label);
  AsmForwardRefClear(&Label);
  AsmTokenDiscard(&Hint);
}

// PC-relative offset to a label given by name.
//
// This syntax is the standard way to refer to labels in
// JMP, BEQ, BNE, BT and BF instructions.
//
off16(Ret) ::= LPAREN IDENTIFIER(LabelName) RPAREN .
{
  AsmForwardRefInit(&Ret, LabelName, 0, 0);
}

// PC-relative offset to a label given by raw value. (debug variant)
//
// Using this syntax produces an assembler warning, unless advanced (debug)
// instructions are enabled in the assembler. (see "-a" command line option)
//
// This syntax allows VM developers and testers to produce (arbitrary)
// signed 24-bit branch offsets for testing the virtual machine.
//
off16(Ret) ::= LPAREN i16(Value) RPAREN .
{
  AsmForwardRefInit(&Ret, NULL, Value, 0);
}

// RET: Return from subroutine (with N results)
//
// Stack: (caller frame ...) (current frame ... [ret0 [... retN]]) => (caller frame ... [ret0 [... retN]])
//
// Copies the results of the current function to the caller's stack frame,
// disposes the current stackframe and returns to the caller of the current
// subroutine.
//
// This instruction always copies exactly the number of results defined
// as part of the function header from the current stack frame to the caller's stack frame.
//
// Syntax example:
//   RET       - Return
code(Ret) ::= code(Code) RET(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_RET);
  AsmTokenDiscard(&Hint);
}

//----------------------------------------------------------------------
// Stack operations
//

// POP(N): Discards the n top of stack values
//
// Stack (N=1): ... a => ...
//
// Syntax example:
//   POP(1)
code(Ret) ::= code(Code) POP(Hint) LPAREN u8(N) RPAREN.
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_POP);
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Ret, N);
  AsmTokenDiscard(&Hint);
}

// PEEK(N): Duplicates the N-th top of stack value
//
// Stack (N=1): ... a => ... a a
//
// Syntax example:
//   PEEK(1)   // Duplicate the top of stack value
code(Ret) ::= code(Code) PEEK(Hint) LPAREN u8(N) RPAREN .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_PEEK);
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Ret, N);
  AsmTokenDiscard(&Hint);
}

// SWAP: Swaps the two top of stack value
//
//
// Stack: ... a b => ... b a
//
// Syntax example:
//   SWAP
code(Ret) ::= code(Code) SWAP(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SWAP);
  AsmTokenDiscard(&Hint);
}

//----------------------------------------------------------------------
// Compare instructions
//

// CMPEQ: Compare equal.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as 32-bit integers
// and compares if a is equal to b . Pushes one (1) onto the stack
// if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   CMPEQ
code(Ret) ::= code(Code) CMPEQ(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_CMPEQ);
  AsmTokenDiscard(&Hint);
}

// CMPNE: Compare equal.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as 32-bit integers
// and compares if a is not equal to b . Pushes one (1) onto the stack
// if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   CMPNE
code(Ret) ::= code(Code) CMPNE(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_CMPNE);
  AsmTokenDiscard(&Hint);
}


// UCMPLT: Unsigned compare less-than.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as unsigned 32-bit integers
// and compares if a is strictly less than b. Pushes one (1) onto the stack
// if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   UCMPLT
code(Ret) ::= code(Code) UCMPLT(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_UCMPLT);
  AsmTokenDiscard(&Hint);
}

// UCMPLE: Unsigned compare less-or-equal.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as unsigned 32-bit integers
// and compares if a is less or equal than b. Pushes one (1) onto the stack
// if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   UCMPLE
code(Ret) ::= code(Code) UCMPLE(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_UCMPLE);
  AsmTokenDiscard(&Hint);
}

// UCMPGE: Unsigned compare greater-or-equal.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as unsigned 32-bit integers
// and compares if a is greater or equal than b. Pushes one (1) onto the stack
// if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   UCMPGE
code(Ret) ::= code(Code) UCMPGE(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_UCMPGE);
  AsmTokenDiscard(&Hint);
}

// UCMPGT: Unsigned compare greater-than.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as unsigned 32-bit integers
// and compares if a is strictly greater than b. Pushes one (1) onto the stack
// if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   UCMPGT
code(Ret) ::= code(Code) UCMPGT(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_UCMPGT);
  AsmTokenDiscard(&Hint);
}

// SCMPLT: Signed compare less-than.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as signed (two's complement)
// 32-bit integers and compares if a is strictly less than b. Pushes one (1) onto
// the stack if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   SCMPLT
code(Ret) ::= code(Code) SCMPLT(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SCMPLT);
  AsmTokenDiscard(&Hint);
}

// SCMPLE: Signed compare less-or-equal.
//
// Interprets the two top of stack values as signed (two's complement)
// 32-bit integers and compares if a is less or equal than b. Pushes one (1) onto
// the stack if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   SCMPLE
code(Ret) ::= code(Code) SCMPLE(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SCMPLE);
  AsmTokenDiscard(&Hint);
}

// SCMPGE: Signed compare greater-or-equal.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as signed (two's complement)
// 32-bit integers and compares if a is greater or equal than b. Pushes one (1) onto
// the stack if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   SCMPGE
code(Ret) ::= code(Code) SCMPGE(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SCMPGE);
  AsmTokenDiscard(&Hint);
}

// SCMPGT: Signed compare greater-than.
//
// Stack: ... a b => ... 0/1
//
// Interprets the two top of stack values as signed (two's complement)
// 32-bit integers and compares if a is strictly greater or than b. Pushes one (1) onto
// the stack if the test evaluates to true or zero (0) otherwise.
//
// Syntax example:
//   SCMPGT
code(Ret) ::= code(Code) SCMPGT(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SCMPGT);
  AsmTokenDiscard(&Hint);
}

//----------------------------------------------------------------------
// Load/store instructions
//

// LDVAR(Index): Load from local variable or argument.
//
// Stack: ... => z
//
// Loads a value or pointer from a the local variables or arguments
// of the current function.
//
// Syntax example:
//   LDVAR (@arg1)   // Load from @arg1
//   LDVAR (#3)      // Load from local variable slot #3 (debug variant)
//   LDVAR (#128)    // Load from argument slot #0 (debug variant)
code(Ret) ::= code(Code) LDVAR(Hint) idx8(Index) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_LDVAR);
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Ret, ASM_RELOC_LOCAL, &Index);
  AsmForwardRefClear(&Index);
  AsmTokenDiscard(&Hint);
}

// STVAR(Index): Store to local variable or argument.
//
// Stack: ... a => ...
//
// Store a value or pointer in a local variable or input argument slot
// of the current function.
//
// Syntax example:
//   STVAR (@arg1)   // Store to @arg1
//   STVAR (#3)      // Store to local variable slot #3 (debug variant)
//   STVAR (#128)    // Store to argument slot #0 (debug variant)
code(Ret) ::= code(Code) STVAR(Hint) idx8(Index) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_STVAR);
  Ret = AsmUtilAppendRelocation(pAsm, Hint, Ret, ASM_RELOC_LOCAL, &Index);
  AsmForwardRefClear(&Index);
  AsmTokenDiscard(&Hint);
}

// Index of a named local variable or argument.
//
// This syntax variant is the default for variable and
// argument references in LDVAR and STVAR instructions.
idx8(Ret) ::= LPAREN LOCAL_NAME(VarName) RPAREN .
{
  AsmForwardRefInit(&Ret, VarName, 0, 0);
}

// Index of a named local variable or argument slot. (debug variant)
//
// Internally the virtual machine uses unsigned 8-bit indices to address
// local variables and arguments:
//
// * Values in range 0 to 127 refer to local variable slots 0-127
// * Values between 128 and 255 refer to argument slots 0-127
//
// Using this syntax produces an assembler warning, unless advanced (debug)
// instructions are enabled in the assembler. (see "-a" command line option)
//
// This syntax allows VM developers and testers to produce (arbitrary)
// 8-bit local variable/argument indicesfor testing the virtual machine.
idx8(Ret) ::= LPAREN u8(Index) RPAREN .
{
  AsmForwardRefInit(&Ret, NULL, Index, 0);
}

// LDW: Load 32-bit word from memory.
//
// Stack: ... offset handle => ... z
//
// Loads a single 32-bit value from the virtual machine memory location referenced
// by "offset" and "handle" and pushes the loaded value to the operand stack. "handle"
// denotes the object handle of the source object, "offset" is a byte offset into the
// source object's data region.
//
// Syntax example:
//   LDW
code(Ret) ::= code(Code) LDW(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_LDW);
  AsmTokenDiscard(&Hint);
}

// STW: Store 32-bit word to memory.
//
// Stack: ... a offset handle => ...
//
// Stores a single 32-bit value to the virtual machine memory location referenced
// by "offset" and "handle". "handle" denotes the object handle of the destination
// object, "offset" is a byte offset into the destination object's data region.
//
// Syntax example:
//   STW
code(Ret) ::= code(Code) STW(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_STW);
  AsmTokenDiscard(&Hint);
}

// LDB: Load byte from memory.
//
// Stack: ... offset handle => ... z
//
// Loads a single byte (unsigned 8-bit value) from the virtual machine memory location
// referenced by "offset" and "handle" and pushes the loaded value to the operand stack.
// "handle" denotes the object handle of the source object, "offset" is a byte offset
// into the source object's data region.
//
// Syntax example:
//   LDB
code(Ret) ::= code(Code) LDB(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_LDB);
  AsmTokenDiscard(&Hint);
}

// STB: Store byte from memory.
//
// Stack: ... a offset handle => ...
//
// Stores a single byte (unsigned 8-bit value) from the virtual machine memory location
// referenced by "ptr" and pushes the loaded value to the operand stack. The STB instruction
// does not have any alignment requirments for the virtual pointer offset.
//
// Syntax example:
//   STB
code(Ret) ::= code(Code) STB(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_STB);
  AsmTokenDiscard(&Hint);
}

//----------------------------------------------------------------------
// Arithmetic and logic instructions
//
// The virtual machine uses binary representation for unsigned
// integers and two's complement representation for signed integers. As
// a consequence the unsigned ADD, SUB and RSB instructions also correctly
// work with signed (two's complement) values. The MUL instruction can be
// used to multiply two signed or two unsigned operands.
//

// ADD: Add two operands. (a + b)
//
// Stack: ... a b => ... z
//
// Adds a and b as unsigned values and pushes the result back onto the
// stack. Unsigned overflows are silently ignored and the result
// is truncated to an unsigned 32 bit value.
//
// Syntax example:
//   ADD
code(Ret) ::= code(Code) ADD(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_ADD);
  AsmTokenDiscard(&Hint);
}

// SUB: Subtract two operands. (a - b)
//
// Stack: ... a b => ... z
//
// Subtracts a and b as unsigned values and pushes the result back onto the
// stack. Unsigned overflows are silently ignored and the result is
// truncated to an unsigned 32 bit value.
//
// Syntax example:
//   SUB
code(Ret) ::= code(Code) SUB(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SUB);
  AsmTokenDiscard(&Hint);
}

// MUL: Multiply two operands (a * b) and return the lower 32 bits of the result.
//
// Stack: ... a b => ... z
//
// Multiplies the two values a and b (interpreted as unsigned 32-bit values)
// and pushes the lower 32 bits of the result back onto the stack. Overflows are
// silently ignored, the result is truncated to an unsigned 32 bit value.
//
// NOTE: This instruction can be used to multiply two signed (two's complement)
// values. It can be shown that the lower 32-bit result word of a 32x32 multiplication
// can be computed with the same algorithm irrespective of wether binary (unsigned)
// or two's complement (signed) numbers are used. Only the upper 32-bit result
// word (which is not computed by this instructions) needs different handling.
//
// Syntax example:
//   MUL
code(Ret) ::= code(Code) MUL(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_MUL);
  AsmTokenDiscard(&Hint);
}

// UDIV: Divides two unsigned operands (a / b)
//
// Stack: ... a b => ... z
//
// Divides value a by value b (both are unsigned 32-bit values) and pushes
// the quotient to the operand stack. The b operand must not be zero.
//
// Syntax Example:
//   UDIV
code(Ret) ::= code(Code) UDIV(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_UDIV);
  AsmTokenDiscard(&Hint);
}

// SDIV: Divides two signed operands (a / b)
//
// Stack: ... a b => ... z
//
// Divides value a by value b (both are signed 32-bit values) and pushes
// the quotient to the operand stack. The b operand must not be zero and the
// quotient must be representable as a signed (two's complement) 32-bit integer.
//
// Syntax Example:
//   SDIV
code(Ret) ::= code(Code) SDIV(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SDIV);
  AsmTokenDiscard(&Hint);
}

// SHL: Logical shift left (a << b)
//
// Stack: ... a b => .. z
//
// Shifts value a by b (mod 32) bits to the left and pushes the result to the operand
// stack. The result is silently truncated to 32 bits. Values a and b are interpreted
// as unsigned integers.
//
// Syntax Example:
//   SHL
code(Ret) ::= code(Code) SHL(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SHL);
  AsmTokenDiscard(&Hint);
}

// SHR: Logical (unsigned) shift right (a >> b)
//
// Stack: ... a b => .. z
//
// Shifts value a by b (mod 32) bits to the right and pushes the result to the operand
// stack. This instruction performs a logical shift, bits shifted in from the left are
// set to zero. Values a and b are interpreted as unsigned integers.
//
// Syntax Example:
//   SHL
code(Ret) ::= code(Code) SHR(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_SHR);
  AsmTokenDiscard(&Hint);
}

// ASR: Arithmetic (signed) shift right (a >> b)
//
// Stack: ... a b => .. z
//
// Shifts value a by b (mod 32) bits to the right and pushes the result to the operand
// stack. This instruction performs an arithmetic shift, bits shifted in from the left
// are identical to the sign-bit of the a operand. Value a is interpreted as signed
// (two's complement) integer. Value b is interpreted as unsigned integers.
//
// Syntax Example:
//   ASR
code(Ret) ::= code(Code) ASR(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_ASR);
  AsmTokenDiscard(&Hint);
}

// ROR: Logical rotate right (a ror b)
//
// Stack: ... a b => .. z
//
// Rotates value a by b (mod 32) bits to the right and pushes the result to the
// operand stack. This operation performs a logic shift right, bits shifted in from
// the left are the bits falling out at the right. Values a and b are interpreted
// as unsigned integers.
//
// Syntax Example:
//   ROR
code(Ret) ::= code(Code) ROR(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_ROR);
  AsmTokenDiscard(&Hint);
}

// AND: Bit-wise AND (a & b)
//
// Stack: ... a b => .. z
//
// Computes the bit-wise AND of values a and b and pushes the result to the
// operand stack. Values a and b are interpreted as unsigned integers.
//
// Syntax Example:
//   AND
code(Ret) ::= code(Code) AND(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_AND);
  AsmTokenDiscard(&Hint);
}

// OR: Bit-wise OR (a | b)
//
// Stack: ... a b => .. z
//
// Computes the bit-wise OR of values a and b and pushes the result to the
// operand stack. Values a and b are interpreted as unsigned integers.
//
// Syntax Example:
//   OR
code(Ret) ::= code(Code) OR(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_OR);
  AsmTokenDiscard(&Hint);
}

// XOR: Bit-wise exclusive-OR (a ^ b)
//
// Stack: ... a b => .. z
//
// Computes the bit-wise exclusive-OR of values a and b and pushes the result to
// the operand stack. Values a and b are interpreted as unsigned integers.
//
// Syntax Example:
//   XOR
code(Ret) ::= code(Code) XOR(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_XOR);
  AsmTokenDiscard(&Hint);
}

// NOT: Bit-wise not / one's complement (~a)
//
// Stack: ... a => .. z
//
// Computes the bit-wise NOT of value a and pushes the result to
// the operand stack.
//
// Syntax Example:
//   NOT
code(Ret) ::= code(Code) NOT(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_NOT);
  AsmTokenDiscard(&Hint);
}

// NEG: Negation / two's complement (-a)
//
// Stack: ... a => .. z
//
// Computes the negation (two's complement) of value a and pushes
// the result to the operand stack.
//
// Syntax Example:
//   NEG
code(Ret) ::= code(Code) NEG(Hint) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_NEG);
  AsmTokenDiscard(&Hint);
}

//----------------------------------------------------------------------
// Virtual Machine native code interaction (syscall-style interface)
//

// VMC: Virtual Machine Call
//
// Stack: (depends on called function)
//
// Virtual machine call names are defined in the asm_vmcallsids.gperf
// static lookup table. See vm/bytecodes.h for more details on the
// standard virtual machine calls.
//
// Syntax Example:
//   VMC(VM.EXIT)      // Call the named extension "VM.EXIT"
//   VMC(0.0)          // Call the virtual machine extension 0.0 (VM.EXIT)
code(Ret) ::= code(Code) VMC(Hint) vmcallid(callid) .
{
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Code, VM_OP_VMC);
  Ret = AsmUtilAppendCodeByte(pAsm, Hint, Ret, callid);
  AsmTokenDiscard(&Hint);
}

// Named virtual machine call
vmcallid(Ret) ::= LPAREN IDENTIFIER(CallName) RPAREN .
{
  // Lookup the call name
  const char *name = AsmTokenGetText(CallName);
  const AsmVmCallInfo *callinfo = AsmVmCallLookup(name, strlen(name));
  if (!callinfo) {
    // Unknown call (encode with call-id 255 to allow assembler to proceed)
    AsmError(pAsm, CallName, "unknown virtual machine call '%s'", name);
    Ret = 0xFF;

  } else {
    // Encode the call
    Ret = callinfo->callId;
  }

  AsmTokenDiscard(&CallName);
}

// Virtual Machine Call ID (debug variant)
//
// Using this syntax produces an assembler warning, unless advanced (debug)
// instructions are enabled in the assembler. (see "-a" command line option)
//
// This syntax allows VM developers and testers to produce (arbitrary)
// virtual machine call IDs and hints for testing the virtual machine.
vmcallid(Ret) ::= LPAREN u4(MajorCallId) COMMA u4(MinorCallId) RPAREN .
{
  Ret = (MajorCallId << 4) | MinorCallId;
}


//----------------------------------------------------------------------
// Immediate operands and helper productions
//

// Single 32-bit signed or unsigned immediate operand.
//
// The value is returned as 64-bit signed integer between
// INT32_MIN and UINT32_MAX.
imm32(Ret) ::= INT(Value) .
{
  Ret = AsmUtilParseInteger(pAsm, Value, INT32_MIN, UINT32_MAX);
  AsmTokenDiscard(&Value);
}

// Handle given by object name.
//
// This syntax is the standard way to refer to named
// data (or functions) objects in  "HANDLE" and "CALL" instructions.
//
handle(Ret) ::= LPAREN IDENTIFIER(GlobalName) RPAREN .
{
  AsmForwardRefInit(&Ret, GlobalName, 0, 0);
}

// Handle given by raw 32-bit handle value. (debug variant)
//
handle(Ret) ::= LPAREN u32(Value) RPAREN .
{
  AsmForwardRefInit(&Ret, NULL, Value, 0);
}

// Single 4-bit unsigned value
u4(Ret) ::= INT(Value) .
{
  Ret = AsmUtilParseInteger(pAsm, Value, 0, ASM_UINT4_MAX);
  AsmTokenDiscard(&Value);
}

// Single 8-bit unsigned value
u8(Ret) ::= INT(Value) .
{
  Ret = AsmUtilParseInteger(pAsm, Value, 0, UINT8_MAX);
  AsmTokenDiscard(&Value);
}

// Single 8-bit signed or unsigned value.
any8(Ret) ::= INT(Value) .
{
  Ret = AsmUtilParseInteger(pAsm, Value, INT8_MIN, UINT8_MAX);
  AsmTokenDiscard(&Value);
}

// Single 16-bit signed value
i16(Ret) ::= INT(Value) .
{
  Ret = AsmUtilParseInteger(pAsm, Value, INT16_MIN, INT16_MAX);
  AsmTokenDiscard(&Value);
}

// Single 32-bit unsigned value
//
// Values outside the 32-bit unsigned range (0..4294967295) are
// truncated and produce an assembler warning.
u32(Ret) ::= INT(Value) .
{
  Ret = AsmUtilParseInteger(pAsm, Value, 0, UINT32_MAX);
  AsmTokenDiscard(&Value);
}

