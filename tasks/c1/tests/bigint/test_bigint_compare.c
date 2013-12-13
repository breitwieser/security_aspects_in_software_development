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

	//Invalid inputs
//	p->sign=positive;
//	p->words=NULL;
//	//same behaviour for consecutive calls
//	int ret = BigIntSgn(p);
//	CU_ASSERT_TRUE(ret == BigIntSgn(p));

	//Valid inputs
	BigInteger *b1 = TEST_BIGINT_INIT(0x12345678);
	b1->sign=positive; //TODO müsste das nicht automatisch passieren?
	CU_ASSERT_TRUE(BigIntSgn(b1)==1);
	BigIntFree(b1);

	BigInteger *b2 = TEST_BIGINT_INIT(0x12345678);
	b2->sign=negative;
	CU_ASSERT_TRUE(BigIntSgn(b2) == -1);
	BigIntFree(b2);

	BigInteger *b3 = TEST_BIGINT_INIT(0x0);
	CU_ASSERT_TRUE(BigIntSgn(b3) == 0);
	BigIntFree(b3);

}

//----------------------------------------------------------------------
void TestBigIntCompareSimple(void)
{
	BigInteger *b1 = TEST_BIGINT_INIT(0x12345678);
	BigInteger *b2 = TEST_BIGINT_INIT(0x12345678);
	b1->sign=positive;
	b2->sign=positive;
	CU_ASSERT_TRUE(BigIntCompare(b1, b2)==0);
	b1->sign=positive;
	b2->sign=negative;
	CU_ASSERT_TRUE(BigIntCompare(b1, b2)==1);
	CU_ASSERT_TRUE(BigIntCompare(b2, b1)==-1);
	BigIntFree(b1);
	BigIntFree(b2);

	BigInteger *b3 = TEST_BIGINT_INIT(0x0, 0x1, 0x0);
	BigInteger *b4 = TEST_BIGINT_INIT(0x1, 0x1, 0x0);
	b1->sign=positive;
	b2->sign=positive;
	CU_ASSERT_TRUE(BigIntCompare(b3, b4)==-1);
	CU_ASSERT_TRUE(BigIntCompare(b4, b3)==1);
	b1->sign=negative;
	b2->sign=negative;
	CU_ASSERT_TRUE(BigIntCompare(b3, b4)==1);
	CU_ASSERT_TRUE(BigIntCompare(b4, b3)==-1);
	BigIntFree(b3);
	BigIntFree(b4);

	//invalid input
	CU_ASSERT_TRUE(BigIntCompare(NULL, NULL)==0);
	b1->sign=positive;
	b2->sign=positive;
	BigInteger *b_3 = TEST_BIGINT_INIT(0x12345678);
	b_3->sign=positive;
	CU_ASSERT_TRUE(BigIntCompare(b_3, NULL)==0);
	CU_ASSERT_TRUE(BigIntCompare(NULL, b_3)==0);
}


