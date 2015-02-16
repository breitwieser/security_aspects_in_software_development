///
/// \file
/// \brief Public API of the tinybn big integer library.
///
/// \note This header file contains any implementation details
///  of your big integer library. You can put declarations of
///  internal helper functions, type and structure definitions,
///  constants, etc. in this file.
///
#ifndef TINYBN_IMP_H
#define TINYBN_IMP_H 1

// Include the public API definitions
#include "tinybn.h"

// Additional standard headers
#include <assert.h>
#include <stdlib.h>
#include <string.h>

enum Sign { negative = -1, zero = 0, positive = 1 };
struct BigInteger
{
  /// \todo Add the fields required by your big-integer implementation
  ///  here. At minimum you will need:
  ///   \li A pointer to your underlying words array
  ///   \li The size of your word array
  ///   \li An indication of the sign (positive, negative, zero?) of the big integer.
  enum Sign sign;
  mp_word_t* words;
  size_t wordcount;
};


/// \todo Add your own helper function declarations, type definitions,
///    etc. here.
BigInteger* _BigIntAlloc(size_t count);
bool _BigIntNull(BigInteger *z);
bool _BigIntResize(BigInteger *z, size_t size);

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#endif // TINYBN_IMP_H
