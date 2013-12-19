///
/// \file
/// \brief Error logging
///
#include "assembler_imp.h"

#include <stdio.h>

//----------------------------------------------------------------------
static void AsmReportStatusV(const char *prefix, Assembler *assembler,
			     const AsmToken *token, const char *message,
			     va_list ap)
{
  unsigned line = 0;
  unsigned column = 0;

  // Extract source line information from token (if known)
  if (token) {
    line = AsmTokenGetLine(token);
    column = AsmTokenGetColumn(token);
  }

  // Print the error message prefix (add source line if available)
  if (line != 0) {
    fprintf(stderr, "%5s (line %3u, column %2u): ",
	    prefix, line, column);
  } else {
    fprintf(stderr, "%5s: ", prefix);
  }

  // Print the user error message
  vfprintf(stderr, message, ap);

  // And the newline
  fputc('\n', stderr);
}

//----------------------------------------------------------------------
bool AsmIsReportEnabled(Assembler *assembler, AsmVerbosityLevel level)
{
  if (!assembler) {
    // No assembler given, assume enabled
    return true;
  }

  return assembler->verbosityLevel >= level;
}

//----------------------------------------------------------------------
void AsmError(Assembler *assembler, const AsmToken *token,
	      const char *message,  ...)
{
  if (AsmIsReportEnabled(assembler, ASM_VERBOSITY_ERROR)) {
    va_list ap;

    va_start(ap, message);
    AsmReportStatusV("error", assembler, token, message, ap);
    va_end(ap);
  }

  // Increment the assembler's error counter
  // (if an assembler object is given)
  if (assembler) {
    assembler->errors += 1;
  }
}

//----------------------------------------------------------------------
void AsmInfo(Assembler *assembler, const AsmToken *token,
	     const char *message,  ...)
{
  if (AsmIsReportEnabled(assembler, ASM_VERBOSITY_INFO)) {
    va_list ap;

    va_start(ap, message);
    AsmReportStatusV("info", assembler, token, message, ap);
    va_end(ap);
  }
}

//----------------------------------------------------------------------
unsigned AsmGetErrorCount(Assembler *assembler)
{
  if (!assembler) {
    return 0; // No assembler object (assume no errors)
  }

  return assembler->errors;
}
