///
/// \file
/// \brief Lexical analysis for the assembler source files.
///
#include "assembler_imp.h"
#include "grammar.h"   // Token definitions

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
struct AsmLexer {
  /// \brief Read character callback for the lexer. (required)
  ///
  /// This callback function is invoked by \ref AsmLexerNextChar to
  /// fetch the next character from the lexer's input stream.
  AsmLexerReadCallback readCallback;

  /// \brief Close callback for the lexer (optional)
  ///
  /// This (optional) callback function is invoked by
  /// \ref AsmLexerDelete when a lexer is being destroyed. It can be
  /// used to cleanup any state associated with the lexer's input
  /// stream.
  ///
  /// Note, that this callback should also be called by \ref AsmLexerCreate
  /// to dispose the state associated with the input stream, in case
  /// of an error during creation of the lexer object.
  AsmLexerDeleteCallback deleteCallback;

  /// \brief  Application specific callback data for the read
  /// and close callbacks.
  void *appData;

  /// \brief Current line number
  unsigned line;

  /// \brief Current column number.
  unsigned column;

  /// Next lookahead character (or EOF if pending).
  ///
  /// This field is used by \ref AsmLexerNextChar to
  /// implement a one-character "ungetc" facility.
  ///
  /// When fecthing the next character, the \ref AsmLexerNextChar
  /// first considers this field and tests if it contains a valid
  /// character (different to \ref ASM_TOKEN_EOF). If a valid
  /// character is found, the field is reset to \ref ASM_TOKEN_EOF
  /// and the character is returned. If no valid character is found,
  /// the \ref AsmLexerNextChar function invokes the read callback
  /// to fetch a character from the input stream.
  ///
  /// \note The lookahead field can be used to "unread" at most
  ///  one character (typically if the end of a token was found,
  ///  and the scanning function needs to "pushback" the next
  ///  character after the token, which already had been read at
  ///  this time.)
  int lookahead;

  /// \todo Add any additional fields required by your lexer here.
};

//----------------------------------------------------------------------
AsmLexer* AsmLexerCreate(AsmLexerReadCallback read_cb,
			 AsmLexerDeleteCallback delete_cb,
			 void *app_data)
{
  // Check for bad parameters (missing read callback)
  if (!read_cb) {
    if (delete_cb) {
      delete_cb(app_data);
    }

    return NULL;
  }

  // Allocate the lexer
  AsmLexer *lexer = malloc(sizeof(AsmLexer));
  if (!lexer) {
    if (delete_cb) {
      delete_cb(app_data);
    }

    return NULL;
  }

  // Remember the callbacks for later use
  lexer->readCallback = read_cb;
  lexer->deleteCallback = delete_cb;
  lexer->appData = app_data;
  lexer->lookahead = ASM_LEXER_EOF;

  lexer->line = 1;
  lexer->column = 0;

  /// \todo Initialize your lexer state
  return lexer;
}


//----------------------------------------------------------------------
void AsmLexerDelete(AsmLexer *lexer)
{
  if (lexer) {
    // Optional callback specific cleanup
    if (lexer->deleteCallback) {
      (lexer->deleteCallback)(lexer->appData);
    }

    /// \todo Perform any additional cleanup required by your lexer.
    free(lexer);
  }
}

//----------------------------------------------------------------------
/// \brief Tests if a character is a whitespace.
static bool AsmLexerIsSpace(int c)
{
  return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}


//----------------------------------------------------------------------
/// \brief Updates the line and column tracked by the lexer.
///
/// This function should be called, whenever a new character has been
/// read from the lexer's input stream. It updates the current line and
/// column position. Keeping track of the accurate line and column positions
/// in the lexer greatly simplifies debugging and allows better diagnostic
/// output in the parser.
///
/// \param[in,out] lexer the lexer to be updated.
/// \param c the character which has been read.
static void AsmLexerUpdatePosition(AsmLexer *lexer, int c)
{
  assert (c != ASM_LEXER_EOF);
  assert (c != ASM_LEXER_ERROR);

  if (c == '\n') {
    // Newline
    lexer->line += 1;
    lexer->column = 0;

  } else if (c == '\t') {
    // Treat as spaces
    lexer->column += 8;

  } else {
    // Increment the column
    lexer->column += 1;
  }
}

