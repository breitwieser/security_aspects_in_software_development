///
/// \file
/// \brief Basic big integer tests.
///
#include "tests.h"
#include "tinybn_imp.h"
#include "testhelpers.h"

//----------------------------------------------------------------------
void assertWords(const BigInteger *b1, const BigInteger *b2) {
	CU_ASSERT_EQUAL(b1->wordcount, b2->wordcount);
	for (size_t i = 0; i < b1->wordcount; i++)
		CU_ASSERT_EQUAL(b1->words[i], b2->words[i]);
}

//----------------------------------------------------------------------
void TestBigIntCopy(void) {
	//self assignment
	BigInteger *b1 = TEST_BIGINT_INIT(0x12345678);
	CU_ASSERT_TRUE(BigIntCopy(b1, b1));
	BigIntFree(b1);

	//copy to empty bigint instance
	BigInteger *b2 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	b2->sign = positive;
	BigInteger *b3 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntCopy(b3, b2));
	CU_ASSERT_EQUAL(b2->sign, b3->sign);
	assertWords(b2, b3);
	BigIntFree(b2);
	BigIntFree(b3);

	//copy to existing bigint instance
	BigInteger *b4 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	BigInteger *b5 = TEST_BIGINT_INIT(0xFFFFFFFF);
	CU_ASSERT_TRUE(BigIntCopy(b5, b4));
	CU_ASSERT_EQUAL(b4->sign, b5->sign);
	assertWords(b4, b5);
	BigIntFree(b4);
	BigIntFree(b5);

	//realloc fails
	BigInteger *b12 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	BigInteger *b13 = TEST_BIGINT_INIT(0xFFFFFFFF);
	TestSetAllocFaultCountdown(1);
	CU_ASSERT_FALSE(BigIntCopy(b13, b12));
	BigIntFree(b12);
	BigIntFree(b13);
	TestNoAllocFaults();

	//Invalid input
	BigInteger *b9 = TEST_BIGINT_INIT(0x12345678);
	CU_ASSERT_FALSE(BigIntCopy(NULL, NULL));
	CU_ASSERT_FALSE(BigIntCopy(NULL, b9));
	CU_ASSERT_FALSE(BigIntCopy(b9, NULL));
	BigIntFree(b9);

	//words of src bigint instance is zero
	BigInteger *b10 = BigIntAlloc();
	free(b10->words);
	b10->words=NULL;
	BigInteger *b11 = TEST_BIGINT_INIT(0x12345678);
	CU_ASSERT_FALSE(BigIntCopy(b11, b10));
	CU_ASSERT_TRUE(BigIntCopy(b10, b11)); //should work
	BigIntFree(b10);
	BigIntFree(b11);
}

//----------------------------------------------------------------------
void TestBigIntNeg(void) {
	//self assignment
	BigInteger *b1 = TEST_BIGINT_INIT(0x12345678);
	b1->sign = positive;
	BigInteger *target = TEST_BIGINT_INIT(0x12345678);
	target->sign = negative;
	CU_ASSERT_TRUE(BigIntNeg(b1, b1));
	CU_ASSERT_EQUAL(b1->sign, target->sign);
	assertWords(b1, target);
	BigIntFree(b1);
	BigIntFree(target);

	//dest is empty bigint instance
	BigInteger *b2 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	b2->sign = positive;
	BigInteger *b3 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntNeg(b3, b2));
	CU_ASSERT_EQUAL(b3->sign, negative);
	assertWords(b2, b3);
	BigInteger *b4 = BigIntAlloc();
	b2->sign = negative;
	CU_ASSERT_TRUE(BigIntNeg(b4, b2));
	CU_ASSERT_EQUAL(b4->sign, positive);
	assertWords(b2, b4);
	BigInteger *b5 = BigIntAlloc();
	b2->sign = zero;
	CU_ASSERT_TRUE(BigIntNeg(b5, b2));
	CU_ASSERT_EQUAL(b5->sign, zero);
	assertWords(b2, b5);
	BigIntFree(b2);
	BigIntFree(b3);
	BigIntFree(b4);
	BigIntFree(b5);

	//copy to existing bigint instance
	BigInteger *b6 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	BigInteger *b7 = TEST_BIGINT_INIT(0xFFFFFFFF);
	b6->sign = positive;
	CU_ASSERT_TRUE(BigIntNeg(b7, b6));
	CU_ASSERT_EQUAL(b7->sign, negative);
	assertWords(b6, b7);
	b6->sign = negative;
	CU_ASSERT_TRUE(BigIntNeg(b7, b6));
	CU_ASSERT_EQUAL(b7->sign, positive);
	assertWords(b6, b7);
	b6->sign = zero;
	CU_ASSERT_TRUE(BigIntNeg(b7, b6));
	CU_ASSERT_EQUAL(b7->sign, zero);
	assertWords(b6, b7);
	BigIntFree(b6);
	BigIntFree(b7);

	//realloc fails
	BigInteger *b12 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	BigInteger *b13 = TEST_BIGINT_INIT(0xFFFFFFFF);
	TestSetAllocFaultCountdown(1);
	CU_ASSERT_FALSE(BigIntNeg(b13, b12));
	BigIntFree(b12);
	BigIntFree(b13);
	TestNoAllocFaults();

	//Invalid input
	BigInteger *b9 = TEST_BIGINT_INIT(0x12345678);
	CU_ASSERT_FALSE(BigIntNeg(NULL, NULL));
	CU_ASSERT_FALSE(BigIntNeg(NULL, b9));
	CU_ASSERT_FALSE(BigIntNeg(b9, NULL));
	BigIntFree(b9);

	//words of src bigint instance is zero
	BigInteger *b10 = BigIntAlloc();
	free(b10->words);
	b10->words = NULL;
	BigInteger *b11 = TEST_BIGINT_INIT(0x12345678);
	CU_ASSERT_FALSE(BigIntNeg(b11, b10));
	CU_ASSERT_TRUE(BigIntNeg(b10, b11)); //should work
	BigIntFree(b10);
	BigIntFree(b11);

}

