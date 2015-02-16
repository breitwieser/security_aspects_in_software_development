///
/// \file
/// \brief Basic big integer tests.
///

#include "tests.h"
#include "tinybn_imp.h"

//----------------------------------------------------------------------
void TestBigIntMult() {
	//self assignment
	//1*2=2
	BigInteger *b1 = TEST_BIGINT_INIT(0x00000001);
	BigInteger *b2 = TEST_BIGINT_INIT(0x00000002);
	b1->sign = positive;
	b2->sign = positive;
	CU_ASSERT_TRUE(BigIntMul(b1, b1, b2));
	CU_ASSERT_EQUAL(b1->sign, positive);
	assertMagnitude(b1, (mp_word_t[] ) { 0x00000002 }, 1);
	//2*2=4
	CU_ASSERT_TRUE(BigIntMul(b2, b1, b2));
	CU_ASSERT_EQUAL(b2->sign, positive);
	assertMagnitude(b2, (mp_word_t[] ) { 0x00000004 }, 1);
	BigIntFree(b1);
	BigIntFree(b2);

	//regular calculations
	//1*2=2
	BigInteger *b3 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	BigInteger *b4 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	BigInteger *res1 = BigIntAlloc();
	b3->sign = positive;
	b4->sign = positive;
	CU_ASSERT_TRUE(BigIntMul(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000002 }, 1);
	BigIntFree(res1);
	//2*1=2
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntMul(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000002 }, 1);
	BigIntFree(res1);
	//(-1)*(-2)=2
	res1 = BigIntAlloc();
	b3->sign = negative;
	b4->sign = negative;
	CU_ASSERT_TRUE(BigIntMul(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000002 }, 1);
	BigIntFree(res1);
	//(-2)*(-1)=2
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntMul(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000002 }, 1);
	BigIntFree(res1);
	//1*(-2)=-2
	res1 = BigIntAlloc();
	b3->sign = positive;
	b4->sign = negative;
	CU_ASSERT_TRUE(BigIntMul(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000002 }, 1);
	BigIntFree(res1);
	//-1*2=-2
	res1 = BigIntAlloc();
	b3->sign = negative;
	b4->sign = positive;
	CU_ASSERT_TRUE(BigIntMul(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000002 }, 1);
	BigIntFree(res1);
	//0*2=0
	BigInteger *b5 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000000);
	res1 = BigIntAlloc();
	b4->sign = positive;
	CU_ASSERT_TRUE(BigIntMul(res1, b5, b4));
	CU_ASSERT_EQUAL(res1->sign, zero);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000000 }, 1);
	BigIntFree(res1);
	//2*0=0
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntMul(res1, b4, b5));
	CU_ASSERT_EQUAL(res1->sign, zero);
	assertMagnitude(res1, (mp_word_t[] ) { 0x00000000 }, 1);
	BigIntFree(res1);

	BigIntFree(b3);
	BigIntFree(b4);
	BigIntFree(b5);


	//overflow
	BigInteger *b7 = TEST_BIGINT_INIT(0xFFFFFFFF);
	BigInteger *b8 = TEST_BIGINT_INIT(0xFF000000);
	BigInteger *res3 = BigIntAlloc();
	b7->sign = positive;
	b8->sign = positive;
	CU_ASSERT_TRUE(BigIntMul(res3, b7, b8));
	CU_ASSERT_EQUAL(res3->sign, positive);
	assertMagnitude(res3, (mp_word_t[] ) { 0x01000000, 0xFEFFFFFF }, 2);
	BigIntFree(b7);
	BigIntFree(b8);
	BigIntFree(res3);

	//invalid input
	CU_ASSERT_FALSE(BigIntMul(NULL, NULL, NULL));
	BigInteger *b9 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	BigInteger *b10 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	BigInteger *b11 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	CU_ASSERT_FALSE(BigIntMul(NULL, b9, b10));
	CU_ASSERT_FALSE(BigIntMul(b9, NULL, b10));
	CU_ASSERT_FALSE(BigIntMul(b9, b10, NULL));
	BigIntFree(b9);
	BigIntFree(b10);
	BigIntFree(b11);

	//allocation of temp object fails
	BigInteger *b12 = TEST_BIGINT_INIT(0x00000001);
	BigInteger *b13 = TEST_BIGINT_INIT(0x00000002);
	BigInteger *res4 = BigIntAlloc();
	TestSetAllocFaultCountdown(1);
	CU_ASSERT_FALSE(BigIntMul(res4, b12, b13));
	BigIntFree(b12);
	BigIntFree(b13);
	BigIntFree(res4);
	TestNoAllocFaults();
}
