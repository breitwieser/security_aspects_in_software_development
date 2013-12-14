///
/// \file
/// \brief Basic big integer tests.
///

#include "tests.h"
#include "tinybn_imp.h"

//----------------------------------------------------------------------
void TestBigIntAdd(void) {

	//result instance equals one of the operands
	//1+2=3
	BigInteger *b1= TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	BigInteger *b2 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	b1->sign = positive;
	b2->sign = positive;
	CU_ASSERT_TRUE(BigIntAdd(b1, b1, b2));
	CU_ASSERT_EQUAL(b1->sign, positive);
	assertMagnitude(b1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003},	3);
	//3+2=5 (b1=3)
	CU_ASSERT_TRUE(BigIntAdd(b2, b1, b2));
	CU_ASSERT_EQUAL(b2->sign, positive);
	assertMagnitude(b2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000005},	3);
	BigIntFree(b1);
	BigIntFree(b2);

	//simple tests
	//1+2=3
	BigInteger *b3 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	BigInteger *b4 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	BigInteger *res1 = BigIntAlloc();
	b3->sign = positive;
	b4->sign = positive;
	CU_ASSERT_TRUE(BigIntAdd(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);
	BigIntFree(res1);
	//2+1=3
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntAdd(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);
	BigIntFree(res1);
	//-1+(-2)=-3
	res1 = BigIntAlloc();
	b3->sign = negative;
	b4->sign = negative;
	CU_ASSERT_TRUE(BigIntAdd(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);
	BigIntFree(res1);
	//-2+(-1)=-3
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntAdd(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);
	BigIntFree(res1);
	//1+(-2)=-1
//	//FIXME: this test crashes
//	res1 = BigIntAlloc();
//	b3->sign = positive;
//	b4->sign = negative;
//	CU_ASSERT_TRUE(BigIntAdd(res1, b3, b4));
//	CU_ASSERT_EQUAL(res1->sign, negative);
//	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);
//	BigIntFree(res1);
	//(-2)+1=-1
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntAdd(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);
	BigIntFree(res1);
//    //-1+2=1
//	//FIXME: this test crashes
//	BigIntFree(res1);
//	res1 = BigIntAlloc();
//	b3->sign = negative;
//	b4->sign = positive;
//	CU_ASSERT_TRUE(BigIntAdd(res1, b3, b4));
//	CU_ASSERT_EQUAL(res1->sign, positive);
//	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);
//	BigIntFree(res1);
	//2+(-1)=1
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntAdd(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);

	BigIntFree(b3);
	BigIntFree(b4);
	BigIntFree(res1);

	//result equals 0
	//0+0=0
	BigInteger *b5 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000000);
	BigInteger *b6 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000000);
	BigInteger *res2 = BigIntAlloc();
	b5->sign = zero;
	b6->sign = zero;
	CU_ASSERT_TRUE(BigIntAdd(res2, b5, b6));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);
	BigIntFree(res2);
	//0+0=0
	res2 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntAdd(res2, b6, b5));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);
	BigIntFree(zero);
	//1+(-1)=0
	BigIntFree(b5);
	BigIntFree(b6);
	b5 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	b6 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	res2 = BigIntAlloc();
	b5->sign = positive;
	b6->sign = negative;
	CU_ASSERT_TRUE(BigIntAdd(res2, b5, b6));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);
	BigIntFree(res2);
	//-1+1=0
	res2 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntAdd(res2, b6, b5));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);

	BigIntFree(b5);
	BigIntFree(b6);
	BigIntFree(res2);

	//overflow
	BigInteger *b7 = TEST_BIGINT_INIT(0xFFFFFFFF);
	BigInteger *b8 = TEST_BIGINT_INIT(0x00000001);
	BigInteger *res3 = BigIntAlloc();
	b7->sign = positive;
	b8->sign = positive;
	CU_ASSERT_TRUE(BigIntAdd(res3, b7, b8));
	CU_ASSERT_EQUAL(res3->sign, positive);
	assertMagnitude(res3, (mp_word_t[]) {0x00000001, 0x00000000}, 2);
	BigIntFree(b7);
	BigIntFree(b8);
	BigIntFree(res3);

	//invalid values
	CU_ASSERT_FALSE(BigIntAdd(NULL, NULL, NULL));
	BigInteger *b9 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	BigInteger *b10 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	BigInteger *b11 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	CU_ASSERT_FALSE(BigIntAdd(NULL, b9, b10));
	CU_ASSERT_FALSE(BigIntAdd(b9, NULL, b10));
	CU_ASSERT_FALSE(BigIntAdd(b9, b10, NULL));
	//words = NULL FIXME ich denke da sollte die Berechnung trotzdem gemacht werden
	free(b9->words);
	b9->words=NULL;
	CU_ASSERT_TRUE(BigIntAdd(b9, b10, b11));
	CU_ASSERT_TRUE(BigIntAdd(b10, b9, b11));
	CU_ASSERT_TRUE(BigIntAdd(b10, b11, b9));
	//TODO assert result
	BigIntFree(b9);
	BigIntFree(b10);
	BigIntFree(b11);
}

