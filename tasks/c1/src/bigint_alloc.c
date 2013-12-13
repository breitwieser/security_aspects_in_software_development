///
/// \file
/// \brief Allocation management of big integers.
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
BigInteger* BigIntAlloc(void)
{
  /// \todo Allocate a new big-integer data structure and initialize
  ///   it to represent the big-integer value zero.
  BigInteger* big = (BigInteger*)malloc(sizeof(BigInteger));
  if(big == NULL)
    return NULL;
  big->words = (mp_word_t*)malloc(sizeof(mp_word_t));
  if(big->words == NULL)
  {
    free(big);
    return NULL;
  }
  big->words[0] = 0;
  big->sign = zero;
  big->wordcount = 1;
  return big;
}

BigInteger* _BigIntAlloc(size_t count)
{
  if(count <= 0)
    return NULL;
  BigInteger* big = (BigInteger*)malloc(sizeof(BigInteger));
  if(big == NULL)
    return NULL;
  big->words = (mp_word_t*)malloc(count*sizeof(mp_word_t));
  if(big->words == NULL)
  {
    free(big);
    return NULL;
  }
  big->words[0] = 0;
  big->sign = zero;
  big->wordcount = 1;
  return big;
}

bool _BigIntNull(BigInteger *z)
{
  if(z == NULL)
    return false;
  z->words = (mp_word_t*)realloc(z->words, sizeof(mp_word_t));
  if(z->words == NULL)
    return false;
  z->wordcount = 1;
  z->sign = zero;
  return true;
}

//----------------------------------------------------------------------
void BigIntFree(BigInteger *z)
{
  /// \todo Free the big-integer structure referenced by \c z, and all
  ///  associated (memory) resources.
  if(z == NULL)
    return;
  if(z->words != NULL)
     free(z->words);
  free(z);
  z = NULL;
}

