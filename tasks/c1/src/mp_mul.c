///
/// \file
/// \brief Low-level multi-precision multiplication
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
void MpMul(mp_word_t *restrict z, const mp_word_t *a, size_t len_a,
           const mp_word_t *b, size_t len_b)
{
  /// \todo Implement multi-precision multiplication. See Algorithm 14.12 in
  ///  the "Handbook of Applied Cryptography" for a suitable multiplication
  ///  algorithm.
	// input validation
	if (len_a && a == NULL)
		return;
	if (len_b && b == NULL)
		return;
	if (z == NULL)
		return;

	//muliply
	int len = len_a > len_b ? len_a : len_b;
	mp_long_t carry = 0;

	//initialize output var
	for (int i = 0; i < len_a + len_b; i++)
		z[i] = 0;

	//	printf(" \n");
	mp_long_t uv;
	for (int i = 0; i < len_b; i++) {
		carry = 0;
		for (int j = 0; j < len_a; j++) {
			mp_long_t w_a, w_b = 0;
			if (len_a > j)
				w_a = a[j];
			if (len_b > i)
				w_b = b[i];

			uv = z[i + j] + w_a * w_b + carry;
			z[i + j] = uv;
			carry = (mp_word_t)(uv >> sizeof(mp_word_t)*8);
//			printf("z[%2d] %10x - carry %10x - a %10x - b %10x\n", i+j, z[i+j], carry, w_a, w_b);
		}
		z[i + len_a] = carry;

	}
}
