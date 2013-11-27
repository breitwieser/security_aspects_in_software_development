///
/// \file
/// \brief Basic big integer tests.
///
#include "tests.h"

//----------------------------------------------------------------------
void TestBigIntAllocSimple(void)
{
  // Try to allocate and deallocate a big integer
  BigInteger *bn = BigIntAlloc();
  CU_ASSERT_PTR_NOT_NULL_FATAL(bn);

  // Sanity check: Any new big-integer is initialized to zero. We
  // need exactly one word to represent the zero value.
  CU_ASSERT_EQUAL(BigIntGetWordCount(bn), 1);
  CU_ASSERT_EQUAL(BigIntGetAt(bn, 0), MP_WORD_C(0));

  BigIntFree(bn);
}

//----------------------------------------------------------------------
void TestBigIntInitSimple(void)
{
  // Check the TestBigIntInit function and TEST_HELPER_BIGINT_INIT macro
  // are operational. We require them for the entire remaining testsuite.
  //
  // This test exercises the minimum big-integer functionality which is
  // required by most of the other test cases:
  //   * BigIntAlloc/BigIntFree for allocation and deallocation
  //   * BigIntSetAt to initialize big-integers with test data
  //   * BigIntGetAt/BigIntGetWordCount to access the word-level representation
  //     of big-integers (for comparison with expected test results)
  //

  // Single word big-integer
  BigInteger *w1 = TEST_BIGINT_INIT(0x12345678);
  CU_ASSERT_PTR_NOT_NULL_FATAL(w1);

  CU_ASSERT_EQUAL(BigIntGetWordCount(w1), 1);
  CU_ASSERT_EQUAL(BigIntGetAt(w1, 0), MP_WORD_C(0x12345678));

  BigIntFree(w1);

  // Two word big-integer
  BigInteger *w2 = TEST_BIGINT_INIT(0x12345678, 0x9ABCDEF0);
  CU_ASSERT_PTR_NOT_NULL_FATAL(w2);

  CU_ASSERT_EQUAL(BigIntGetWordCount(w2), 2);
  CU_ASSERT_EQUAL(BigIntGetAt(w2, 1), MP_WORD_C(0x12345678));
  CU_ASSERT_EQUAL(BigIntGetAt(w2, 0), MP_WORD_C(0x9ABCDEF0));

  BigIntFree(w2);

  // Three word big integer
  BigInteger *w3 = TEST_BIGINT_INIT(0xDEAFC0DE, 0xFEEDBACC, 0x87654321);
  CU_ASSERT_PTR_NOT_NULL_FATAL(w3);

  CU_ASSERT_EQUAL(BigIntGetWordCount(w3), 3);
  CU_ASSERT_EQUAL(BigIntGetAt(w3, 2), MP_WORD_C(0xDEAFC0DE));
  CU_ASSERT_EQUAL(BigIntGetAt(w3, 1), MP_WORD_C(0xFEEDBACC));
  CU_ASSERT_EQUAL(BigIntGetAt(w3, 0), MP_WORD_C(0x87654321));

  BigIntFree(w3);
}

//----------------------------------------------------------------------
void TestBigIntAllocNoMem(void)
{
  // Enable malloc fault injection (let all allocations fail)
  TestSetAllocSizeLimit(0);

  // Now try to allocate (this will fail due to out of memory)
  BigInteger *bn = BigIntAlloc();

  // Disable malloc fault injection (avoid interference with
  // other testcases in the same testsuite)
  TestNoAllocFaults();

  // And evaluate the results
  CU_ASSERT_PTR_NULL_FATAL(bn);
}

//----------------------------------------------------------------------
void TestBigIntFreeNull(void)
{
  // Try to free the NULL big integer
  BigIntFree(NULL);
}
