///
/// \file
/// \brief Unit tests for the parser.
///
#include "tests.h"

//----------------------------------------------------------------------
void TestAsmCreateSimple(void)
{
  // Create and delete an assembler object
  Assembler *assembler = AsmNew(ASM_VERBOSITY_QUIET, true);
  CU_ASSERT_PTR_NOT_NULL_FATAL(assembler);

  AsmDelete(assembler);
}

//----------------------------------------------------------------------
void TestAsmDeleteNull(void)
{
  // Try to delete the NULL assembler object
  AsmDelete(NULL);
}

//----------------------------------------------------------------------
void TestAsmParseSimple(void)
{
  // Simple parsing example (Hello world demo implementation)

  // Prepare the lexer
  size_t code_length = 0;
  const char *code = TestGetResource(&code_length, "src/simple.txt");

  AsmLexer *lexer = AsmLexerFromString(code, code_length);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // And let the assembler parse the source code
  Assembler *assembler = AsmNew(ASM_VERBOSITY_QUIET, true);
  CU_ASSERT_PTR_NOT_NULL_FATAL(assembler);

  // And parse
  CU_ASSERT_TRUE(AsmParse(assembler, lexer));
  CU_ASSERT_EQUAL(AsmGetErrorCount(assembler), 0);

  // Dispose the objects
  AsmDelete(assembler);
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestAsmBadOutOfMemoryCreate(void)
{
  // Let next malloc allocation fail
  TestSetAllocFaultCountdown(1);

  // Try to allocate an assembler object (will fail due to
  // simulated out of memory)
  Assembler *assembler = AsmNew(ASM_VERBOSITY_QUIET, true);
  CU_ASSERT_PTR_NULL(assembler);
}
