///
/// \file
/// \brief Low-level multi-precision compare
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
int MpCompare(const mp_word_t *a, size_t len_a,
              const mp_word_t *b, size_t len_b)
{
  /// \todo Implement multi-precision comparison.
	// input validation
	if (len_a && a == NULL)
		return -2;
	if (len_b && b == NULL)
		return -2;

	//array lengths don't match
	//check if additional elements are non zero
	size_t min = len_a;
	if (len_a < len_b) {
		min = len_a;
		for (size_t i = len_b - 1; ; i--) {
			if (b[i] != 0)
				return -1;
			if(i == len_a)
				break;
		}
	} else if (len_b < len_a) {
		min = len_b;
		for (size_t i = len_a - 1; ; i--) {
			if (a[i] != 0)
				return 1;
			if(i == len_b)
				break;
		}
	}

	//one array holds zero elements
	//the other has only elements with zeros in it
	if(min == 0)
		return 0;

	//compare
	for (size_t i = min - 1; ; i--) {
		if (a[i] > b[i])
			return 1;
		else if (b[i] > a[i])
			return -1;
		if(i == 0)
			break;
	}

  return 0;
}
