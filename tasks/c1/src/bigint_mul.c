///
/// \file
/// \brief Big integer addition and subtraction.
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
bool BigIntMul(BigInteger *z, const BigInteger *a, const BigInteger *b)
{
  /// \todo Impement multiplication between a and b. Use your
  /// MpMul function, to perform the low-level multiplication.
  /// Note that may have to allocate a temporary buffer for the
  /// multiplication result, since \c z can refer to the same
  /// big-integers as \¢ a and/or \c b,
  if(z == NULL || b == NULL || a == NULL)
    return false;
  int wordcount = a->wordcount+b->wordcount;
  BigInteger* res = _BigIntAlloc(wordcount); // allocate enough space
  if(res == NULL)
    return false;
  MpMul(res->words, a->words, a->wordcount, b->words, b->wordcount);
  for(int i = wordcount; i > 0; i--) // truncate leading zeros
  {
    if(res->words[i-1] != 0)
    {
      wordcount = i;
      break;
    }
    else if(i == 1)
      wordcount = 1;
  }
  res->wordcount = MAX(wordcount, 1);
  res->sign = a->sign * b->sign;
  bool result = BigIntCopy(z, res);
  BigIntFree(res);
  return result;
}
