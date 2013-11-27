///
/// \file
/// \brief Common helpers for big integer testing.
///
/// The helper functions in this file are used to initialize
/// big integers for the remaining tests.
#include "tests.h"

//----------------------------------------------------------------------
BigInteger* TestHelperBigIntInit(size_t len, const mp_word_t *init_values)
{
  BigInteger *bn = BigIntAlloc();
  CU_ASSERT_PTR_NOT_NULL_FATAL(bn);

  // Load the words (note that our word array is in "big-endian" word
  // order, to improve readbility.)
  for (size_t i = 0; i < len; ++i) {
    CU_ASSERT_TRUE_FATAL(BigIntSetAt(bn, len - i - 1, init_values[i]));
  }

  return bn;
}
