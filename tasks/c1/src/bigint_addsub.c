///
/// \file
/// \brief Big integer addition and subtraction.
///
#include "tinybn_imp.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

//----------------------------------------------------------------------
bool BigIntAdd(BigInteger *z, const BigInteger *a, const BigInteger *b)
{
  /// \todo Implement the big-integer addition function. Use your MpAdd
  ///   and MpSub functions for the low-level arithmetic.
  if(z == NULL || b == NULL || a == NULL)
    return false;
  if(z->words == NULL || a->words == NULL || b->words == NULL)
    return false;
  if(a->sign == zero)
    return BigIntCopy(z,b);
  if(b->sign == zero)
    return BigIntCopy(z,a);

  if(a->sign == b->sign) // +a + +b || -a + -b
  {
      BigInteger* res = BigIntAlloc();
      if(res == NULL)
        return false;
      int wordcount = MAX(a->wordcount, b->wordcount);
      if(!BigIntSetAt(res, wordcount, 0)) // Allocates wordcount+1 space
      {
          BigIntFree(res);
          return false;
      }
      if(!MpAdd(res->words, a->words, a->wordcount, b->words, b->wordcount))
      {
          BigIntFree(res);
          return false;
      }
      for(int i = wordcount; i >= 0; i--) // truncate leading zeros
      {
        if(res->words[i] != 0)
        {
          wordcount = i;
          break;
        }
      }
      res->wordcount = wordcount;
      res->sign = a->sign;
      bool result = BigIntCopy(z, res);
      BigIntFree(res);
      return result;
  }
  else if(a->sign > b->sign) // +a + -b
  {
    BigInteger* res = BigIntAlloc();
    if(res == NULL)
      return false;
    if(!BigIntNeg(res, b))
    {
        BigIntFree(res);
        return false;
    }
    bool result = BigIntSub(z, a, b); // +a - +b
    BigIntFree(res);
    return result;
  }
  else if(a->sign < b->sign) // -a + +b
  {
    BigInteger* res = BigIntAlloc();
    if(res == NULL)
      return false;
    if(!BigIntNeg(res, a))
    {
        BigIntFree(res);
        return false;
    }
    bool result = BigIntSub(z, b, a); // +b - +a
    BigIntFree(res);
    return result;
  }

  return false; // Not yet implemented
}

//----------------------------------------------------------------------
bool BigIntSub(BigInteger *z, const BigInteger *a, const BigInteger *b)
{
  /// \todo Implement the big-integer addition function. Use your MpAdd
  ///   and MpSub functions for the low-level arithmetic.
  if(z == NULL || b == NULL || a == NULL)
    return false;
  if(z->words == NULL || a->words == NULL || b->words == NULL)
    return false;
  if(a->sign == zero)
    return BigIntCopy(z,b);
  if(b->sign == zero)
    return BigIntCopy(z,a);
  if(a->sign == b->sign) // +a - +b || -a - -b
  {
      int cmp = BigIntCompare(a, b);
      BigInteger* res = BigIntAlloc();
      if(res == NULL)
        return false;
      if(cmp != 0)
      {
        int wordcount = MAX(a->wordcount, b->wordcount);
        if(!BigIntSetAt(res, wordcount-1, 0)) // Allocates wordcount space
        {
            BigIntFree(res);
            return false;
        }
        if(!MpSub(res->words, a->words, a->wordcount, b->words, b->wordcount))
        {
            BigIntFree(res);
            return false;
        }
        for(int i = wordcount; i >= 0; i--) // truncate leading zeros
        {
          if(res->words[i] != 0)
          {
            wordcount = i;
            break;
          }
        }
        res->wordcount = wordcount;
        /* +a - +b
         *  a < b => +*- => - (eg. 3-5)
         *  a > b => +*+ => + (eg. 5-3)
         * -a - -b
         *  a < b => -*- => + (eg. -3--5)
         *  a > b => -*+ => - (eg. -5--3)
         */
        res->sign = a->sign * cmp;
      }
      bool result = BigIntCopy(z, res);
      BigIntFree(res);
      return result;
  }
  else if(a->sign > b->sign) // +a - -b
  {
    BigInteger* res = BigIntAlloc();
    if(res == NULL)
      return false;
    if(!BigIntNeg(res, b))
    {
        BigIntFree(res);
        return false;
    }
    bool result = BigIntAdd(z, a, b); // +a + +b
    BigIntFree(res);
    return result;
  }
  else if(a->sign < b->sign) // -a - +b
  {
    BigInteger* res = BigIntAlloc();
    if(res == NULL)
      return false;
    if(!BigIntNeg(res, b))
    {
        BigIntFree(res);
        return false;
    }
    bool result = BigIntAdd(z, b, a); // -a + -b
    BigIntFree(res);
    return result;
  }
  return false; // Not yet implemented
}
