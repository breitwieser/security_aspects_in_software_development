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
  BigInteger* big = malloc(sizeof(BigInteger));
  if(big == NULL)
    return NULL;
  big->words = malloc(sizeof(mp_word_t));
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

