///
/// \file
/// \brief Big integer input and output
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
size_t BigIntGetWordCount(const BigInteger *z)
{
  /// \todo Returns the number of words used in the word array of
  ///   your big integer.
  return 0; // Not yet implemented
}

//----------------------------------------------------------------------
mp_word_t BigIntGetAt(const BigInteger *z, size_t index)
{
  /// \todo Return the word at the given inex from the big integer's
  ///   word array. The word containing the least significant bit is
  ///   word at index 0. Trying to access a non-existing word (out of
  ///   bounds) returns zero.

  return 0; // Not yet implemented
}

//----------------------------------------------------------------------
bool BigIntSetAt(BigInteger *z, size_t index, mp_word_t value)
{
  /// \todo Sets a given word of the big integer's word array to the
  ///   given value. The word containing the least significant bit is
  ///   word at index 0. The word array is grown by this function if
  ///   the user specifies a non-existing index.

  return false; // Not yet implemented
}

//----------------------------------------------------------------------
bool BigIntLoad(BigInteger *z, const unsigned char *data, size_t len)
{
  /// \todo Load the byte array into the big integer.
  return false; // Not yet implemented
}

//----------------------------------------------------------------------
bool BigIntSave(unsigned char *data, size_t len, const BigInteger *a)
{
  /// \todo Save the big integer into a byte array.
  return false; // Not yet implemented
}
