///
/// \file
/// \brief Assembler object handling
///
#include "assembler_imp.h"
#include "grammar.h"

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------
struct AsmObject
{
  /// Link to the next object
  AsmObject *next;

  /// Qualifiers of this object
  VmQualifiers qualifiers;

  /// Content of this object
  AsmCodeBuffer *content;

  /// The source token declaring this object
  AsmToken *token;

  /// Handle value of this object.
  ///
  /// This field is zero if the object has not been assigned a
  /// handle value yet.
  uint32_t handle;
};

//----------------------------------------------------------------------
AsmObject* AsmObjectNew(AsmToken *token, VmQualifiers qualifiers,
			AsmCodeBuffer *content)
{
  // Check the token type (and implicitly check if the token is NULL)
  if (AsmTokenGetType(token) != ASM_TOKEN_IDENTIFIER) {
    return NULL; // Bad arguments
  }

  if (!content) {
    return NULL; // Bad arguments
  }

  // Allocate the object
  AsmObject *object = malloc(sizeof(AsmObject));
  if (!object) {
    return NULL; // Out of memory
  }

  object->next = NULL;
  object->qualifiers = qualifiers;
  object->token = token;
  object->content = content;
  object->handle = 0;

  return object;
}

//----------------------------------------------------------------------
void AsmObjectDelete(AsmObject *object)
{
  if (object) {
    AsmCodeDelete(object->content);
    AsmTokenDelete(object->token);
    memset(object, 0, sizeof(AsmObject));
    free(object);
  }
}

//----------------------------------------------------------------------
void AsmObjectsDeleteChain(AsmObject *first)
{
  if (first) {
    while (first != NULL) {
      AsmObject *next = first->next;
      AsmObjectDelete(first);
      first = next;
    }
  }
}

//----------------------------------------------------------------------
AsmCodeBuffer* AsmObjectGetContent(const AsmObject *object)
{
  if (!object) {
    return NULL;
  }

  return object->content;
}

//----------------------------------------------------------------------
uint32_t AsmObjectGetHandle(const AsmObject *object)
{
  if (!object) {
    return 0;
  }

  return object->handle;
}

//----------------------------------------------------------------------
void AsmObjectSetHandle(AsmObject *object, uint32_t handle)
{
  if (object) {
    object->handle = handle;
  }
}

//----------------------------------------------------------------------
VmQualifiers AsmObjectGetQualifiers(const AsmObject *object)
{
  if (!object) {
    return VM_QUALIFIER_NONE;
  }

  return object->qualifiers;
}

//----------------------------------------------------------------------
const AsmToken* AsmObjectGetToken(const AsmObject *object)
{
  if (!object) {
    return NULL;
  }

  return object->token;
}

//----------------------------------------------------------------------
AsmObject* AsmObjectGetNext(const AsmObject *object)
{
  if (!object) {
    return NULL;
  }

  return object->next;
}

//----------------------------------------------------------------------
bool AsmObjectWrite(const AsmObject *object, AsmObjectWriteCallback write_cb,
		    void *cb_data)
{
  if (!object || !write_cb) {
    return false;
  }

  // Write the object content
  AsmBuffer *bytes = AsmCodeGetBytes(object->content);
  size_t content_length = AsmBufferLength(bytes);

  // Write the object header
  uint32_t qualifiers = (uint32_t) object->qualifiers;
  bool is_bigendian = AsmCodeIsBigEndian(object->content);

  uint8_t header[] = {
    ASM_INIT_WORD(VM_TAG_OBJECT, is_bigendian),   // Magic bytes (object tag)
    ASM_INIT_WORD(qualifiers, is_bigendian),      // Qualifiers
    ASM_INIT_WORD(object->handle, is_bigendian),  // Handle
    ASM_INIT_WORD(content_length, is_bigendian),  // Length
  };

  if (!write_cb(cb_data, header, sizeof(header))) {
    return false;
  }

  // Write the object content (if not empty)
  if (content_length > 0) {
    const uint8_t *content_data = AsmBufferBytes(bytes);
    assert (content_data != NULL);
    if (!write_cb(cb_data, content_data, content_length)) {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------
AsmObject* AsmObjectsJoin(AsmObject *head, AsmObject *tail)
{
  if (!head) {
    // Trivial case: Join result is tail chain
    return tail;

  } else if (!tail) {
    // Trivial case: Join result is head chain
    return head;

  } else {
    // Find the end of the head chain
    AsmObject *current = head;
    AsmObject *last = head;

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
AsmObject* AsmObjectsLookup(AsmObject *head, const char *name)
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
