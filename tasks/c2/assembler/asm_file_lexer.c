///
/// \file
/// \brief FILE-stream lexer.
///
#include "assembler.h"

#include <stdlib.h>
#include <assert.h>

//----------------------------------------------------------------------
static int AsmFileLexerRead(void *p_stm)
{
  assert (p_stm != NULL);
  FILE *file = p_stm;

  int c = fgetc(file);
  if (c == EOF) {
    if (feof(file)) {
      return ASM_LEXER_EOF; // Regular end of file
    }

    return ASM_LEXER_ERROR; // I/O error
  }

  // We have a new character
  assert (0 <= c && c <= 255);
  return c;
}

//----------------------------------------------------------------------
static void AsmFileLexerClose(void *p_stm)
{
  assert (p_stm != NULL);
  FILE *file = p_stm;

  fclose(file);
}

//----------------------------------------------------------------------
AsmLexer* AsmLexerFromFile(FILE *file, bool closeOnDelete)
{
  if (!file) {
    return NULL; // Bad arguments
  }

  // Select delete callback
  if (closeOnDelete) {
    // File should be closed on delete (or error)
    return AsmLexerCreate(AsmFileLexerRead, AsmFileLexerClose, file);

  } else {
    // File is managed by caller
    return AsmLexerCreate(AsmFileLexerRead, NULL, file);
  }
}
