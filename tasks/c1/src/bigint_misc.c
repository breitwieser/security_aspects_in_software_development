///
/// \file
/// \brief Miscellaneous functiosn for big integer
///
#include "tinybn_imp.h"



//----------------------------------------------------------------------
bool BigIntCopy(BigInteger *z, const BigInteger *a)
{
  // Parameter check
  if (!z || !a) {
    return false;
  }

  /// \todo Copy the value and sign of big-integer a to
  ///  big-integer z. (Note that a and z may refer to the same object).
  if(z == a)
      return true;
  if(a->words == NULL)
    return false;
  z->sign = a->sign;
  mp_word_t *tmp = (mp_word_t*)realloc(z->words, a->wordcount*sizeof(mp_word_t));
  if(tmp == NULL)
      return false;
  z->words = tmp;
  z->wordcount = a->wordcount;
  memcpy(z->words, a->words, a->wordcount*sizeof(mp_word_t));
  return true;
 }

//----------------------------------------------------------------------
bool BigIntNeg(BigInteger *z, const BigInteger *a)
{
  /// \todo Copy the value of big-integer \c a to big-integer \c z. Set
  /// the sign of \c z to the opposite of the sign of \c a.
  /// (Note that a and z may refer to the same object).
  if(BigIntCopy(z,a))
  {
    if(a->sign == zero)
      z->sign = zero;
    else
      z->sign = -a->sign;
    return true;
  }
  return false;
}


//----------------------------------------------------------------------
bool BigIntAbs(BigInteger *z, const BigInteger *a)
{
  bool ret;

  if (BigIntSgn(a) >= 0) {
    // Zero or positive (a >= 0): z = |a| = a
    ret = BigIntCopy(z, a);

  } else {
    // Negative (a < 0): z = |a| = -a
    ret = BigIntNeg(z, a);
  }

  return ret;
}
