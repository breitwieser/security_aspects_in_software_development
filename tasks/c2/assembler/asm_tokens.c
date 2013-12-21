///
/// \file
/// \brief Lexical token handling
///
#include "assembler_imp.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
struct AsmToken {
	/// \todo Add the required fields of your token structure.
	///
	/// A token needs to store its token code and its textual representation.
	/// To get usable diagnostic output (e.g. for parser errors) you also
	/// need to store the line and column number of the token.
	int type;
	char *data;
	size_t len;
	unsigned line;
	unsigned column;
};

//----------------------------------------------------------------------
AsmToken* AsmTokenNew(int type, unsigned line, unsigned column,
		const char *text, size_t text_len) {
	/// \todo Add your own implementation.
	if (text_len != 0 && text == NULL)
		return NULL;

	AsmToken *token = (AsmToken*) malloc(sizeof(AsmToken));
	if (token == NULL) {
		return NULL;
	}

	token->line = line;
	token->column = column;
	token->type = type;

	//integer overflow
	if(text_len + 1 < text_len){
		free(token);
		return NULL;
	}
	token->data = (char*) malloc(text_len + 1);
	if (token->data == NULL) {
		free(token);
		return NULL;
	}
	if(text_len != 0){
		memcpy(token->data, text, text_len);
	}
	token->data[text_len] = '\0';

	return token;
}

//----------------------------------------------------------------------
void AsmTokenDelete(AsmToken *token) {
	/// \todo Add your own implementation.
	if (token == NULL)
		return;
	if (token->data != NULL) {
		free(token->data);
	}
	free(token);
}

//----------------------------------------------------------------------
int AsmTokenGetType(const AsmToken *token) {
	/// \todo Add your own implementation.
	if (token == NULL)
		return 0;
	return token->type;
}

//----------------------------------------------------------------------
const char* AsmTokenGetText(const AsmToken *token) {
	/// \todo Add your own implementation.
	if (token == NULL)
		return NULL;
	return token->data;
}

//----------------------------------------------------------------------
unsigned AsmTokenGetLine(const AsmToken *token) {
	/// \todo Add your own implementation.
	if (token == NULL)
		return 0;
	return token->line;
}

//----------------------------------------------------------------------
unsigned AsmTokenGetColumn(const AsmToken *token) {
	/// \todo Add your own implementation.
	if (token == NULL)
		return 0;
	return token->column;
}
