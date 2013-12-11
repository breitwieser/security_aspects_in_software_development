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
  BigInteger* res = BigIntAlloc();
  if(res == NULL)
    return false;
  if(!BigIntSetAt(res, a->wordcount+b->wordcount, 0)) // allocate enough space
  {
      BigIntFree(res);
      return false;
  }
  MpMul(res->words, a->words, a->wordcount, b->words, b->wordcount);
  int wordcount = res->wordcount;
  for(int i = wordcount-1; i >= 0; i--) // truncate leading zeros
  {
    if(res->words[i] != 0)
    {
      wordcount = i+1;
      break;
    }
  }
  res->wordcount = wordcount;
  res->sign = a->sign * b->sign;
  bool result = BigIntCopy(z, res);
  BigIntFree(res);
  return result;
}
