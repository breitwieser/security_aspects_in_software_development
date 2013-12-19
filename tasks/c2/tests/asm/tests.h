///
/// \file
/// \brief Common header for the tinyvm assembler unit tests.
///
#ifndef ASSEMBLER_TESTS_H
#define ASSEMBLER_TESTS_H

// Common test helpers
#include "testhelpers.h"

// Public include file for the TinyVM assembler API.
#include "assembler.h"

// Unit tests for token handling (defined in tokens.c)
void TestTokenCreateSimple(void);
void TestTokenCreateEmpty(void);
void TestTokenCreateNull(void);
void TestTokenBadCreateNull(void);
void TestTokenBadCreateOverflow(void);
void TestTokenBadGettersNull(void);
void TestTokenBadDeleteNull(void);
void TestTokenOutOfMemory(void);

// Unit tests for buffer handling (defined in buffer.c)
void TestBufferCreateSimple(void);
void TestBufferClearZero(void);
void TestBufferAppendEmpty(void);
void TestBufferAppendGrowing(void);
void TestBufferAppendBytewise(void);
void TestBufferAppendMixed(void);
void TestBufferAppendMixedLittleEndian(void);
void TestBufferBadCreateNull(void);
void TestBufferBadDeleteNull(void);
void TestBufferBadGettersNull(void);
void TestBufferBadAppendNull(void);
void TestBufferBadAppendOverflow(void);
void TestBufferBadOutOfMemorySingle(void);

// Unit tests for the lexical analyser (defined in lexer.c)
void TestLexerCreateStrSimple(void);
void TestLexerCreateStrEmpty(void);
void TestLexerCreateStrNull(void);
void TestLexerCreateExt(void);
void TestLexerCreateNoDeleteCb(void);
void TestLexerScanEmpty(void);
void TestLexerScanSpaces(void);
void TestLexerScanChars(void);
void TestLexerScanIntegers(void);
void TestLexerScanIdentifiers(void);
void TestLexerScanLocalNames(void);
void TestLexerScanSimple(void);
void TestLexerScanExtSimple(void);
void TestLexerBadDeleteNull(void);
void TestLexerBadGettersNull(void);
void TestLexerBadCreateStrNull(void);
void TestLexerBadCreateExtNull(void);
void TestLexerBadScanNull(void);
void TestLexerBadScanComment(void);
void TestLexerBadScanCommentEof(void);
void TestLexerBadScanIntegers(void);
void TestLexerScanInvalidCharRecovery(void);
void TestLexerBadScanDotKeyword(void);
void TestLexerBadScanIoError(void);
void TestLexerBadOutOfMemoryStr(void);
void TestLexerBadOutOfMemoryExt(void);
void TestLexerBadOutOfMemoryDeleteCb(void);

// Unit tests for the parser (defined in parser.c)
void TestAsmCreateSimple(void);
void TestAsmDeleteNull(void);
void TestAsmParseSimple(void);
void TestAsmBadOutOfMemoryCreate(void);

#endif // ASSEMBLER_TESTS_H
