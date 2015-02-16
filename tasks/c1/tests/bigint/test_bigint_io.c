///
/// \file
/// \brief Unit tests for the big integer I/O APIs
///
#include "tests.h"
#include "tinybn_imp.h"

// Test prime ([Hankerson; A.2.1, P-192, n)
static const unsigned char TEST_PRIME_1[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x99, 0xDE, 0xF8, 0x36, 0x14,
		0x6B, 0xC9, 0xB1, 0xB4, 0xD2, 0x28, 0x31 };

//----------------------------------------------------------------------
void TestBigIntLoadSimple(void) {
	// Simple test of the BigIntLoad facilities (no leading zero bytes,
	// input length as an even multiple of word size)
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	CU_ASSERT_TRUE(BigIntLoad(p, TEST_PRIME_1, sizeof(TEST_PRIME_1)));

	// Expect the known prime
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(0xB4D22831));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0x146BC9B1));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 2), MP_WORD_C(0x99DEF836));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 3), MP_WORD_C(0xFFFFFFFF));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 4), MP_WORD_C(0xFFFFFFFF));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 5), MP_WORD_C(0xFFFFFFFF));

	// Out-of-bounds behavior
	CU_ASSERT_EQUAL(BigIntGetAt(p, 6), MP_WORD_C(0));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 7), MP_WORD_C(0));

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntLoadShort(void) {
	// Simple test of the BigIntLoad function (input is smaller than the
	// native word size)
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	// Test value: 2^16 + 1 = 65537
	static const unsigned char TEST_65537[] = { 0x01, 0x00, 0x01 };

	CU_ASSERT_TRUE(BigIntLoad(p, TEST_65537, sizeof(TEST_65537)));

	// Expect the known value
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(65537));
	CU_ASSERT_TRUE(BigIntGetWordCount(p) == 1);

	// Out-of-bounds behavior
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0));

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntLoadLeadingZero(void) {
	// Try to load a big integer with some extra leading zeros. The input size
	// is NOT an even multiple of the word size.
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	// Test value: 0x123456789ABCDEF
	static const unsigned char TEST_123456789ABCDEF[] = { 0x00, 0x01, 0x23,
			0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

	CU_ASSERT_TRUE(
			BigIntLoad(p, TEST_123456789ABCDEF, sizeof(TEST_123456789ABCDEF)));

	// Expect the known values
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(0x89ABCDEF));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0x01234567));
	CU_ASSERT_TRUE(BigIntGetWordCount(p) == 2);

	// Out-of-bounds behavior
	CU_ASSERT_EQUAL(BigIntGetAt(p, 2), MP_WORD_C(0));

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntGetSet(void) {
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	// We start with a zero integer
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(0));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 2), MP_WORD_C(0));

	// Set first word, then check
	CU_ASSERT_TRUE(BigIntSetAt(p, 0, MP_WORD_C(0x01234567)));

	CU_ASSERT(BigIntGetWordCount(p) == 1U); // Need at least one word
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(0x01234567));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 2), MP_WORD_C(0));

	// Set third word, then check
	CU_ASSERT_TRUE(BigIntSetAt(p, 2, MP_WORD_C(0x89ABCDEF)));

	CU_ASSERT(BigIntGetWordCount(p) == 3U); // Need at least three words (second is zero)
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(0x01234567));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 2), MP_WORD_C(0x89ABCDEF));

	// Set second word, then check
	CU_ASSERT_TRUE(BigIntSetAt(p, 1, MP_WORD_C(0xDEADBEEF)));

	CU_ASSERT(BigIntGetWordCount(p) == 3U); // Need at least three words (all words are occupied)
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(0x01234567));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0xDEADBEEF));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 2), MP_WORD_C(0x89ABCDEF));

	// Now set the third word again
	CU_ASSERT_TRUE(BigIntSetAt(p, 2, MP_WORD_C(0)));

	CU_ASSERT(BigIntGetWordCount(p) == 2U); // Need at least two words
	CU_ASSERT_EQUAL(BigIntGetAt(p, 0), MP_WORD_C(0x01234567));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 1), MP_WORD_C(0xDEADBEEF));
	CU_ASSERT_EQUAL(BigIntGetAt(p, 2), MP_WORD_C(0));

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntSaveSimple(void) {
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	// Fill the test-prime
	CU_ASSERT_TRUE(BigIntSetAt(p, 0, MP_WORD_C(0xB4D22831)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 1, MP_WORD_C(0x146BC9B1)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 2, MP_WORD_C(0x99DEF836)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 3, MP_WORD_C(0xFFFFFFFF)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 4, MP_WORD_C(0xFFFFFFFF)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 5, MP_WORD_C(0xFFFFFFFF)));

	// Fill the result buffer with 0xAA
	// (note that this pattern does not occur in our test input)
	unsigned char result[sizeof(TEST_PRIME_1)];
	memset(result, 0xAA, sizeof(result));

	// Save to a byte buffer of the correct length
	CU_ASSERT_TRUE(BigIntSave(result, sizeof(result), p));

	// Compare the result with the known answer
	CU_ASSERT(memcmp(result, TEST_PRIME_1, sizeof(TEST_PRIME_1)) == 0);

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntSaveTruncate(void) {
	// Truncation behavior of the BigIntSave function
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	// Setup the test value: 0xF1E2D3C40A0B0C0D
	CU_ASSERT_TRUE(BigIntSetAt(p, 0, MP_WORD_C(0x0A0B0C0D)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 1, MP_WORD_C(0xF1E2D3C4)));

	// Save to one-byte buffer
	unsigned char test_1[8];
	memset(test_1, 0xFF, sizeof(test_1));
	CU_ASSERT_TRUE(BigIntSave(test_1, 1, p));
	CU_ASSERT_EQUAL(test_1[0], 0x0D);
	CU_ASSERT_EQUAL(test_1[1], 0xFF);
	CU_ASSERT_EQUAL(test_1[2], 0xFF);
	CU_ASSERT_EQUAL(test_1[3], 0xFF);
	CU_ASSERT_EQUAL(test_1[4], 0xFF);
	CU_ASSERT_EQUAL(test_1[5], 0xFF);
	CU_ASSERT_EQUAL(test_1[6], 0xFF);
	CU_ASSERT_EQUAL(test_1[7], 0xFF);

	// Save to three-byte buffer
	unsigned char test_3[8];
	memset(test_3, 0xCC, sizeof(test_3));
	CU_ASSERT_TRUE(BigIntSave(test_3, 3, p));
	CU_ASSERT_EQUAL(test_3[0], 0x0B);
	CU_ASSERT_EQUAL(test_3[1], 0x0C);
	CU_ASSERT_EQUAL(test_3[2], 0x0D);
	CU_ASSERT_EQUAL(test_3[3], 0xCC);
	CU_ASSERT_EQUAL(test_3[4], 0xCC);
	CU_ASSERT_EQUAL(test_3[5], 0xCC);
	CU_ASSERT_EQUAL(test_3[6], 0xCC);
	CU_ASSERT_EQUAL(test_3[7], 0xCC);

	// Save to four-byte buffer
	unsigned char test_4[8];
	memset(test_4, 0xAA, sizeof(test_4));
	CU_ASSERT_TRUE(BigIntSave(test_4, 4, p));
	CU_ASSERT_EQUAL(test_4[0], 0x0A);
	CU_ASSERT_EQUAL(test_4[1], 0x0B);
	CU_ASSERT_EQUAL(test_4[2], 0x0C);
	CU_ASSERT_EQUAL(test_4[3], 0x0D);
	CU_ASSERT_EQUAL(test_4[4], 0xAA);
	CU_ASSERT_EQUAL(test_4[5], 0xAA);
	CU_ASSERT_EQUAL(test_4[6], 0xAA);
	CU_ASSERT_EQUAL(test_4[7], 0xAA);

	// Save to four-byte buffer
	// (second try, the pointer passed to BigIntSave points
	//  into the middle of our result array)
	unsigned char test_4o3[8];
	memset(test_4o3, 0xEE, sizeof(test_4o3));
	CU_ASSERT_TRUE(BigIntSave(test_4o3 + 3, 4, p));
	CU_ASSERT_EQUAL(test_4o3[0], 0xEE);
	CU_ASSERT_EQUAL(test_4o3[1], 0xEE);
	CU_ASSERT_EQUAL(test_4o3[2], 0xEE);
	CU_ASSERT_EQUAL(test_4o3[3], 0x0A);
	CU_ASSERT_EQUAL(test_4o3[4], 0x0B);
	CU_ASSERT_EQUAL(test_4o3[5], 0x0C);
	CU_ASSERT_EQUAL(test_4o3[6], 0x0D);
	CU_ASSERT_EQUAL(test_4o3[7], 0xEE);

	// Save to five-byte buffer
	unsigned char test_5o1[8];
	memset(test_5o1, 0xCC, sizeof(test_5o1));
	CU_ASSERT_TRUE(BigIntSave(test_5o1 + 1, 5, p));
	CU_ASSERT_EQUAL(test_5o1[0], 0xCC);
	CU_ASSERT_EQUAL(test_5o1[1], 0xC4);
	CU_ASSERT_EQUAL(test_5o1[2], 0x0A);
	CU_ASSERT_EQUAL(test_5o1[3], 0x0B);
	CU_ASSERT_EQUAL(test_5o1[4], 0x0C);
	CU_ASSERT_EQUAL(test_5o1[5], 0x0D);
	CU_ASSERT_EQUAL(test_5o1[6], 0xCC);
	CU_ASSERT_EQUAL(test_5o1[7], 0xCC);

	// Save to eight-byte buffer (no truncation)
	unsigned char test_8[10];
	memset(test_8, 0xBB, sizeof(test_8));
	CU_ASSERT_TRUE(BigIntSave(test_8 + 1, 8, p));
	CU_ASSERT_EQUAL(test_8[0], 0xBB);
	CU_ASSERT_EQUAL(test_8[1], 0xF1);
	CU_ASSERT_EQUAL(test_8[2], 0xE2);
	CU_ASSERT_EQUAL(test_8[3], 0xD3);
	CU_ASSERT_EQUAL(test_8[4], 0xC4);
	CU_ASSERT_EQUAL(test_8[5], 0x0A);
	CU_ASSERT_EQUAL(test_8[6], 0x0B);
	CU_ASSERT_EQUAL(test_8[7], 0x0C);
	CU_ASSERT_EQUAL(test_8[8], 0x0D);
	CU_ASSERT_EQUAL(test_8[9], 0xBB);

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntSaveExtend(void) {
	// Zero-extension behavior of the BigIntSave function
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	// Setup the test value: 0x9182736AFBECDDC
	CU_ASSERT_TRUE(BigIntSetAt(p, 0, MP_WORD_C(0xAFBECDDC)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 1, MP_WORD_C(0x09182736)));

	// Save to eight-byte buffer (no extend or truncate)
	unsigned char test_8[10];
	memset(test_8, 0xEE, sizeof(test_8));
	CU_ASSERT_TRUE(BigIntSave(test_8 + 1, 8, p));
	CU_ASSERT_EQUAL(test_8[0], 0xEE);
	CU_ASSERT_EQUAL(test_8[1], 0x09);
	CU_ASSERT_EQUAL(test_8[2], 0x18);
	CU_ASSERT_EQUAL(test_8[3], 0x27);
	CU_ASSERT_EQUAL(test_8[4], 0x36);
	CU_ASSERT_EQUAL(test_8[5], 0xAF);
	CU_ASSERT_EQUAL(test_8[6], 0xBE);
	CU_ASSERT_EQUAL(test_8[7], 0xCD);
	CU_ASSERT_EQUAL(test_8[8], 0xDC);
	CU_ASSERT_EQUAL(test_8[9], 0xEE);

	// Save to nine-byte buffer (add one leading zero)
	unsigned char test_9[11];
	memset(test_9, 0x55, sizeof(test_9));
	CU_ASSERT_TRUE(BigIntSave(test_9 + 1, 9, p));
	CU_ASSERT_EQUAL(test_9[0], 0x55);
	CU_ASSERT_EQUAL(test_9[1], 0x00);
	CU_ASSERT_EQUAL(test_9[2], 0x09);
	CU_ASSERT_EQUAL(test_9[3], 0x18);
	CU_ASSERT_EQUAL(test_9[4], 0x27);
	CU_ASSERT_EQUAL(test_9[5], 0x36);
	CU_ASSERT_EQUAL(test_9[6], 0xAF);
	CU_ASSERT_EQUAL(test_9[7], 0xBE);
	CU_ASSERT_EQUAL(test_9[8], 0xCD);
	CU_ASSERT_EQUAL(test_9[9], 0xDC);
	CU_ASSERT_EQUAL(test_9[10], 0x55);

	// Save to eleven-byte buffer (three leading zeros)
	unsigned char test_11[13];
	memset(test_11, 0x33, sizeof(test_11));
	CU_ASSERT_TRUE(BigIntSave(test_11 + 1, 11, p));
	CU_ASSERT_EQUAL(test_11[0], 0x33);
	CU_ASSERT_EQUAL(test_11[1], 0x00);
	CU_ASSERT_EQUAL(test_11[2], 0x00);
	CU_ASSERT_EQUAL(test_11[3], 0x00);
	CU_ASSERT_EQUAL(test_11[4], 0x09);
	CU_ASSERT_EQUAL(test_11[5], 0x18);
	CU_ASSERT_EQUAL(test_11[6], 0x27);
	CU_ASSERT_EQUAL(test_11[7], 0x36);
	CU_ASSERT_EQUAL(test_11[8], 0xAF);
	CU_ASSERT_EQUAL(test_11[9], 0xBE);
	CU_ASSERT_EQUAL(test_11[10], 0xCD);
	CU_ASSERT_EQUAL(test_11[11], 0xDC);
	CU_ASSERT_EQUAL(test_11[12], 0x33);

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntLoadNull(void) {
	BigInteger *p1 = BigIntAlloc();
	BigInteger *p2 = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p1);
	CU_ASSERT_PTR_NOT_NULL_FATAL(p2);

	// Try to load from NULL buffer with non-zero size
	// Here the load fails since there are no valid bytes at NULL.
	CU_ASSERT_FALSE(BigIntLoad(p1, NULL, sizeof(TEST_PRIME_1)));

	// Try to load from NULL buffer (with zero size)
	// Here the load succeeds, since we do not read any bytes (the
	// size is zero). In this case it does not matter wether we give
	// NULL or any other pointer as data parameter.
	// Big integer p2 will be initialized as zero.
	CU_ASSERT_TRUE(BigIntLoad(p2, NULL, 0));

	// Try to load into NULL big-integer
	CU_ASSERT_FALSE(BigIntLoad(NULL, TEST_PRIME_1, sizeof(TEST_PRIME_1)));
	CU_ASSERT_FALSE(BigIntLoad(NULL, TEST_PRIME_1, 0));
	CU_ASSERT_FALSE(BigIntLoad(NULL, NULL, sizeof(TEST_PRIME_1)));
	CU_ASSERT_FALSE(BigIntLoad(NULL, NULL, 0));

	BigIntFree(p2);
	BigIntFree(p1);
}

//----------------------------------------------------------------------
void TestBigIntSaveNull(void) {
	BigInteger *p = BigIntAlloc();
	CU_ASSERT_PTR_NOT_NULL_FATAL(p);

	// Fill in a test pattern
	CU_ASSERT_TRUE(BigIntSetAt(p, 0, MP_WORD_C(0xC0DEC0DE)));
	CU_ASSERT_TRUE(BigIntSetAt(p, 1, MP_WORD_C(0xDEADBEEF)));

	// Try to save to a NULL buffer with non-zero size
	// Here the save fails, since there are no writable bytes at NULL.
	CU_ASSERT_FALSE(BigIntSave(NULL, 64, p));

	// Try to save to a NULL buffer with zero size
	// Here the save operation will succeed, since we logically truncate
	// our output to zero bytes. In this case it does not matter wether we
	// give NULL or any other pointer as data parameter.
	CU_ASSERT_TRUE(BigIntSave(NULL, 0, p));

	BigIntFree(p);
}

//----------------------------------------------------------------------
void TestBigIntGetNull(void) {
	// Try to access words of the NULL big integer
	// Both BigIntGetAt calls gracefully fail and return zero.
	CU_ASSERT_EQUAL(BigIntGetAt(NULL, 0), MP_WORD_C(0));
	CU_ASSERT_EQUAL(BigIntGetAt(NULL, 1), MP_WORD_C(0));

	// Try to get the number of words used by the NULL big-integer
	CU_ASSERT_EQUAL(BigIntGetWordCount(NULL), 0);
}

//----------------------------------------------------------------------
void TestBigIntSetNull(void) {
	// Try to write words of the NULL big integer
	// Both BigIntSetAt calls gracefully fail and return false.
	CU_ASSERT_FALSE(BigIntSetAt(NULL, 0, MP_WORD_C(0xFFFFFFFF)));
	CU_ASSERT_FALSE(BigIntSetAt(NULL, 1, MP_WORD_C(0xFFFFFFFF)));
}

//----------------------------------------------------------------------
void TestBigIntIOTask3(void) {
	//BigIntGetWordCount
	BigInteger *b1 = BigIntAlloc();
	free(b1->words);
	b1->words=NULL;
	CU_ASSERT_FALSE(BigIntGetWordCount(b1));
	BigIntFree(b1);

	//BigIntGetAt
	BigInteger *b2 = BigIntAlloc();
	free(b2->words);
	b2->words = NULL;
	CU_ASSERT_FALSE(BigIntGetAt(b2, 0));
	BigIntFree(b2);

	//BigIntSetAt
	BigInteger *b3 = BigIntAlloc();
	free(b3->words);
	b3->words = NULL;
	CU_ASSERT_TRUE(BigIntSetAt(b3, 3, 0xFFFFFFFF));
	assertMagnitude(b3,  (mp_word_t[]) {0, 0, 0, 0xFFFFFFFF}, 4);
	BigIntFree(b3);

	BigInteger *b4 = BigIntAlloc();
	TestSetAllocFaultCountdown(1);
	CU_ASSERT_FALSE(BigIntSetAt(b4, 3, 0xFFFFFFFF));
	TestNoAllocFaults();
	BigIntFree(b4);

	//BigIntLoad
	// Test value: 0x123456789ABCDEF
	static const unsigned char TEST_123456789ABCDEF[] = { 0x00, 0x01, 0x23,
			0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

	static const unsigned char TEST_00[] = { 0x00, 0x00};

	BigInteger *b5 = BigIntAlloc();
	free(b5->words);
	b5->words = NULL;
	CU_ASSERT_TRUE(BigIntLoad(b5, TEST_123456789ABCDEF, sizeof(TEST_123456789ABCDEF)));
	assertMagnitude(b5,  (mp_word_t[]) {0x89ABCDEF, 0x1234567}, 2);
	BigIntFree(b5);

	BigInteger *b_5_1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntLoad(b_5_1, TEST_00, sizeof(TEST_00)));
	CU_ASSERT_EQUAL(b_5_1->sign, zero);
	BigIntFree(b_5_1);

	BigInteger *b6 = BigIntAlloc();
	TestSetAllocFaultCountdown(1);
	CU_ASSERT_FALSE(BigIntLoad(b6, TEST_123456789ABCDEF, sizeof(TEST_123456789ABCDEF)));
	TestNoAllocFaults();
	BigIntFree(b6);

	//BigIntSave
	unsigned char result[sizeof(TEST_PRIME_1)];
	CU_ASSERT_FALSE(BigIntSave(result, sizeof(result), NULL));

}