//----------------------------------------------------------------------
void TestBigIntAbs(void) {
	//self assignment
	BigInteger *b1 = TEST_BIGINT_INIT(0x12345678);
	b1->sign = positive;
	BigInteger *target = TEST_BIGINT_INIT(0x12345678);
	target->sign = positive;
	CU_ASSERT_TRUE(BigIntAbs(b1, b1));
	CU_ASSERT_EQUAL(b1->sign, target->sign);
	assertWords(b1, target);
	BigIntFree(b1);
	BigIntFree(target);

	//dest is empty bigint instance
	BigInteger *b2 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	b2->sign = positive;
	BigInteger *b3 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntAbs(b3, b2));
	CU_ASSERT_EQUAL(b3->sign, positive);
	assertWords(b2, b3);
	BigInteger *b4 = BigIntAlloc();
	b2->sign = negative;
	CU_ASSERT_TRUE(BigIntAbs(b4, b2));
	CU_ASSERT_EQUAL(b4->sign, positive);
	assertWords(b2, b4);
	BigInteger *b5 = BigIntAlloc();
	b2->sign = zero;
	CU_ASSERT_TRUE(BigIntAbs(b5, b2));
	CU_ASSERT_EQUAL(b5->sign, zero);
	assertWords(b2, b5);
	BigIntFree(b2);
	BigIntFree(b3);
	BigIntFree(b4);
	BigIntFree(b5);

	//copy to existing bigint instance
	BigInteger *b6 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	BigInteger *b7 = TEST_BIGINT_INIT(0xFFFFFFFF);
	b6->sign = positive;
	CU_ASSERT_TRUE(BigIntAbs(b7, b6));
	CU_ASSERT_EQUAL(b7->sign, positive);
	assertWords(b6, b7);
	b6->sign = negative;
	CU_ASSERT_TRUE(BigIntAbs(b7, b6));
	CU_ASSERT_EQUAL(b7->sign, positive);
	assertWords(b6, b7);
	b6->sign = zero;
	CU_ASSERT_TRUE(BigIntAbs(b7, b6));
	CU_ASSERT_EQUAL(b7->sign, zero);
	assertWords(b6, b7);
	BigIntFree(b6);
	BigIntFree(b7);

	//realloc fails
	BigInteger *b12 = TEST_BIGINT_INIT(0x12345678, 0x87654321);
	BigInteger *b13 = TEST_BIGINT_INIT(0xFFFFFFFF);
	TestSetAllocFaultCountdown(1);
	CU_ASSERT_FALSE(BigIntAbs(b13, b12));
	BigIntFree(b12);
	BigIntFree(b13);
	TestNoAllocFaults();

	//Invalid input
	BigInteger *b9 = TEST_BIGINT_INIT(0x12345678);
	CU_ASSERT_FALSE(BigIntAbs(NULL, NULL));
	CU_ASSERT_FALSE(BigIntAbs(NULL, b9));
	CU_ASSERT_FALSE(BigIntAbs(b9, NULL));
	BigIntFree(b9);

	//words of src bigint instance is zero
	BigInteger *b10 = BigIntAlloc();
	free(b10->words);
	b10->words = NULL;
	BigInteger *b11 = TEST_BIGINT_INIT(0x12345678);
	CU_ASSERT_FALSE(BigIntAbs(b11, b10));
	CU_ASSERT_TRUE(BigIntAbs(b10, b11)); //should work
	BigIntFree(b10);
	BigIntFree(b11);
}
