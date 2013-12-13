///
/// \file
/// \brief Basic big integer tests.
///
#include "tests.h"
#include "tinybn_imp.h"

//----------------------------------------------------------------------
void TestBigIntSgn(void)
{
	//Null pointer
	CU_ASSERT_TRUE(BigIntSgn(NULL) == 0);

	//Valid inputs
	BigInteger *b1 = TEST_BIGINT_INIT(0x12345678);
	b1->sign=positive; //TODO müsste das nicht automatisch passieren?
	CU_ASSERT_TRUE(BigIntSgn(b1)==1);
	BigIntFree(b1);

	BigInteger *b2 = TEST_BIGINT_INIT(0x12345678);
	b2->sign=negative;
	CU_ASSERT_TRUE(BigIntSgn(b2) == -1);
	BigIntFree(b2);

//	//FIXME: fatal fail
//	BigInteger *b3 = TEST_BIGINT_INIT(0x00000000, 0x00000000);
//	CU_ASSERT_TRUE(BigIntSgn(b3) == 0);
//	BigIntFree(b3);

}

//----------------------------------------------------------------------
void TestBigIntCompareSimple(void)
{
	BigInteger *b1 = TEST_BIGINT_INIT(0x12345678);
	BigInteger *b2 = TEST_BIGINT_INIT(0x12345678);
	b1->sign=positive;
	b2->sign=positive;
	CU_ASSERT_TRUE(BigIntCompare(b1, b2)==0);
	CU_ASSERT_TRUE(BigIntCompare(b2, b1)==0);
	b1->sign = negative;
	b2->sign = negative;
	CU_ASSERT_TRUE(BigIntCompare(b1, b2) == 0);
	CU_ASSERT_TRUE(BigIntCompare(b2, b1) == 0);
	b1->sign = positive;
	b2->sign = negative;
	CU_ASSERT_TRUE(BigIntCompare(b1, b2) == 1);
	CU_ASSERT_TRUE(BigIntCompare(b2, b1) == -1);

	BigIntFree(b1);
	BigIntFree(b2);

	//compare b3=(-)1 and b4=(-)2
	BigInteger *b3 = TEST_BIGINT_INIT(0x00000000,  0x00000000, 0x00000001);
	BigInteger *b4 = TEST_BIGINT_INIT(0x00000000,  0x00000000, 0x00000002);
	b3->sign=positive;
	b4->sign=positive;
	CU_ASSERT_TRUE(BigIntCompare(b3, b4)==-1);
	CU_ASSERT_TRUE(BigIntCompare(b4, b3)==1);
	b3->sign=negative;
	b4->sign=negative;
	CU_ASSERT_TRUE(BigIntCompare(b3, b4)==1);
	CU_ASSERT_TRUE(BigIntCompare(b4, b3)==-1);
	BigIntFree(b3);
	BigIntFree(b4);

	//invalid input
	CU_ASSERT_TRUE(BigIntCompare(NULL, NULL)==0);
	BigInteger *b_3 = TEST_BIGINT_INIT(0x12345678);
	b_3->sign=positive;
	CU_ASSERT_TRUE(BigIntCompare(b_3, NULL)==0);
	CU_ASSERT_TRUE(BigIntCompare(NULL, b_3)==0);
	BigIntFree(b_3);

	//words=NULL
	BigInteger *b6 = BigIntAlloc();
	free(b6->words);
	b6->words=NULL;
	BigInteger *b7 = TEST_BIGINT_INIT(0x00000001);
	b7->sign=positive;
	CU_ASSERT_TRUE(BigIntCompare(b6, b7)==-1);
	CU_ASSERT_TRUE(BigIntCompare(b7, b6)==1);
	BigIntFree(b6);
	BigIntFree(b7);
}


