///
/// \file
/// \brief Lexical token handling
///
#include "assembler_imp.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
struct AsmToken
{
  /// \todo Add the required fields of your token structure.
  ///
  /// A token needs to store its token code and its textual representation.
  /// To get usable diagnostic output (e.g. for parser errors) you also
  /// need to store the line and column number of the token.
};

//----------------------------------------------------------------------
AsmToken* AsmTokenNew(int type, unsigned line, unsigned column,
		      const char *text, size_t text_len)
{
  /// \todo Add your own implementation.
  return NULL;
}

//----------------------------------------------------------------------
void AsmTokenDelete(AsmToken *token)
{
  /// \todo Add your own implementation.
}

//----------------------------------------------------------------------
int AsmTokenGetType(const AsmToken *token)
{
  /// \todo Add your own implementation.
  return 0;
}

//----------------------------------------------------------------------
const char* AsmTokenGetText(const AsmToken *token)
{
  /// \todo Add your own implementation.
  return NULL;
}

//----------------------------------------------------------------------
unsigned AsmTokenGetLine(const AsmToken *token)
{
  /// \todo Add your own implementation.
  return 0; // No information available
}

//----------------------------------------------------------------------
unsigned AsmTokenGetColumn(const AsmToken *token)
{
  /// \todo Add your own implementation.
  return 0; // No information available
}