//----------------------------------------------------------------------
/// \brief Invokes the lexer's input read callback to fetch the next character.
///
/// This function invokes the lexer's read callback to fetch the next
/// character from the lexer's input stream.
///
/// \note There should not be any need to modify this function
///  in your own implementation. We recommend to use it as is.
static int AsmLexerNextChar(AsmLexer *lexer, bool skipWhiteSpaces)
{
  assert (lexer != NULL);
  assert (lexer->readCallback != NULL);
  assert (lexer->lookahead == ASM_LEXER_EOF ||
	  (0 <= lexer->lookahead && lexer->lookahead <= 255));

  int c;

  // Fetch the lookahead character (if any)
  if (lexer->lookahead != EOF) {
    // Fetch amd clear lookahead
    c = lexer->lookahead;
    lexer->lookahead = ASM_LEXER_EOF;

    // Only return whitespaces if desired
    if (!skipWhiteSpaces || !AsmLexerIsSpace(c)) {
      assert (0 <= c && c <= 255);
      return c;
    }
  }

  do {
    // Ask the read callback for a new character
    c = (lexer->readCallback)(lexer->appData);
    if (c == ASM_LEXER_EOF) {
      // End of file condition reported by lexer
      return c;

    } else if (c < 0 || c > 255) {
      // Invalid lexer response or lexer error...
      return ASM_LEXER_ERROR;

    }

    // Update the position
    AsmLexerUpdatePosition(lexer, c);

  } while (skipWhiteSpaces && AsmLexerIsSpace(c));

  // Return the (unsigned) character
  assert (lexer->lookahead == EOF);
  assert (0 <= c && c <= 255);
  return c;
}


//----------------------------------------------------------------------
/// \brief Skips an (optional) C++-style comment in the lexer input stream.
///
/// This helper function can be used to skip C++ style line comments
/// (starting with a "//") in the lexer input stream.
///
/// \param[in,out] lexer The lexer that should skip the comment.
///
/// \return \ref ASM_LEXER_SUCCESS on success (an optional comment
///   was skipped or no comment was found).
///
/// \return \ref ASM_LEXER_ERROR in case of an I/O error.
static inline int AsmLexerSkipLineComment(AsmLexer *lexer)
{
  /// \todo Prepare your lexer for scanning over comments or whitespaces
  ///  (depending on your implementation, you may need to clear temporary buffers,
  ///   or do other housekeeping)

  // Scan second character of line comment
  int c = AsmLexerNextChar(lexer, false);
  if (c != '/') {
    // Unrecognized comment (does not start with '//')
    // (also cover c==ASM_LEXER_ERROR/EOF)
    return ASM_LEXER_ERROR;
  }


  while (c != ASM_LEXER_EOF && c != '\n') {
    // Scan next character
    c = AsmLexerNextChar(lexer, false);
    if (c == ASM_LEXER_ERROR) {
      return ASM_LEXER_ERROR; // Lexer error
    }
  }

  // Setup the lookahead
  lexer->lookahead = c;

  return ASM_LEXER_SUCCESS;
}


int isInteger(int c)
{
  return (c >= '0' && c <= '9') || c == '-';
}

int isDigit(int c)
{
  return (c >= '0' && c <= '9');
}

int isHex(int c)
{
  return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') ||(c >= '0' && c <= '9');
}

int isOct(int c)
{
  return (c >= '0' && c <= '7');
}

