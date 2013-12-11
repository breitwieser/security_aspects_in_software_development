///
/// \file
/// \brief Big integer addition and subtraction.
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
int BigIntSgn(const BigInteger *a)
{
  int ret = 0; // Assume zero

  if (a) {
    /// \todo Determine the sign of big-integer \c a and set \c ret
    ///  to -1 if \c a is negative, to +1 if \c a is positive, or to 0
    ///  if \c a is zero.
    ret = a->sign;
  }

  return ret;
}

//----------------------------------------------------------------------
int BigIntCompare(const BigInteger *a, const BigInteger *b)
{
  /// \todo Compare the big-integers \c a and \c b. First compare based
  ///  on then signs, in case of equal sign use your MpCompare function to
  ///  compare the magnitudes.
  if(a == NULL || b == NULL)
      return 0;
  if(a->words == NULL || b->words == 0)
      return 0;
  if(a->sign != b->sign)
  {
      if(a->sign < b->sign)
          return -1;
      else
          return 1;
  }
  return MpCompare(a->words, a->wordcount, b->words, b->wordcount);
}

