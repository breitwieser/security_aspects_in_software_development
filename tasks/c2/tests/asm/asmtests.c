///
/// Test suite definitions for the tinyvm assembler.
///
#include "tests.h"

//----------------------------------------------------------------------
CU_SuiteInfo UNIT_TEST_SUITES[] = {
  // Token handling tests (defined in tokens.c)
  {
    .pName        = "asm/tokens",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "create.simple",       &TestTokenCreateSimple  },
      { "create.empty",        &TestTokenCreateEmpty   },
      { "create.null",         &TestTokenCreateNull    },
      { "bad.create.null",     &TestTokenBadCreateNull },
      { "bad.create.overflow", &TestTokenBadCreateOverflow },
      { "bad.getters.null",    &TestTokenBadGettersNull },
      { "bad.delete.null",     &TestTokenBadDeleteNull  },
      { "bad.outofmemory",     &TestTokenOutOfMemory    },
      CU_TEST_INFO_NULL,
    }
  },

  // Buffer handling tests suite (defined in buffer.c)
  {
    .pName        = "asm/buffers",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "create.simple",       &TestBufferCreateSimple  },
      { "clear.zero",          &TestBufferClearZero     },
      { "append.empty",        &TestBufferAppendEmpty   },
      { "append.growing",      &TestBufferAppendGrowing },
      { "append.bytewise",     &TestBufferAppendBytewise },
      { "append.mixed.big",    &TestBufferAppendMixed },
      { "append.mixed.little", &TestBufferAppendMixedLittleEndian },
      { "bad.delete.null",     &TestBufferBadDeleteNull },
      { "bad.create.null",     &TestBufferBadCreateNull },
      { "bad.getters.null",    &TestBufferBadGettersNull },
      { "bad.append.null",     &TestBufferBadAppendNull },
      { "bad.append.overflow", &TestBufferBadAppendOverflow },
      { "bad.oom.single",      &TestBufferBadOutOfMemorySingle },
      { "buffer.remaining",    &TestBufferRemaining},
      CU_TEST_INFO_NULL,
    }
  },

  // Buffer handling tests suite (defined in memview.c)
  {
    .pName        = "asm/memview",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "create.simple",       &TestMemViewCreateSimple  },
      CU_TEST_INFO_NULL,
    }
  },

  // Lexical scanner tests suite (defined in lexer.c)
  {
    .pName        = "asm/lexer",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "create.str.simple",    &TestLexerCreateStrSimple },
      { "create.str.empty",     &TestLexerCreateStrEmpty },
      { "create.str.null",      &TestLexerCreateStrNull},
      { "create.ext",           &TestLexerCreateExt },
      { "create.nodeletecb",    &TestLexerCreateNoDeleteCb },
      { "scan.empty",           &TestLexerScanEmpty },
      { "scan.spaces",          &TestLexerScanSpaces },
      { "scan.chars",           &TestLexerScanChars },
      { "scan.integers",        &TestLexerScanIntegers },
      { "scan.identifiers",     &TestLexerScanIdentifiers },
      { "scan.localnames",      &TestLexerScanLocalNames  },
      { "scan.simple",          &TestLexerScanSimple },
      { "scan.ext.simple",      &TestLexerScanExtSimple },
      { "scan.recovery",        &TestLexerScanInvalidCharRecovery },
      { "bad.delete.null",      &TestLexerBadDeleteNull },
      { "bad.getters.null",     &TestLexerBadGettersNull },
      { "bad.create.str.null",  &TestLexerBadCreateStrNull },
      { "bad.create.ext.null",  &TestLexerBadCreateExtNull },
      { "bad.scan.null",        &TestLexerBadScanNull },
      { "bad.scan.comment",     &TestLexerBadScanComment },
      { "bad.scan.commenteof",  &TestLexerBadScanCommentEof },
      { "bad.scan.integers",    &TestLexerBadScanIntegers },
      { "bad.scan.dot",         &TestLexerBadScanDotKeyword },
      { "bad.scan.ioerror",     &TestLexerBadScanIoError },
      { "bad.oom.str",          &TestLexerBadOutOfMemoryStr },
      { "bad.oom.ext",          &TestLexerBadOutOfMemoryExt },
      { "bad.oom.deletecb",     &TestLexerBadOutOfMemoryDeleteCb },
      CU_TEST_INFO_NULL,
    }
  },

  // Parser tests suite (defined in parser.c)
  {
    .pName        = "asm/parser",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "create.simple",       &TestAsmCreateSimple },
      { "delete.null",         &TestAsmDeleteNull },
      { "parse.simple",        &TestAsmParseSimple },
      { "parse.syntax",        &TestAsmParseSyntax },
      { "parse.syntax.swap",   &TestAsmParseSyntaxSwap },
      { "bad.oom.create",      &TestAsmBadOutOfMemoryCreate },
      CU_TEST_INFO_NULL,
    }
  },

  // Full assembler (assembler+link) test suite (defined in build.c)
  {
    .pName        = "asm/build",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "build.hello", &TestAsmBuildHello },
      { "build.arc4",  &TestAsmBuildArc4 },
      CU_TEST_INFO_NULL,
    }
  },

  // End of assembler test suites
  CU_SUITE_INFO_NULL
};
