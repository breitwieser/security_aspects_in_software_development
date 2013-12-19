///
/// \file
/// \brief In-memory string lexer.
///
#include "assembler.h"

#include <stdlib.h>
#include <assert.h>

//----------------------------------------------------------------------
typedef struct AsmStringLexStream {
  const char *position;
  size_t length;
} AsmStringLexStream;

//----------------------------------------------------------------------
static int AsmStringLexStreamRead(void *p_stm)
{
  assert (p_stm != NULL);

  AsmStringLexStream *stm = p_stm;
  if (stm->length == 0) {
    return ASM_LEXER_EOF;
  }

  assert (stm->position != NULL);

  unsigned char c = *stm->position;
  stm->length   -= 1;
  stm->position += 1;
  return c;
}

//----------------------------------------------------------------------
static void AsmStringLexStreamDelete(void *p_stm)
{
  assert (p_stm != NULL);
  free(p_stm);
}

//----------------------------------------------------------------------
AsmLexer* AsmLexerFromString(const char *str, size_t length)
{
  if (!str && length > 0) {
    return NULL; // Bad adrguments
  }

  // Allocate the lexer stream
  AsmStringLexStream* stm = malloc(sizeof(AsmStringLexStream));
  if (stm == NULL) {
    return NULL; // Out of memory
  }

  stm->position = str;
  stm->length = length;

  // Create the actual lexer
  return AsmLexerCreate(&AsmStringLexStreamRead,
			&AsmStringLexStreamDelete, stm);
}