bool isLetter(int c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isLetterUnderscore(int c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isSpecial(int c)
{
  return (c == '[') || (c == ']') || (c == '(') || (c == ')') || (c == ',');
}

bool isEOF(int c)
{
  return c == ASM_LEXER_EOF;
}

int upper(int c)
{
  if(c >= 'a' && c <= 'f')
     return ('A' + c - 'a');
  if(c == 'X')
    return 'x';
  return c;
}

/// Tokens to be recognized:
/// * Identifiers and keywords starting with a letter or underscore
///   (A-Z,a-z,_) followed by zero or more letters, digits or underscores.
///
///   Use the AsmKeywordLookup function to distinguish between identifier and
///   normal keywords.
///
///   For keywords, the lookup function will return an AsmKeywordInfo object
///   with the correct token code for the keyowrd, for identifiers the lookup
///   function will return NULL. (The token code for identifiers is ASM_TOKEN_IDENTIFIER)
int AsmScanIdentifier(int first, AsmToken **ptoken, AsmLexer *lexer)
{
  int c = first;
  AsmBuffer* buffer = (AsmBuffer*)malloc(sizeof(AsmBuffer));
  AsmBufferInit(buffer, false);

  do
  {
    AsmBufferAppendByte(buffer, c);
    c = AsmLexerNextChar(lexer, false);
    if(c == ASM_LEXER_ERROR)
    {
        AsmBufferClear(buffer);
        free(buffer);
        buffer = NULL;
        return ASM_LEXER_ERROR;
    }
  } while(isLetterUnderscore(c) || isDigit(c));

  const AsmKeywordInfo* info = AsmKeywordLookup(AsmBufferBytes(buffer), AsmBufferLength(buffer));
  int type = ASM_TOKEN_IDENTIFIER;
  if(info != NULL) // keyword
    type = info->tokenType;
  AsmToken* tok = AsmTokenNew(type, lexer->line, lexer->column, AsmBufferBytes(buffer), AsmBufferLength(buffer));
  AsmBufferClear(buffer);
  free(buffer);
  buffer = NULL;
  if(tok == NULL)
    return ASM_LEXER_ERROR;
  *ptoken = tok;
  lexer->lookahead = c;
  return ASM_LEXER_SUCCESS;
}

///
/// * Local variable names starting with an "@" followed by one or more
///   letters, digits or underscores.
///
///   The name following an "@" is not scanned for keywords. (for example,
///   '@data' would be a valid local name)
///
///   The token code for local names is ASM_TOKEN_LOCAL_NAME
int AsmScanLocalVariable(AsmToken **ptoken, AsmLexer *lexer)
{
  int c = '@';
  AsmBuffer* buffer = (AsmBuffer*)malloc(sizeof(AsmBuffer));
  AsmBufferInit(buffer, false);
  do
  {
      AsmBufferAppendByte(buffer, c);
    c = AsmLexerNextChar(lexer, false);
    if(c == ASM_LEXER_ERROR)
    {
        AsmBufferClear(buffer);
        free(buffer);
        buffer = NULL;
        return ASM_LEXER_ERROR;
    }
  } while(isLetterUnderscore(c) || isDigit(c));

  AsmToken* tok = AsmTokenNew(ASM_TOKEN_LOCAL_NAME, lexer->line, lexer->column, AsmBufferBytes(buffer), AsmBufferLength(buffer));
  AsmBufferClear(buffer);
  free(buffer);
  buffer = NULL;
  if(tok == NULL)
    return ASM_LEXER_ERROR;
  *ptoken = tok;
  lexer->lookahead = c;
  return ASM_LEXER_SUCCESS;
}

///
/// * Special keywords starting with a "." followed by a non-empty sequence
///   of letters.
///
///   You can use the AsmKeywordLookup function to find the token code
///   for special keywords (don't forget to pass the leading '.'). For valid
///   special keywords the lookup function will return an AsmKewordInfo structure
///   for invalid keywords NULL is returned.
int AsmScanKeyword(AsmToken **ptoken, AsmLexer *lexer)
{
  int c = '.';
  AsmBuffer* buffer = (AsmBuffer*)malloc(sizeof(AsmBuffer));
  AsmBufferInit(buffer, false);
  do
  {
    AsmBufferAppendByte(buffer, c);
    c = AsmLexerNextChar(lexer, false);
    if(c == ASM_LEXER_ERROR)
    {
        AsmBufferClear(buffer);
        free(buffer);
        buffer = NULL;
        return ASM_LEXER_ERROR;
    }
  } while(isLetter(c));

  const AsmKeywordInfo* info = AsmKeywordLookup(AsmBufferBytes(buffer), AsmBufferLength(buffer));
  if(info == NULL) // invalid keyword
  {
      AsmBufferClear(buffer);
      free(buffer);
      buffer = NULL;
      return ASM_LEXER_ERROR;
  }
  AsmToken* tok = AsmTokenNew(info->tokenType, lexer->line, lexer->column, AsmBufferBytes(buffer), AsmBufferLength(buffer));
  AsmBufferClear(buffer);
  free(buffer);
  buffer = NULL;
  if(tok == NULL)
    return ASM_LEXER_ERROR;
  *ptoken = tok;
  lexer->lookahead = c;
  return ASM_LEXER_SUCCESS;
}

///
/// * Decimal, hexadecimal and octal integers (token code: ASM_TOKEN_INT)
///   Any integer may start with a leading '-' sign.
///
///   + Decimal integers consist of a leading non-zero decimal digit (1-9) followed
///     by an optional sequence of arbitrary decimal digits (0-9).
///
///   + Hexadecimal integers start with a leading "0x" or "0X" prefix followed
///     by a non-empty sequence of hexadecimal digits (0-9, A-F, a-f)
///
///   + Octal digits start with a leading zero (0) followed by an optional sequence
///     of arbitrary octal digits (0-7)
///
///   Your scanner should return the parsed integer "string" (including the
///   leading '-' if any) as token text. (Actual integer parsing of the text
///   into an int64_t value is handled by the AsmUtilParseInteger and should NOT
///   be implemented here)
///
///   The token code for integers is ASM_TOKEN_INT
int AsmScanInteger(int first, AsmToken **ptoken, AsmLexer *lexer)
{
  int c = first;
  bool neg = false;
  AsmBuffer* buffer = (AsmBuffer*)malloc(sizeof(AsmBuffer));
  AsmBufferInit(buffer, false);
  if(c == '-') // parse -
  {
      neg = true;
      AsmBufferAppendByte(buffer, c);
      c = AsmLexerNextChar(lexer, false);
      if(c == ASM_LEXER_ERROR)
        {
            AsmBufferClear(buffer);
            free(buffer);
            buffer = NULL;
            return ASM_LEXER_ERROR;
        }
  }
  if(c == '0') // Hex or Oct
  {
      AsmBufferAppendByte(buffer, c);
      c = AsmLexerNextChar(lexer, false);
      if(c == ASM_LEXER_ERROR)
        {
            AsmBufferClear(buffer);
            free(buffer);
            buffer = NULL;
            return ASM_LEXER_ERROR;
        }
      if(c == 'x' || c == 'X') // hex
      {
          int hcount = 0;
          do
          {
            AsmBufferAppendByte(buffer, upper(c));
            hcount++;
            c = AsmLexerNextChar(lexer, false);
            if(c == ASM_LEXER_ERROR)
            {
                  AsmBufferClear(buffer);
                  free(buffer);
                  buffer = NULL;
                  return ASM_LEXER_ERROR;
            }
          } while(isHex(c));
          if(hcount < 2)
          {
              AsmBufferClear(buffer);
              free(buffer);
              buffer = NULL;
              return ASM_LEXER_ERROR;
          }
      }
      else if(isOct(c)) // oct
      {
          do
          {
            AsmBufferAppendByte(buffer, c);
            c = AsmLexerNextChar(lexer, false);
            if(c == ASM_LEXER_ERROR)
              {
                  AsmBufferClear(buffer);
                  free(buffer);
                  buffer = NULL;
                  return ASM_LEXER_ERROR;
              }
          } while(isOct(c));
      }
      // 0 is a valid oct number
      if(! (isEOF(c) || AsmLexerIsSpace(c) || isSpecial(c))) // error
      {
            AsmBufferClear(buffer);
            free(buffer);
            buffer = NULL;
            return ASM_LEXER_ERROR;
      }
  }
  else if (isDigit(c)) // Decimal
  {
      do
      {
        AsmBufferAppendByte(buffer, c);
        c = AsmLexerNextChar(lexer, false);
        if(c == ASM_LEXER_ERROR)
          {
              AsmBufferClear(buffer);
              free(buffer);
              buffer = NULL;
              return ASM_LEXER_ERROR;
          }
      } while(isDigit(c));
      if(! (isEOF(c) || AsmLexerIsSpace(c) || isSpecial(c))) // error
      {
            AsmBufferClear(buffer);
            free(buffer);
            buffer = NULL;
            return ASM_LEXER_ERROR;
      }
  }
  else if(neg) // minus without number
  {
      AsmBufferClear(buffer);
      free(buffer);
      buffer = NULL;
      return ASM_LEXER_ERROR;
  }

  AsmToken* tok = AsmTokenNew(ASM_TOKEN_INT, lexer->line, lexer->column, AsmBufferBytes(buffer), AsmBufferLength(buffer));
  AsmBufferClear(buffer);
  free(buffer);
  buffer = NULL;
  if(tok == NULL)
    return ASM_LEXER_ERROR;
  *ptoken = tok;
  lexer->lookahead = c;
  return ASM_LEXER_SUCCESS;
}

int AsmScanSingle(int type, const char* name, AsmToken **ptoken, AsmLexer *lexer)
{
  AsmToken* tok = AsmTokenNew(type, lexer->line, lexer->column, name, 1);
  if(tok == NULL)
    return ASM_LEXER_ERROR;
  *ptoken = tok;
  return ASM_LEXER_SUCCESS;
}

//----------------------------------------------------------------------
int AsmLexerScan(AsmToken **ptoken, AsmLexer *lexer)
{
  if (ptoken) {
    *ptoken = NULL;
  }

  if (!ptoken || !lexer) {
    return ASM_LEXER_ERROR; // Bad arguments
  }

  int first;

  // Prepare for scanning the next token. We first skip all line-comments
  // and whitespaces until we reach an interesting character.
  do {
    // Read the next non-whitespace character
    first = AsmLexerNextChar(lexer, true);
    if (first == ASM_LEXER_ERROR || first == ASM_LEXER_EOF) {
      return first; // Error or end of file
    }

    assert (0 <= first && first <= 255);

    // Skip if line comment stop else
    if (first == '/') {
      int ret = AsmLexerSkipLineComment(lexer);
      if (ret == ASM_LEXER_ERROR) {
	return ret; // Error while parsing line comment
      }

      // Failed to parse the comment
      assert (ret == ASM_LEXER_SUCCESS);
    }
  } while (first == '/');

  /// \todo Add your lexical scanner implementation here. It is advisable
  ///  to split handling of different types of tokens (integers, identifiers)
  ///  into separate functions, and to call these functions (depending
  ///  on the first character) from here.
  ///
  /// In case of a lexer error (unrecognized character, I/O error, ...) your
  /// implementation should return ASM_LEXER_ERROR.
  ///
  /// If a valid token has been scanned, your implementation should return
  /// a new token object (created witth AsmTokenNew) via *ptoken,
  ///
  /// If the end of file has been reached without scanning a token, your
  /// implementation should return ASM_LEXER_EOF.

  /// Tokens to be recognized:
  /// * Identifiers and keywords starting with a letter or underscore
  ///   (A-Z,a-z,_) followed by zero or more letters, digits or underscores.
  ///
  ///   Use the AsmKeywordLookup function to distinguish between identifier and
  ///   normal keywords.
  ///
  ///   For keywords, the lookup function will return an AsmKeywordInfo object
  ///   with the correct token code for the keyowrd, for identifiers the lookup
  ///   function will return NULL. (The token code for identifiers is ASM_TOKEN_IDENTIFIER)

  if(isLetterUnderscore(first))
  {
    return AsmScanIdentifier(first, ptoken, lexer);
  }
  ///
  /// * Local variable names starting with an "@" followed by one or more
  ///   letters, digits or underscores.
  ///
  ///   The name following an "@" is not scanned for keywords. (for example,
  ///   '@data' would be a valid local name)
  ///
  ///   The token code for local names is ASM_TOKEN_LOCAL_NAME

  if(first == '@')
  {
    return AsmScanLocalVariable(ptoken, lexer);
  }
  ///
  ///
  /// * Special keywords starting with a "." followed by a non-empty sequence
  ///   of letters.
  ///
  ///   You can use the AsmKeywordLookup function to find the token code
  ///   for special keywords (don't forget to pass the leading '.'). For valid
  ///   special keywords the lookup function will return an AsmKewordInfo structure
  ///   for invalid keywords NULL is returned.

  if(first == '.')
  {
    return AsmScanKeyword(ptoken, lexer);
  }
  ///
  /// * Decimal, hexadecimal and octal integers (token code: ASM_TOKEN_INT)
  ///   Any integer may start with a leading '-' sign.
  ///
  ///   + Decimal integers consist of a leading non-zero decimal digit (1-9) followed
  ///     by an optional sequence of arbitrary decimal digits (0-9).
  ///
  ///   + Hexadecimal integers start with a leading "0x" or "0X" prefix followed
  ///     by a non-empty sequence of hexadecimal digits (0-9, A-F, a-f)
  ///
  ///   + Octal digits start with a leading zero (0) followed by an optional sequence
  ///     of arbitrary octal digits (0-7)
  ///
  ///   Your scanner should return the parsed integer "string" (including the
  ///   leading '-' if any) as token text. (Actual integer parsing of the text
  ///   into an int64_t value is handled by the AsmUtilParseInteger and should NOT
  ///   be implemented here)
  ///
  ///   The token code for integers is ASM_TOKEN_INT

  if(isInteger(first))
  {
    return AsmScanInteger(first, ptoken, lexer);
  }
  ///
  /// * Single character tokens
  ///   '[' - Left opening bracket (ASM_TOKEN_LBRACK)
  ///   ']' - Right closing bracket (ASM_TOKEN_RBRACK)
  ///   '(' - Left opening parenthesis (ASM_TOKEN_LPAREN)
  ///   ')' - Right closing parenthesis (ASM_TOKEN_RPAREN)
  ///   ',' - Comma (ASM_TOKEN_COMMA)
  ///

  switch(first)
  {
    case '[':
      return AsmScanSingle(ASM_TOKEN_LBRACK, "[", ptoken, lexer);
    case ']':
      return AsmScanSingle(ASM_TOKEN_RBRACK, "]", ptoken, lexer);
    case '(':
      return AsmScanSingle(ASM_TOKEN_LPAREN, "(", ptoken, lexer);
    case ')':
      return AsmScanSingle(ASM_TOKEN_RPAREN, ")", ptoken, lexer);
    case ',':
      return AsmScanSingle(ASM_TOKEN_COMMA, ",", ptoken, lexer);
    case ':':
      return AsmScanSingle(ASM_TOKEN_DOUBLECOLON, ":", ptoken, lexer);
  }

  return ASM_LEXER_ERROR;
}

//----------------------------------------------------------------------
unsigned AsmLexerGetLine(AsmLexer *lexer)
{
  if (lexer == NULL) {
    return 0;
  }

  return lexer->line;
}

//----------------------------------------------------------------------
unsigned AsmLexerGetColumn(AsmLexer *lexer)
{
  if (lexer == NULL) {
    return 0;
  }

  return lexer->column;
}
