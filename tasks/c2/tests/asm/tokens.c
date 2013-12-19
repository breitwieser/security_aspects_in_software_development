///
/// \file
/// \brief Token handling unit test for the tinyvm assembler.
///
#include "tests.h"

//----------------------------------------------------------------------
void TestTokenCreateSimple(void)
{
  static const char constText[] = "hello";

  // Create a token with given text
  AsmToken* token = AsmTokenNew(42, 1, 2, constText, strlen(constText));
  CU_ASSERT_PTR_NOT_NULL_FATAL(token);

  // Line and column numbers
  CU_ASSERT(AsmTokenGetLine(token) == 1);
  CU_ASSERT(AsmTokenGetColumn(token) == 2);

  // Token text and type
  const char *tokenText = AsmTokenGetText(token);
  CU_ASSERT_PTR_NOT_NULL_FATAL(tokenText);
  CU_ASSERT_PTR_NOT_EQUAL(constText, tokenText);
  CU_ASSERT_STRING_EQUAL(constText, tokenText);

  CU_ASSERT_EQUAL(AsmTokenGetType(token), 42);

  AsmTokenDelete(token);
}

//----------------------------------------------------------------------
void TestTokenCreateEmpty(void)
{
  // Create a token with empty text (non-NULL pointer, zero length)
  AsmToken* token = AsmTokenNew(42, 0, 0, "", 0);
  CU_ASSERT_PTR_NOT_NULL_FATAL(token);

  // Token text
  const char *tokenText = AsmTokenGetText(token);
  CU_ASSERT_PTR_NOT_NULL_FATAL(tokenText);
  CU_ASSERT_STRING_EQUAL(tokenText, "");

  AsmTokenDelete(token);
}

//----------------------------------------------------------------------
void TestTokenCreateNull(void)
{
  // Create a token with empty text (NULL pointer, zero length)
  AsmToken* token = AsmTokenNew(42, 0, 0, NULL, 0);
  CU_ASSERT_PTR_NOT_NULL_FATAL(token);

  // Token text
  const char *tokenText = AsmTokenGetText(token);
  CU_ASSERT_PTR_NOT_NULL_FATAL(tokenText);
  CU_ASSERT_STRING_EQUAL(tokenText, "");
  CU_ASSERT_EQUAL(AsmTokenGetType(token), 42);

  AsmTokenDelete(token);
}

//----------------------------------------------------------------------
void TestTokenBadCreateNull(void)
{
  // Bad attempt to create a token with empty text (NULL pointer, non-zero length)
  AsmToken* token = AsmTokenNew(42, 0, 0, NULL, 27);
  CU_ASSERT_PTR_NULL(token);
}

//----------------------------------------------------------------------
void TestTokenBadCreateOverflow(void)
{
  // Bad attempt to create a token (integer overflow in size computation)  
  AsmToken* token = AsmTokenNew(42, 0, 0, "dummy value", SIZE_MAX);
  CU_ASSERT_PTR_NULL(token);
}


//----------------------------------------------------------------------
void TestTokenOutOfMemory(void)
{
  static const char TOKEN_TEXT[] = "Fuzzy the Bug says: Dont Feed The "
    "Bugs! This is a long token text of bug feeding!";
  size_t length = strlen(TOKEN_TEXT);

  // Try #1: Create the token (we have enough memory)
  AsmToken *token1 = AsmTokenNew(42, 0, 0, TOKEN_TEXT, length);
  CU_ASSERT_PTR_NOT_NULL_FATAL(token1);
  CU_ASSERT_PTR_NOT_EQUAL(AsmTokenGetText(token1), TOKEN_TEXT);
  CU_ASSERT_STRING_EQUAL(AsmTokenGetText(token1), TOKEN_TEXT);
  AsmTokenDelete(token1);

  // Try #2: Fail all allocations larger than 1 byte
  TestSetAllocSizeLimit(1);
  AsmToken *token2 = AsmTokenNew(42, 0, 0, TOKEN_TEXT, length);
  CU_ASSERT_PTR_NULL(token2);
  
  // Try #2: Fail allocations larger than "length" bytes
  //  (no space for the trailing '\0')
  TestSetAllocSizeLimit(length);
  AsmToken *token3 = AsmTokenNew(42, 0, 0, TOKEN_TEXT, length);
  CU_ASSERT_PTR_NULL(token3);
}

//----------------------------------------------------------------------
void TestTokenBadGettersNull(void)
{
  CU_ASSERT_PTR_NULL(AsmTokenGetText(NULL));
  CU_ASSERT_EQUAL(AsmTokenGetLine(NULL), 0);
  CU_ASSERT_EQUAL(AsmTokenGetColumn(NULL), 0);
  CU_ASSERT_EQUAL(AsmTokenGetType(NULL), 0);
}

//----------------------------------------------------------------------
void TestTokenBadDeleteNull(void)
{
  // Try to delete the NULL token
  AsmTokenDelete(NULL);
  CU_PASS();
}
