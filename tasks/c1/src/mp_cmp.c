///
/// \file
/// \brief Low-level multi-precision compare
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
int MpCompare(const mp_word_t *a, size_t len_a,
              const mp_word_t *b, size_t len_b)
{
  int result = 0;

  /// \todo Implement multi-precision comparison.
	// input validation
	if (len_a && a == NULL)
		return 0;
	if (len_b && b == NULL)
		return 0;

	//compare
	if (len_a > len_b)
		return 1;
	else if (len_b > len_a)
		return -1;

	for (int i = len_a - 1; i >= 0; i--) {
		if (a[i] > b[i])
			return 1;
		else if (b[i] > a[i])
			return -1;
	}

  return result;
}
