///
/// \file
/// \brief Variable list handling
///
#include "assembler_imp.h"
#include "grammar.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
struct AsmVariable {
  /// Link to the next variable
  AsmVariable *next;

  /// Index of this variable
  uint8_t index;

  /// The source token declaring this variable
  AsmToken *token;
};

//----------------------------------------------------------------------
AsmVariable* AsmVariableNew(AsmToken *token)
{
  // Check the token type (and implicitly check if the token is NULL)
  if (AsmTokenGetType(token) != ASM_TOKEN_LOCAL_NAME) {
    return NULL; // Bad arguments
  }

  // Allocate and initialize
  AsmVariable *variable = malloc(sizeof(AsmVariable));
  if (!variable) {
    return NULL; // Out of memory
  }

  variable->token = token;
  variable->next = NULL;
  variable->index = 0;
  return variable;
}

//----------------------------------------------------------------------
void AsmVariablesDeleteChain(AsmVariable *first)
{
  if (first) {
    while (first != NULL) {
      AsmVariable *next = first->next;
      AsmVariableDelete(first);
      first = next;
    }
  }
}


//----------------------------------------------------------------------
void AsmVariableDelete(AsmVariable *variable)
{
  if (variable) {
    AsmTokenDelete(variable->token);

    memset(variable, 0, sizeof(AsmVariable));
    free(variable);
  }
}

//----------------------------------------------------------------------
const char *AsmVariableGetName(const AsmVariable *variable)
{
  return AsmTokenGetText(AsmVariableGetToken(variable));
}

//----------------------------------------------------------------------
const AsmToken* AsmVariableGetToken(const AsmVariable *variable)
{
  if (variable == NULL) {
    return NULL;
  }

  return variable->token;
}

//----------------------------------------------------------------------
AsmVariable* AsmVariableGetNext(AsmVariable *variable)
{
  if (!variable) {
    return NULL;
  }

  return variable->next;
}

//----------------------------------------------------------------------
AsmVariable* AsmVariablesJoin(AsmVariable *head, AsmVariable *tail)
{
  if (!head) {
    // Trivial case: Join result is tail chain
    return tail;

  } else if (!tail) {
    // Trivial case: Join result is head chain
    return head;

  } else {
    // Find the end of the head chain
    AsmVariable *current = head;
    AsmVariable *last = head;

    while (current != NULL) {
      last = current;
      current = current->next;
    }

    assert (last != NULL);
    assert (last->next == NULL);

    // Append the tail
    last->next = tail;
    return head;
  }
}

//----------------------------------------------------------------------
AsmVariable* AsmVariablesSplit(AsmVariable *head)
{
  if (!head) {
    // Trivial case: Split of NULL head
    return NULL;
  }

  // Split the head of the chain
  AsmVariable *next = head->next;
  head->next = NULL;
  return next;
}


//----------------------------------------------------------------------
AsmVariable* AsmVariablesLookup(AsmVariable *head, const char *name)
{
  if (!name) {
    return NULL;
  }

  while (head != NULL) {
    const char *varname = AsmTokenGetText(head->token);
    assert (varname != NULL);

    if (strcmp(varname, name) == 0) {
      return head;
    }

    head = head->next;
  }

  return NULL;
}

//----------------------------------------------------------------------
uint8_t AsmVariableGetIndex(const AsmVariable *variable)
{
  if (!variable) {
    return 0;
  }

  return variable->index;
}

//----------------------------------------------------------------------
void AsmVariableSetIndex(AsmVariable* variable, uint8_t index)
{
  if (variable) {
    variable->index = index;
  }
}