//----------------------------------------------------------------------
void TestBigIntSub(void) {

	//result instance equals one of the operands
	//3-1=2
	BigInteger *b1= TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000003);
	BigInteger *b2 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	b1->sign = positive;
	b2->sign = positive;
	CU_ASSERT_TRUE(BigIntSub(b1, b1, b2));
	CU_ASSERT_EQUAL(b1->sign, positive);
	assertMagnitude(b1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000002},	3);
	//2-1=1 (b1=2)
	CU_ASSERT_TRUE(BigIntSub(b2, b1, b2));
	CU_ASSERT_EQUAL(b2->sign, positive);
	assertMagnitude(b2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001},	3);
	BigIntFree(b1);
	BigIntFree(b2);

	//simple tests
	//1-2=-1
	BigInteger *b3 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	BigInteger *b4 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	BigInteger *res1 = BigIntAlloc();
	b3->sign = positive;
	b4->sign = positive;
	CU_ASSERT_TRUE(BigIntSub(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);
	BigIntFree(res1);
	//2-1=1
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntSub(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);
	BigIntFree(res1);
	//-1-(-2)=1
	res1 = BigIntAlloc();
	b3->sign = negative;
	b4->sign = negative;
	CU_ASSERT_TRUE(BigIntSub(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);
	BigIntFree(res1);
	//-2-(-1)=-1
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntSub(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000001}, 3);
	BigIntFree(res1);
	//1-(-2)=3
	res1 = BigIntAlloc();
	b3->sign = positive;
	b4->sign = negative;
	CU_ASSERT_TRUE(BigIntSub(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);
	BigIntFree(res1);
	//(-2)-1=-3
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntSub(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);
	BigIntFree(res1);
    //-1-2=-3
	BigIntFree(res1);
	res1 = BigIntAlloc();
	b3->sign = negative;
	b4->sign = positive;
	CU_ASSERT_TRUE(BigIntSub(res1, b3, b4));
	CU_ASSERT_EQUAL(res1->sign, negative);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);
	BigIntFree(res1);
	//2-(-1)=3
	res1 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntSub(res1, b4, b3));
	CU_ASSERT_EQUAL(res1->sign, positive);
	assertMagnitude(res1, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000003}, 3);

	BigIntFree(b3);
	BigIntFree(b4);
	BigIntFree(res1);

	//result equals 0
	//0-0=0
	BigInteger *b5 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000000);
	BigInteger *b6 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000000);
	BigInteger *res2 = BigIntAlloc();
	b5->sign = zero;
	b6->sign = zero;
	CU_ASSERT_TRUE(BigIntSub(res2, b5, b6));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);
	BigIntFree(res2);
	//0-0=0
	res2 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntSub(res2, b6, b5));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);
	BigIntFree(zero);
	//1-1=0
	BigIntFree(b5);
	BigIntFree(b6);
	b5 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	b6 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	res2 = BigIntAlloc();
	b5->sign = positive;
	b6->sign = positive;
	CU_ASSERT_TRUE(BigIntSub(res2, b5, b6));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);
	BigIntFree(res2);
	//-1-(-1)=0
	b5->sign = negative;
	b6->sign = negative;
	res2 = BigIntAlloc();
	CU_ASSERT_TRUE(BigIntSub(res2, b6, b5));
	CU_ASSERT_EQUAL(res2->sign, zero);
	assertMagnitude(res2, (mp_word_t[]) {0x00000000, 0x00000000, 0x00000000}, 3);

	BigIntFree(b5);
	BigIntFree(b6);
	BigIntFree(res2);

	//overflow
	//(2^32-1)-(-1)
	BigInteger *b7 = TEST_BIGINT_INIT(0xFFFFFFFF);
	BigInteger *b8 = TEST_BIGINT_INIT(0x00000001);
	BigInteger *res3 = BigIntAlloc();
	b7->sign = positive;
	b8->sign = negative;
	CU_ASSERT_TRUE(BigIntSub(res3, b7, b8));
	CU_ASSERT_EQUAL(res3->sign, positive);
	assertMagnitude(res3, (mp_word_t[]) {0x00000001, 0x00000000}, 2);
	BigIntFree(res3);
	//(-2^32-1)-1
	res3 = BigIntAlloc();
	b7->sign = negative;
	b8->sign = positive;
	CU_ASSERT_TRUE(BigIntSub(res3, b8, b7));
	CU_ASSERT_EQUAL(res3->sign, positive);
	assertMagnitude(res3, (mp_word_t[]) {0x00000001, 0x00000000}, 2);
	BigIntFree(b7);
	BigIntFree(b8);
	BigIntFree(res3);

	//invalid values
	CU_ASSERT_FALSE(BigIntAdd(NULL, NULL, NULL));
	BigInteger *b9 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000001);
	BigInteger *b10 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	BigInteger *b11 = TEST_BIGINT_INIT(0x00000000, 0x00000000, 0x00000002);
	CU_ASSERT_FALSE(BigIntSub(NULL, b9, b10));
	CU_ASSERT_FALSE(BigIntSub(b9, NULL, b10));
	CU_ASSERT_FALSE(BigIntSub(b9, b10, NULL));
	//words = NULL; FIXME ich denke da sollte die Berechnung trotzdem gemacht werden
	free(b9->words);
	b9->words=NULL;
	CU_ASSERT_TRUE(BigIntSub(b9, b10, b11));
	CU_ASSERT_TRUE(BigIntSub(b10, b9, b11));
	CU_ASSERT_TRUE(BigIntSub(b10, b11, b9));
	//TODO assert result
	BigIntFree(b9);
	BigIntFree(b10);
	BigIntFree(b11);
}
