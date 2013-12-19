///
/// \file
/// \brief Unit tests for the lexical scanner.
///
#include "tests.h"
#include "grammar.h" // Token definitions of the assembler

//----------------------------------------------------------------------
#define TEST_EXPECT_TOKEN(_lexer,_type)					\
  do {									\
    AsmToken *test_token = NULL;					\
    CU_ASSERT_EQUAL(AsmLexerScan(&test_token, _lexer), ASM_LEXER_SUCCESS); \
    CU_ASSERT_EQUAL(ASM_TOKEN_##_type, AsmTokenGetType(test_token));	\
    AsmTokenDelete(test_token);						\
  } while(0)

//----------------------------------------------------------------------
#define TEST_EXPECT_TOKEN_TEXT(_lexer,_type,_text)			\
  do {									\
    AsmToken *test_token = NULL;					\
    CU_ASSERT_EQUAL(AsmLexerScan(&test_token, _lexer), ASM_LEXER_SUCCESS); \
    CU_ASSERT_EQUAL(ASM_TOKEN_##_type, AsmTokenGetType(test_token));	\
    CU_ASSERT_PTR_NOT_NULL_FATAL(AsmTokenGetText(test_token));		\
    CU_ASSERT_STRING_EQUAL(_text, AsmTokenGetText(test_token));		\
    AsmTokenDelete(test_token);						\
  } while(0)

//----------------------------------------------------------------------
#define TEST_EXPECT_EOF(_lexer)						\
  do {									\
    AsmToken *test_token = NULL;					\
    CU_ASSERT_EQUAL(AsmLexerScan(&test_token, _lexer), ASM_LEXER_EOF);	\
    CU_ASSERT_PTR_NULL(test_token);					\
    AsmTokenDelete(test_token);						\
  } while(0)

//----------------------------------------------------------------------
#define TEST_EXPECT_ERROR(_lexer)					\
  do {									\
    AsmToken *test_token = NULL;					\
    CU_ASSERT_EQUAL(AsmLexerScan(&test_token, _lexer), ASM_LEXER_ERROR);\
    CU_ASSERT_PTR_NULL(test_token);					\
    AsmTokenDelete(test_token);						\
  } while(0)

//----------------------------------------------------------------------
#define TEST_SCAN_BAD_TOKEN(_text)					\
  do {									\
    static const char INPUT[] = _text;					\
    AsmToken *test_token = NULL;					\
    AsmLexer *lexer = AsmLexerFromString(INPUT, strlen(INPUT));		\
    CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);				\
    CU_ASSERT_EQUAL(AsmLexerScan(&test_token, lexer), ASM_LEXER_ERROR); \
    CU_ASSERT_PTR_NULL(test_token);					\
    AsmTokenDelete(test_token);						\
    AsmLexerDelete(lexer);						\
  } while (0)

//----------------------------------------------------------------------
typedef struct TestLexerStream {
  const char *position;
  unsigned deleteCounter;
} TestLexerStream;

//----------------------------------------------------------------------
static void TestLexerStreamInit(TestLexerStream *stm, const char *text)
{
  stm->position = text;
  stm->deleteCounter = 0;
}

//----------------------------------------------------------------------
static int TestLexerStreamRead(void *p_stream)
{
  TestLexerStream *stream = p_stream;
  unsigned char c = *stream->position;
  if (c == '\0') {
    return ASM_LEXER_EOF;
  }

  // Advance the position
  stream->position += 1;

  // I/O error simulation
  switch (c) {
  case '!':
    // Simulate an I/O error
    return ASM_LEXER_ERROR;

  default:
    // Other characters are passed to the lexer
    return c;
  }
  if (c == '!') {
    return ASM_LEXER_ERROR;
  }

  return c;
}

//----------------------------------------------------------------------
static void TestLexerStreamDelete(void *p_stream)
{
  TestLexerStream *stream = p_stream;
  stream->deleteCounter += 1;
}

//----------------------------------------------------------------------
void TestLexerCreateStrSimple(void)
{
  static const char DUMMY[] = "dummy text";

  // Create a simple string lexer
  AsmLexer *lexer = AsmLexerFromString(DUMMY, strlen(DUMMY));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Discard again
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerCreateStrEmpty(void)
{
  // Create an empty string lexer (with non-null string and zero size)
  AsmLexer *lexer = AsmLexerFromString("", 0);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerCreateStrNull(void)
{
  // Create an empty string lexer (with null string and zero size)
  AsmLexer *lexer = AsmLexerFromString(NULL, 0);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadCreateExtNull(void)
{
  // Try to create an "extended" lexer with null read callback
  TestLexerStream stm;
  TestLexerStreamInit(&stm, "");

  // Step 1: Try to create a lexer with both callbacks set to NULL
  // (creation fails)
  AsmLexer *lexer1 = AsmLexerCreate(NULL, NULL, &stm);
  CU_ASSERT_PTR_NULL(lexer1);

  // Try 2: Try to create a lexer withh NULL read callback
  // (creation fails, delete callback WILL be invoked)
  CU_ASSERT_EQUAL(stm.deleteCounter, 0);
  AsmLexer *lexer2 = AsmLexerCreate(NULL, TestLexerStreamDelete, &stm);
  CU_ASSERT_PTR_NULL(lexer2);
  CU_ASSERT_EQUAL(stm.deleteCounter, 1);
}

//----------------------------------------------------------------------
void TestLexerCreateExt(void)
{
  // Create an "extended" lexer with read and delete callbacks
  TestLexerStream stm;
  TestLexerStreamInit(&stm, "");

  AsmLexer *lexer = AsmLexerCreate(TestLexerStreamRead,
				   TestLexerStreamDelete, &stm);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Lexer has been created but not yet deleted
  CU_ASSERT_EQUAL(stm.deleteCounter, 0);

  // Now delete the lexer
  AsmLexerDelete(lexer);

  // Delete callback must have been called
  CU_ASSERT_EQUAL(stm.deleteCounter, 1);
}

//----------------------------------------------------------------------
void TestLexerCreateNoDeleteCb(void)
{
  // Create an "extended" lexer with read but no delete callback.
  TestLexerStream stm;
  TestLexerStreamInit(&stm, "");

  AsmLexer *lexer = AsmLexerCreate(TestLexerStreamRead, NULL, &stm);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Now delete the lexer
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanEmpty(void)
{
  // Scan the empty string (this will only produce an EOF)
  AsmLexer *lexer = AsmLexerFromString("", 0);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  TEST_EXPECT_EOF(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanSpaces(void)
{
  static const char SPACES[] = "  \t \r  \n\r  \t  \r\n  \r  \n \r \n";

  // Scan a string with only whitespaces (this will only produce an EOF)
  AsmLexer *lexer = AsmLexerFromString(SPACES, strlen(SPACES));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  TEST_EXPECT_EOF(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanChars(void)
{
  // Scan all valid single character tokens
  static const char SPECIALS[] = ":(),";

  AsmLexer *lexer = AsmLexerFromString(SPECIALS, strlen(SPECIALS));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  TEST_EXPECT_TOKEN(lexer, DOUBLECOLON);
  TEST_EXPECT_TOKEN(lexer, LPAREN);
  TEST_EXPECT_TOKEN(lexer, RPAREN);
  TEST_EXPECT_TOKEN(lexer, COMMA);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanIntegers(void)
{
  // Various valid integers
  static const char INTS[] =
    "123 -123 123456789 10 100 1020 "
    "0x12cAfE00 0xDEADBEEF -0x3 0xdEaDbeeF 0xabcdef 0xABCDEF 0XF00 "
    "0377 0755 0644 -0123 0456 -0777 "
    "0 0x0 0x00 000 -0 -0x0 -00 "
    "1000000000000000000000000000000000000000";


  AsmLexer *lexer = AsmLexerFromString(INTS, strlen(INTS));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Decimal numbers
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "123");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "-123");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "123456789");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "10");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "100");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "1020");

  // Hexadecimal numbers
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0x12CAFE00");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0xDEADBEEF");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "-0x3");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0xDEADBEEF");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0xABCDEF");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0xABCDEF");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0xF00");

  // Octal numbers
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0377");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0755");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0644");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "-0123");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0456");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "-0777");

  // Variants of zero
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0x0");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0x00");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "000");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "-0");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "-0x0");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "-00");

  // A very large integer (which surely would not fit into int64_t)
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "1000000000000000000000000000000000000000");

  TEST_EXPECT_EOF(lexer);
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanIdentifiers(void)
{
  // Various valid identifiers (also in combination with other tokens)
  static const char IDENTS[] = "simple SiMpLE with123 with789ToO "
    "_another more_identifiers complex123_identifier_456_ "
    "first:second _(strange,but,valid)";

  AsmLexer *lexer = AsmLexerFromString(IDENTS, strlen(IDENTS));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Simple cases
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "simple");
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "SiMpLE");
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "with123");
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "with789ToO");
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "_another");
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "more_identifiers");
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "complex123_identifier_456_");

  // "first" and "second" with a space in between
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "first");
  TEST_EXPECT_TOKEN(lexer, DOUBLECOLON);
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "second");

  // strange variations
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "_");
  TEST_EXPECT_TOKEN(lexer, LPAREN);
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "strange");
  TEST_EXPECT_TOKEN(lexer, COMMA);
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "but");
  TEST_EXPECT_TOKEN(lexer, COMMA);
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "valid");
  TEST_EXPECT_TOKEN(lexer, RPAREN);

  TEST_EXPECT_EOF(lexer);
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanLocalNames(void)
{
  // Various valid local (variable) names
  static const char NAMES[] = "@name @arg0 @VAR1 @_AnOthEr_123 @_ "
    "@ADD @0";

  AsmLexer *lexer = AsmLexerFromString(NAMES, strlen(NAMES));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Simple cases
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@name");
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@arg0");
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@VAR1");
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@_AnOthEr_123");
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@_");

  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@ADD");
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@0");

  TEST_EXPECT_EOF(lexer);
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanSimple(void)
{
  // Simple input lines to scan
  static const char LINES[] = "label1: .function(1) MyFunction_First(@a, @b) // comment\n";

  // Initialize a string lexer
  AsmLexer *lexer = AsmLexerFromString(LINES, strlen(LINES));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Check the expected token sequence
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "label1");
  TEST_EXPECT_TOKEN(lexer, DOUBLECOLON);
  TEST_EXPECT_TOKEN(lexer, DOT_FUNCTION);
  TEST_EXPECT_TOKEN(lexer, LPAREN);
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "1");
  TEST_EXPECT_TOKEN(lexer, RPAREN);
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "MyFunction_First");
  TEST_EXPECT_TOKEN(lexer, LPAREN);
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@a");
  TEST_EXPECT_TOKEN(lexer, COMMA);
  TEST_EXPECT_TOKEN_TEXT(lexer, LOCAL_NAME, "@b");
  TEST_EXPECT_TOKEN(lexer, RPAREN);
  TEST_EXPECT_EOF(lexer);

  // Delete the string lexer
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanExtSimple(void)
{
  // Simple input lines to scan (this time use the "extended" lexer interface)
  static const char LINES[] =
    "// this is a simple program\n.object[0] data MyDataObject\n .byte (1, 2)\n"
    " .handle (MyDataObject)\n.end\n";

  TestLexerStream stm;
  TestLexerStreamInit(&stm, LINES);


  // Creation will fail due to out of memory condition. The delete
  // callback is invoked in the error path.
  AsmLexer *lexer = AsmLexerCreate(TestLexerStreamRead, NULL, &stm);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Initial location should be line 1, column 0
  CU_ASSERT_EQUAL(AsmLexerGetLine(lexer), 1);
  CU_ASSERT_EQUAL(AsmLexerGetColumn(lexer), 0);

  // Check the expected token sequence
  TEST_EXPECT_TOKEN(lexer, DOT_OBJECT);
  TEST_EXPECT_TOKEN(lexer, LBRACK);
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "0");
  TEST_EXPECT_TOKEN(lexer, RBRACK);
  TEST_EXPECT_TOKEN(lexer, DATA);
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "MyDataObject");
  TEST_EXPECT_TOKEN(lexer, DOT_BYTE);
  TEST_EXPECT_TOKEN(lexer, LPAREN);
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "1");
  TEST_EXPECT_TOKEN(lexer, COMMA);
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "2");
  TEST_EXPECT_TOKEN(lexer, RPAREN);
  TEST_EXPECT_TOKEN(lexer, DOT_HANDLE);
  TEST_EXPECT_TOKEN(lexer, LPAREN);
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "MyDataObject");
  TEST_EXPECT_TOKEN(lexer, RPAREN);
  TEST_EXPECT_TOKEN(lexer, DOT_END);
  TEST_EXPECT_EOF(lexer);

  // Final location should be line 6, column 0
  CU_ASSERT_EQUAL(AsmLexerGetLine(lexer), 6);
  CU_ASSERT_EQUAL(AsmLexerGetColumn(lexer), 0);

  // Delete the string lexer
  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerScanInvalidCharRecovery(void)
{
  // Scan an input stream with invalid first character
  static const char INPUT[] = "! invalid$$ first %character";

  AsmLexer *lexer = AsmLexerFromString(INPUT, strlen(INPUT));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Scan of first '!' character will fail
  TEST_EXPECT_ERROR(lexer);

  // Lexer should recover and scan the "invalid" identifier
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "invalid");

  // Scan of the next two '$' fails
  TEST_EXPECT_ERROR(lexer);
  TEST_EXPECT_ERROR(lexer);

  // Lexer should recover and scan "first" identifier
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "first");

  // Scan of the '%' fails
  TEST_EXPECT_ERROR(lexer);

  // Lexer should recover and scan "character" identifier
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "character");

  // Now we epxect the end of input
  TEST_EXPECT_EOF(lexer);

  AsmLexerDelete(lexer);
}


