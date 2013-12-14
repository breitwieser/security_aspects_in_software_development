///
/// \file
/// \brief Basic test suite definitions for the tiny big integer library.
///
#include "tests.h"

//----------------------------------------------------------------------
CU_SuiteInfo UNIT_TEST_SUITES[] = {
  // Unit tests for low-level multi-precision addition (MpAdd)
  {
    .pName        = "mp/addsub",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "add.simple", TestMpAddSimple },
      { "sub.simple", TestMpSubSimple },
      { "add.varlen", TestMpAddVarLen },
      { "sub.varlen", TestMpSubVarLen },
      { "add.null",   TestMpAddNull },
      { "sub.null",   TestMpSubNull },
      { "add.bad.dest", TestMpAddBadDest },
      { "sub.bad.dest", TestMpSubBadDest },
      { "add.bad.source", TestMpAddBadSource },
      { "sub.bad.dest", TestMpSubBadSource },
      CU_TEST_INFO_NULL,
    }
  },

  // Unit tests for low-level multi-precision multiplication (MpMul)
  {
    .pName        = "mp/mul",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "mul.simple", TestMpMulSimple },
      { "mul.varlen", TestMpMulVarLen },
      { "mul.null",   TestMpMulNull },
      { "mul.large",  TestMpMulLarge },
      { "mul.bad.dest", TestMpMulBadDest },
      { "mul.bad.source", TestMpMulBadSource },
      CU_TEST_INFO_NULL,
    }
  },

  // Unit tests for low-level multi-precision compares (MpCompare)
  {
    .pName        = "mp/compare",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "cmp.simple", TestMpCompareSimple },
      { "cmp.zero",   TestMpCompareZero },
      { "cmp.task3",   TestMpCompareTask3 },
      CU_TEST_INFO_NULL,
    }
  },

  // Unit tests for basic big-integer functionality
  {
    .pName        = "bigint/basic",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "alloc.simple", TestBigIntAllocSimple },
      { "alloc.nomem",  TestBigIntAllocNoMem  },
      { "init.simple",  TestBigIntInitSimple },
      { "free.null",    TestBigIntFreeNull },
      { "alloc.task3",    TestBigIntAllocTask3 },
      CU_TEST_INFO_NULL,
    }
  },

  // Unit tests for the big-integer I/O API
  {
    .pName        = "bigint/io",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "load.simple", TestBigIntLoadSimple },
      { "load.short", TestBigIntLoadShort },
      { "load.zero", TestBigIntLoadLeadingZero },
      { "save.simple", TestBigIntSaveSimple },
      { "save.truncate", TestBigIntSaveTruncate },
      { "save.extend", TestBigIntSaveExtend },
      { "getset.simple", TestBigIntGetSet },
      { "load.null", TestBigIntLoadNull },
      { "save.null", TestBigIntSaveNull },
      { "get.null", TestBigIntGetNull },
      { "set.null", TestBigIntSetNull },
      { "io.task3", TestBigIntIOTask3 },
      CU_TEST_INFO_NULL,
    }
  },

  // Unit tests for the internal details of the big-integer I/O API
  {
    .pName      = "bigint/internal",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,

    // The test are defined in test_bigint_internal.c
    .pTests = BIGINT_INTERNAL_TESTS,
  },

  // Unit tests for basic big-integer functionality
  {
    .pName        = "bigint/compare",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
     { "test.sign", TestBigIntSgn },
     { "compare.simple", TestBigIntCompareSimple },
     CU_TEST_INFO_NULL,
    }
  },

  {
    .pName        = "bigint/misc",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "misc.copy", TestBigIntCopy },
      { "misc.neg", TestBigIntNeg },
      { "misc.abs", TestBigIntAbs },
      CU_TEST_INFO_NULL,
     }
   },

   {
     .pName        = "bigint/addsub",
     .pInitFunc    = &TestSuiteInit,
     .pCleanupFunc = &TestSuiteCleanup,
     .pTests = (CU_TestInfo[]) {
       { "addsub.add", TestBigIntAdd },
       { "addsub.sub", TestBigIntAdd },
       CU_TEST_INFO_NULL,
      }
    },

    {
      .pName        = "bigint/mult",
      .pInitFunc    = &TestSuiteInit,
      .pCleanupFunc = &TestSuiteCleanup,
      .pTests = (CU_TestInfo[]) {
        { "mult.mult", TestBigIntMult },
        CU_TEST_INFO_NULL,
       }
     },
  // End of test suites
  CU_SUITE_INFO_NULL
};

//----------------------------------------------------------------------
int main(int argc, char **argv)
{
  return TestMain(argc, argv);
}
