///
/// \file
/// \brief Test for the complete assemble/link/emit cycle
///
#include "tests.h"

//----------------------------------------------------------------------
static Buffer* TestBuildProgram(const char *resname, bool bigendian)
{
  // Parse the program
  size_t code_length = 0;
  const char *code = TestGetResource(&code_length, resname);

  AsmLexer *lexer = AsmLexerFromString(code, code_length);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // And let the assembler parse the source code
  Assembler *assembler = AsmNew(ASM_VERBOSITY_QUIET, bigendian);
  CU_ASSERT_PTR_NOT_NULL_FATAL(assembler);
  CU_ASSERT_TRUE(AsmParse(assembler, lexer));
  CU_ASSERT_EQUAL(AsmGetErrorCount(assembler), 0);

  // The lexer is no longer needed
  AsmLexerDelete(lexer);

  // Now link the symbols of the arc4 example program
  CU_ASSERT_TRUE(AsmLink(assembler));
  CU_ASSERT_EQUAL(AsmGetErrorCount(assembler), 0);

  // Build a memory buffer for receiving the bytecode
  Buffer *bytecode = BufferCreate();
  CU_ASSERT_PTR_NOT_NULL_FATAL(bytecode);

  // Write the resulting bytecode to a buffer
  CU_ASSERT_TRUE(AsmWriteToBuffer(bytecode, assembler));

  // Dispose the assembler
  AsmDelete(assembler);

  // NOTE: We currently only check that the buffer is not empty ...
  CU_ASSERT(BufferGetLength(bytecode) > 0);

  // And return the buffer
  return bytecode;
}

//----------------------------------------------------------------------
void TestAsmBuildHello(void)
{
  // Build the little-endian variant
  BufferFree(TestBuildProgram("src/simple.txt", false));

  // And the big-endian variant
  BufferFree(TestBuildProgram("src/simple.txt", true));
}

//----------------------------------------------------------------------
void TestAsmBuildArc4(void)
{
  // Build the little-endian variant
  BufferFree(TestBuildProgram("src/arc4.txt", false));

  // And the big-endian variant
  BufferFree(TestBuildProgram("src/arc4.txt", true));
}