//----------------------------------------------------------------------
void TestLexerBadCreateStrNull(void)
{
  // Try to create a string lexer with NULL string and non-zero size
  // Create an empty string lexer (with null string and zero size)
  AsmLexer *lexer = AsmLexerFromString(NULL, 42);
  CU_ASSERT_PTR_NULL(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadDeleteNull(void)
{
  // Try to delete the null lexer (behaves as no-operation)
  AsmLexerDelete(NULL);
}

//----------------------------------------------------------------------
void TestLexerBadGettersNull(void)
{
  // Try to get line/column information of the null lexer
  CU_ASSERT_EQUAL(AsmLexerGetLine(NULL), 0);
  CU_ASSERT_EQUAL(AsmLexerGetColumn(NULL), 0);
}

//----------------------------------------------------------------------
void TestLexerBadScanNull(void)
{
  AsmToken *token = NULL;

  // Test with NULL lexer object
  CU_ASSERT_EQUAL(AsmLexerScan(NULL,   NULL), ASM_LEXER_ERROR);
  CU_ASSERT_EQUAL(AsmLexerScan(&token, NULL), ASM_LEXER_ERROR);

  // Test with NULL token
  static const char LINE[] = "simple test";
  AsmLexer *lexer = AsmLexerFromString(LINE, strlen(LINE));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Scan the first token
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "simple");

  // Invalid scan (token=NULL)
  CU_ASSERT_EQUAL(AsmLexerScan(NULL, lexer), ASM_LEXER_ERROR);

  // Scan the second token
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "test");
  TEST_EXPECT_EOF(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadScanComment(void)
{
  // Scan an invalid comment, which does not start with "//"
  static const char INPUT[] = " /? Bad comment";

  AsmLexer *lexer = AsmLexerFromString(INPUT, strlen(INPUT));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  TEST_EXPECT_ERROR(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadScanCommentEof(void)
{
  // Scan an invalid comment (end of file after first '/')
  static const char INPUT[] = " /";

  AsmLexer *lexer = AsmLexerFromString(INPUT, strlen(INPUT));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  TEST_EXPECT_ERROR(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadScanIntegers(void)
{
  TEST_SCAN_BAD_TOKEN("0129");   // Invalid digit '9' in octal integer
  TEST_SCAN_BAD_TOKEN("0876");   // Invalid digit '8' in octal integer
  TEST_SCAN_BAD_TOKEN("0787");   // Invalid digit '8' in octal integer

  TEST_SCAN_BAD_TOKEN("12C6");   // Invalid digit 'c' in decimal integer
  TEST_SCAN_BAD_TOKEN("12Z3");   // Invalid digit 'Z' in decimal integer


  TEST_SCAN_BAD_TOKEN("0x");        // Invalid hex integer (no digits)
  TEST_SCAN_BAD_TOKEN("0X");        // Invalid hex integer (no digits)
  TEST_SCAN_BAD_TOKEN("0xABCDEFG"); // Invalid digit "G" in hex integer
  TEST_SCAN_BAD_TOKEN("0xabcdefg"); // Invalid digit "g" in hex integer
  TEST_SCAN_BAD_TOKEN("0x0Z456");   // Invalid digit "Z" in hex integer
  TEST_SCAN_BAD_TOKEN("0x0z987");   // Invalid digit "z" in hex integer
  TEST_SCAN_BAD_TOKEN("0xK0456");   // Invalid digit "K" in hex integer
  TEST_SCAN_BAD_TOKEN("0xk987");    // Invalid digit "k" in hex integer
  TEST_SCAN_BAD_TOKEN("0x012R46");  // Invalid digit "R" in hex integer
  TEST_SCAN_BAD_TOKEN("0x0879r0");  // Invalid digit "r" in hex integer

  TEST_SCAN_BAD_TOKEN("0_foo");         // Invalid integer/identifier sequence
  TEST_SCAN_BAD_TOKEN("1_foo");         // Invalid integer/identifier sequence
  TEST_SCAN_BAD_TOKEN("0x3f_badIdent"); // Invalid integer/identifier sequence
  TEST_SCAN_BAD_TOKEN("03_EvilBugs");   // Invalid integer/identifier sequence

  TEST_SCAN_BAD_TOKEN("-");          // Standalone minus
  TEST_SCAN_BAD_TOKEN("-_");         // Standalone minus/identifier sequence
}

//----------------------------------------------------------------------
void TestLexerBadScanDotKeyword(void)
{
  // Try to scan an invalid ".invalid" keyword
  // (Lexer will scan ".invalid", AsmKeywordLookup fails)
  static const char INPUT[] = ".invalid";

  AsmLexer *lexer = AsmLexerFromString(INPUT, strlen(INPUT));
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Keyword scan fails (the lexer will scan a ".invalid" identifier,
  // the AsmKeywordLookup phase fails)
  TEST_EXPECT_ERROR(lexer);

  // We are now at the end of file (after '.invalid')
  TEST_EXPECT_EOF(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadScanIoError(void)
{
  // Lexer test stream with an I/O error. (the I/O error is simulated
  // at the '!' position)
  //
  TestLexerStream stm;
  TestLexerStreamInit(&stm, "good 123 !never scanned");

  AsmLexer *lexer = AsmLexerCreate(TestLexerStreamRead,
				   TestLexerStreamDelete, &stm);
  CU_ASSERT_PTR_NOT_NULL_FATAL(lexer);

  // Scanning of "good" and 123 should word ...
  TEST_EXPECT_TOKEN_TEXT(lexer, IDENTIFIER, "good");
  TEST_EXPECT_TOKEN_TEXT(lexer, INT, "123");

  // ... scanning will fail at simulated read callback error (!)
  TEST_EXPECT_ERROR(lexer);

  AsmLexerDelete(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadOutOfMemoryStr(void)
{
  // Out of memory during string lexer construction

  // Let the next allocation fail
  TestSetAllocFaultCountdown(1);

  // Trying to create a simple string lexer ...
  static const char INPUT[] = "never scanned";
  AsmLexer *lexer = AsmLexerFromString(INPUT, strlen(INPUT));

  // ... fails due to simulated out of memory!
  CU_ASSERT_PTR_NULL(lexer);
}

//----------------------------------------------------------------------
void TestLexerBadOutOfMemoryExt(void)
{
  // Try to create an "extended" lexer with read and but no delete
  // callback.
  TestLexerStream stm;
  TestLexerStreamInit(&stm, "");

  // Let the next allocation fail
  TestSetAllocFaultCountdown(1);

  // Creation will fail due to out of memory condition. The delete
  // callback is invoked in the error path.
  AsmLexer *lexer = AsmLexerCreate(TestLexerStreamRead,
				   NULL, &stm);
  CU_ASSERT_PTR_NULL(lexer);
}


//----------------------------------------------------------------------
void TestLexerBadOutOfMemoryDeleteCb(void)
{
  // Try to create an "extended" lexer with read and delete callbacks
  TestLexerStream stm;
  TestLexerStreamInit(&stm, "");

  // Let the next allocation fail
  TestSetAllocFaultCountdown(1);

  // Creation will fail due to out of memory condition. The delete
  // callback is invoked in the error path.
  AsmLexer *lexer = AsmLexerCreate(TestLexerStreamRead,
				   TestLexerStreamDelete, &stm);
  CU_ASSERT_PTR_NULL(lexer);

  // The delete callback must have been called exactly once
  CU_ASSERT_EQUAL(stm.deleteCounter, 1);
}
