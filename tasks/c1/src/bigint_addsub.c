///
/// \file
/// \brief Big integer addition and subtraction.
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
bool BigIntAdd(BigInteger *z, const BigInteger *a, const BigInteger *b)
{
  /// \todo Implement the big-integer addition function. Use your MpAdd
  ///   and MpSub functions for the low-level arithmetic.
  if(z == NULL || b == NULL || a == NULL)
    return false;
  if(a->sign == zero)
    return BigIntCopy(z,b);
  if(b->sign == zero)
    return BigIntCopy(z,a);

  if(a->sign == b->sign) // +a + +b || -a + -b
  {
      size_t wordcount = MAX(a->wordcount, b->wordcount);
      BigInteger* res = _BigIntAlloc(wordcount);
      if(res == NULL)
        return false;
      if(MpAdd(res->words, a->words, a->wordcount, b->words, b->wordcount)) // carry
      {
          wordcount++;
          if(!_BigIntResize(res, wordcount))
          {
              BigIntFree(res);
              return false;
          }
          res->words[wordcount-1] = 1;
      }
      for(int i = wordcount; i > 0; i--) // truncate leading zeros
      {
        if(res->words[i-1] != 0)
        {
          wordcount = i;
          break;
        }
//        Can never happen
//        else if(i == 1)
//          wordcount = 1;
      }
      res->wordcount = MAX(wordcount,1);
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
    bool result = BigIntSub(z, a, res); // +a - +b
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
    bool result = BigIntSub(z, b, res); // +b - +a
    BigIntFree(res);
    return result;
  }

  return false;
}

//----------------------------------------------------------------------
bool BigIntSub(BigInteger *z, const BigInteger *a, const BigInteger *b)
{
  /// \todo Implement the big-integer addition function. Use your MpAdd
  ///   and MpSub functions for the low-level arithmetic.
  if(z == NULL || b == NULL || a == NULL)
    return false;
  if(a->sign == zero)
    return BigIntCopy(z,b);
  if(b->sign == zero)
    return BigIntCopy(z,a);
  if(a->sign == b->sign) // +a - +b || -a - -b
  {
      int cmp = BigIntCompare(a, b);
      int wordcount = MAX(a->wordcount, b->wordcount);
      // Allocates 1 or wordcount space
      BigInteger* res = _BigIntAlloc(cmp == 0 ? 1 : wordcount);
      if(res == NULL)
        return false;
      if(cmp != 0)
      {
        if(MpSub(res->words, a->words, a->wordcount, b->words, b->wordcount))
        {
            mp_word_t null[1] = {0};
            MpSub(res->words, null, 1, res->words, res->wordcount);
        }
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
        res->wordcount = MAX(1,wordcount);
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
    bool result = BigIntAdd(z, a, res); // +a + +b
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
    bool result = BigIntAdd(z, res, a); // -a + -b
    BigIntFree(res);
    return result;
  }
  return false;
}
