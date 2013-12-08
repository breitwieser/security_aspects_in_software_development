///
/// \file
/// \brief Low-level multi-precision subtraction
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
bool MpSub(mp_word_t *z, const mp_word_t *a, size_t len_a,
           const mp_word_t *b, size_t len_b)
{
  bool borrow = false;

  /// \todo Implement multi-precision subtraction.

	// input validation
	if (len_a && a == NULL)
		return 0;
	if (len_b && b == NULL)
		return 0;
	if (z == NULL)
		return NULL;

	//generate result array
	int len_z = len_a > len_b ? len_a : len_b;
//	printf(" \n");
	//do calculation
	for (int i = 0; i < len_z; i++) {
		mp_word_t w_a, w_b, w_z = 0;
		if (len_a > i)
			w_a = a[i];
		if (len_b > i)
			w_b = b[i];

		w_z = w_a - w_b - (unsigned) borrow;


		//overflow
		borrow = w_z > w_a ? true : false;
		z[i] = w_z;
//		printf("%10x - %10x - %10x\n - borrow: %d", w_z, w_a, w_b, borrow);

	}


  return borrow;
}
