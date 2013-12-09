///
/// \file
/// \brief Common header for the big integer unit tests.
///
#ifndef ASSEMBLER_TESTS_H
#define ASSEMBLER_TESTS_H

// Common test helpers
#include "testhelpers.h"

// Public include file for the big integer API.
#include "tinybn.h"

// Unit tests for low-level multi-precision addition/subtraction (MpAdd, MpSub)
void TestMpAddSimple(void);
void TestMpSubSimple(void);
void TestMpAddVarLen(void);
void TestMpSubVarLen(void);
void TestMpAddNull(void);
void TestMpSubNull(void);
void TestMpAddBadDest(void);
void TestMpSubBadDest(void);
void TestMpAddBadSource(void);
void TestMpSubBadSource(void);

// Unit tests for low-level multi-precision multiplication (MpMul)
void TestMpMulSimple(void);
void TestMpMulVarLen(void);
void TestMpMulNull(void);
void TestMpMulLarge(void);
void TestMpMulBadDest(void);
void TestMpMulBadSource(void);

// Unit tests for low-level multi-precision compares (MpCompare)
void TestMpCompareSimple(void);
void TestMpCompareZero(void);

// Common big-integer unit test helpers (in test_bigint_common.c)
BigInteger* TestHelperBigIntInit(size_t len, const mp_word_t *init_values);

// Convenience wrapper macro for TestBigIntInit
#define TEST_BIGINT_INIT(...)                                           \
  (__extension__ ({                                                     \
      static const mp_word_t __init_value[] = {  __VA_ARGS__ };         \
      static const size_t __init_length = sizeof(__init_value) / sizeof(__init_value[0]); \
      TestHelperBigIntInit(__init_length, __init_value);                \
    }))

// Unit tests for basic big integer functionaliy
void TestBigIntAllocSimple(void);
void TestBigIntAllocNoMem(void);
void TestBigIntInitSimple(void);
void TestBigIntFreeNull(void);

// Unit tests for the big-integer I/O API
void TestBigIntLoadSimple(void);
void TestBigIntLoadShort(void);
void TestBigIntLoadLeadingZero(void);
void TestBigIntSaveSimple(void);
void TestBigIntSaveTruncate(void);
void TestBigIntSaveExtend(void);
void TestBigIntGetSet(void);
void TestBigIntLoadNull(void);
void TestBigIntSaveNull(void);
void TestBigIntGetNull(void);
void TestBigIntSetNull(void);

// Unit tests for the internal details of the big-integer API
// (the array is defined in test_bigint_internals.c)
extern CU_TestInfo BIGINT_INTERNAL_TESTS[];

#endif // ASSEMBLER_TESTS_H
