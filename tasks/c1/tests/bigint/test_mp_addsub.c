///
/// \file
/// \brief Unit tests for addition and subtraction.
///
#include "tests.h"

//----------------------------------------------------------------------
static bool CheckMpAdd(bool z_carry,
                       mp_word_t z2, mp_word_t z1, mp_word_t z0,
                       mp_word_t a2, mp_word_t a1, mp_word_t a0,
                       mp_word_t b2, mp_word_t b1, mp_word_t b0)
{
  const mp_word_t a[3] = { a0, a1, a2 };
  const mp_word_t b[3] = { b0, b1, b2 };
  mp_word_t z[3];

  memset(z, 0xA5, sizeof(z));

  bool carry = MpAdd(z, a, 3, b, 3);
  return (z_carry == carry) && (z[0] == z0) &&
    (z[1] == z1) && (z[2] == z2);
}

//----------------------------------------------------------------------
#define CHECK_MP_ADD(z_carry,z2,z1,z0,a2,a1,a0,b2,b1,b0)                \
  CU_ASSERT_TRUE(CheckMpAdd(z_carry,                                    \
                            MP_WORD_C(z2), MP_WORD_C(z1), MP_WORD_C(z0), \
                            MP_WORD_C(a2), MP_WORD_C(a1), MP_WORD_C(a0), \
                            MP_WORD_C(b2), MP_WORD_C(b1), MP_WORD_C(b0)));

//----------------------------------------------------------------------
static bool CheckMpSub(bool z_borrow,
                       mp_word_t z2, mp_word_t z1, mp_word_t z0,
                       mp_word_t a2, mp_word_t a1, mp_word_t a0,
                       mp_word_t b2, mp_word_t b1, mp_word_t b0)
{
  const mp_word_t a[3] = { a0, a1, a2 };
  const mp_word_t b[3] = { b0, b1, b2 };
  mp_word_t z[3];

  memset(z, 0xA5, sizeof(z));

  bool borrow = MpSub(z, a, 3, b, 3);
  return (z_borrow == borrow) && (z[0] == z0) &&
    (z[1] == z1) && (z[2] == z2);
}

//----------------------------------------------------------------------
#define CHECK_MP_SUB(z_borrow,z2,z1,z0,a2,a1,a0,b2,b1,b0)                \
  CU_ASSERT_TRUE(CheckMpSub(z_borrow,                                    \
                            MP_WORD_C(z2), MP_WORD_C(z1), MP_WORD_C(z0), \
                            MP_WORD_C(a2), MP_WORD_C(a1), MP_WORD_C(a0), \
                            MP_WORD_C(b2), MP_WORD_C(b1), MP_WORD_C(b0)));


//----------------------------------------------------------------------
void TestMpAddSimple(void)
{
  // Simple addition test cases to verify basic fucntionality and
  // handling of carries.

  // Add 0 + 0 = 0
  CHECK_MP_ADD(false,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000);

  // Add 0 + 1 = 1
  CHECK_MP_ADD(false,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000001);
  CHECK_MP_ADD(false,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000000);

  // Add (0x1234 * 2^80 + 0x9876 * 2^32 + 0xABCD * 2^16)
  //  +  (0x3210 * 2^64 + 0x6789 * 2^48 + 0xDCBA)
  CHECK_MP_ADD(false,
               0x12343210, 0x67899876, 0xABCDDCBA,
               0x12340000, 0x00009876, 0xABCD0000,
               0x00003210, 0x67890000, 0x0000DCBA);


  // Add 0 + 2^32 = 2^32
  CHECK_MP_ADD(false,
               0x00000000, 0x00000001, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000001, 0x00000000);
  CHECK_MP_ADD(false,
               0x00000000, 0x00000001, 0x00000000,
               0x00000000, 0x00000001, 0x00000000,
               0x00000000, 0x00000000, 0x00000000);

  // Add 0 + 2^64 = 2^64
  CHECK_MP_ADD(false,
               0x00000001, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000001, 0x00000000, 0x00000000);
  CHECK_MP_ADD(false,
               0x00000001, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000001, 0x00000000, 0x00000000);

 // Add 0 + 2^63 = 2^63
  CHECK_MP_ADD(false,
               0x00000000, 0x80000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x80000000, 0x00000000);
  CHECK_MP_ADD(false,
               0x00000000, 0x80000000, 0x00000000,
               0x00000000, 0x80000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000);

  // Add 0 + 2^95 = 2^95
  CHECK_MP_ADD(false,
               0x80000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x80000000, 0x00000000, 0x00000000);
  CHECK_MP_ADD(false,
               0x80000000, 0x00000000, 0x00000000,
               0x80000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000);

  // Add 2^31 + 2^31 = 2^32
  CHECK_MP_ADD(false,
               0x00000000, 0x00000001, 0x00000000,
               0x00000000, 0x00000000, 0x80000000,
               0x00000000, 0x00000000, 0x80000000);

  // Add 2^63 + 2^63 = 2^64
  CHECK_MP_ADD(false,
               0x00000001, 0x00000000, 0x00000000,
               0x00000000, 0x80000000, 0x00000000,
               0x00000000, 0x80000000, 0x00000000);

  // Add 2^95 + 2^95 = 2^96
  // We expect a carry from the top word and result zero.
  CHECK_MP_ADD(true,
               0x00000000, 0x00000000, 0x00000000,
               0x80000000, 0x00000000, 0x00000000,
               0x80000000, 0x00000000, 0x00000000);

  // Add (2^95 - 2^64 + 2^31) + (2^63 - 2^32 + 2^31) = (2^95 - 2^64 + 2^63)
  //
  // The overall results fits into 96-bits, thus no carray from the
  // top word. There is one intermediate carry, when adding 0x80000000
  // in word 0.
  CHECK_MP_ADD(false,
               0x7FFFFFFF, 0x80000000, 0x00000000,
               0x7FFFFFFF, 0x00000000, 0x80000000,
               0x00000000, 0x7FFFFFFF, 0x80000000);
  CHECK_MP_ADD(false,
               0x7FFFFFFF, 0x80000000, 0x00000000,
               0x00000000, 0x7FFFFFFF, 0x80000000,
               0x7FFFFFFF, 0x00000000, 0x80000000);

  // Add: 0xDEADC0DEDEAFBEEFCAFEBABE + 0xFEEDBACCF00BAAAA12345678
  //    = (2^96 + 0xDD9B7BABCEBB6999DD331136) mod 2^96
  //
  // We expect a carry in the top word
  CHECK_MP_ADD(true,
               0xDD9B7BAB, 0xCEBB6999, 0xDD331136,
               0xDEADC0DE, 0xDEAFBEEF, 0xCAFEBABE,
               0xFEEDBACC, 0xF00BAAAA, 0x12345678);
}

//----------------------------------------------------------------------
void TestMpSubSimple(void)
{
  // Simple subtraction test cases to verify basic fucntionality and
  // handling of borrows.

  // Subtract 0 - 0 = 0
  CHECK_MP_SUB(false,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000);

  // Subtract 1 - 0 = 0
  CHECK_MP_SUB(false,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000000);

  // Subtract (0x1234 * 2^80 + 0x9876 * 2^32 + 0xABCD * 2^16)
  //       -  (0x3210 * 2^64 + 0x6789 * 2^48 + 0xDCBA)
  // (and vice versa)
  CHECK_MP_SUB(false,
               0x1233CDEF, 0x98779876, 0xABCC2346,
               0x12340000, 0x00009876, 0xABCD0000,
               0x00003210, 0x67890000, 0x0000DCBA);

  CHECK_MP_SUB(true,
               0xEDCC3210, 0x67886789, 0x5433DCBA,
               0x00003210, 0x67890000, 0x0000DCBA,
               0x12340000, 0x00009876, 0xABCD0000);


  // Subtract 2^32 - 1 = (2^32 - 1)
  CHECK_MP_SUB(false,
               0x00000000, 0x00000000, 0xFFFFFFFF,
               0x00000000, 0x00000001, 0x00000000,
               0x00000000, 0x00000000, 0x00000001);

  // Subtract 2^64 - 1 = (2^64 - 1)
  CHECK_MP_SUB(false,
               0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
               0x00000001, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000001);

  // Subtract 2^64 - (2^32 + 1) = (2^64 - 2^32 - 1)
  CHECK_MP_SUB(false,
               0x00000000, 0xFFFFFFFE, 0xFFFFFFFF,
               0x00000001, 0x00000000, 0x00000000,
               0x00000000, 0x00000001, 0x00000001);

  // Subtract 2^63 - (2^32 + 2^11 - 5) = (2^64 - 2^32 - 2^11 + 5)
  CHECK_MP_SUB(false,
               0x00000000, 0x7FFFFFFE, 0xFFFFF805,
               0x00000000, 0x80000000, 0x00000000,
               0x00000000, 0x00000001, 0x000007FB);

  // Subtract 1 - 2^31 = (2^96 + 1 - 2^31) mod (2^96)
  // We expect a borrow from the top word
  CHECK_MP_SUB(true,
               0xFFFFFFFF, 0xFFFFFFFF, 0x80000001,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x80000000);

  // Subtract 0 - 1 = (2^96 - 1) mod (2^96)
  // We expect a borrow from the top word.
  CHECK_MP_SUB(true,
               0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000001);

 // Add: 0xDEADC0DEDEAFBEEFCAFEBABE - 0xFEEDBACCF00BAAAA12345678
  //    = (2^96 - 0x203FF9EE115BEBBA47359BBA) mod 2^96
  //    = 0xDFC00611EEA41445B8CA6446
  //
  // We expect a borrow in the top word
  CHECK_MP_SUB(true,
               0xDFC00611, 0xEEA41445, 0xB8CA6446,
               0xDEADC0DE, 0xDEAFBEEF, 0xCAFEBABE,
               0xFEEDBACC, 0xF00BAAAA, 0x12345678);
}

//----------------------------------------------------------------------
void TestMpAddVarLen(void)
{
  // Addition with two different length arrays
  // (the shorter array is assumed to be zero extended)

  // a = 2^32 + (2^32 - 1)
  static const mp_word_t A[2] = {
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0x00000001)
  };

  // b = 2^31
  static const mp_word_t B[1] = {
    MP_WORD_C(0x80000000)
  };

  // Add A and B
  mp_word_t z0[2];
  memset(z0, 0xA5, sizeof(z0));
  CU_ASSERT_FALSE(MpAdd(z0, A, 2, B, 1));   // Add without carry
  CU_ASSERT(z0[0] == MP_WORD_C(0x7FFFFFFF));
  CU_ASSERT(z0[1] == MP_WORD_C(0x00000002));

  // Add B and A
  mp_word_t z1[2];
  memset(z1, 0xA5, sizeof(z1));
  CU_ASSERT_FALSE(MpAdd(z1, B, 1, A, 2));   // Add without carry
  CU_ASSERT(z1[0] == MP_WORD_C(0x7FFFFFFF));
  CU_ASSERT(z1[1] == MP_WORD_C(0x00000002));
}

//----------------------------------------------------------------------
void TestMpSubVarLen(void)
{
  // Subtraction with two different length arrays
  // (the shorter array is assumed to be zero extended)

  // a = 2^32 + (2^32 - 1)
  static const mp_word_t A[2] = {
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0x00000001)
  };

  // b = 2^31
  static const mp_word_t B[1] = {
    MP_WORD_C(0x80000000)
  };

  // Add A and B
  mp_word_t z0[2];
  memset(z0, 0x5A, sizeof(z0));
  CU_ASSERT_FALSE(MpSub(z0, A, 2, B, 1));   // Subtract (no borrow expected)
  CU_ASSERT(z0[0] == MP_WORD_C(0x7FFFFFFF));
  CU_ASSERT(z0[1] == MP_WORD_C(0x00000001));

  // Add B and A
  mp_word_t z1[2];
  memset(z1, 0x5A, sizeof(z1));
  CU_ASSERT_TRUE(MpSub(z1, B, 1, A, 2));   // Subtract (borrow expected)
  CU_ASSERT(z1[0] == MP_WORD_C(0x80000001));
  CU_ASSERT(z1[1] == MP_WORD_C(0xFFFFFFFE));
}

//----------------------------------------------------------------------
void TestMpAddNull(void)
{
  // Addition with a zero operand

  // a = 0xDEADC0DE12345678
  static const mp_word_t A[2] = {
    MP_WORD_C(0x12345678),
    MP_WORD_C(0xDEADC0DE)
  };

  // Add A + 0 (zero expressed as NULL array of length zero)
  mp_word_t z0[2];
  memset(z0, 0xAA, sizeof(z0));
  CU_ASSERT_FALSE(MpAdd(z0, A, 2, NULL, 0)); // Add (no carry expected)
  CU_ASSERT(z0[0] == MP_WORD_C(0x12345678));
  CU_ASSERT(z0[1] == MP_WORD_C(0xDEADC0DE));

  // Add 0 + A (zero expressed as NULL array of length zero)
  mp_word_t z1[2];
  memset(z1, 0xAA, sizeof(z1));
  CU_ASSERT_FALSE(MpAdd(z1, NULL, 0, A, 2)); // Add (no carry expected)
  CU_ASSERT(z1[0] == MP_WORD_C(0x12345678));
  CU_ASSERT(z1[1] == MP_WORD_C(0xDEADC0DE));
}

//----------------------------------------------------------------------
void TestMpSubNull(void)
{
  // Subtraction with a zero operand

  // a = 0xDEADC0DE12345678
  static const mp_word_t A[2] = {
    MP_WORD_C(0x12345678),
    MP_WORD_C(0xDEADC0DE)
  };

  // Subtract A - 0 (zero expressed as NULL array of length zero)
  mp_word_t z0[2];
  memset(z0, 0xCA, sizeof(z0));
  CU_ASSERT_FALSE(MpSub(z0, A, 2, NULL, 0)); // Subtract (no borrow expected)
  CU_ASSERT(z0[0] == MP_WORD_C(0x12345678));
  CU_ASSERT(z0[1] == MP_WORD_C(0xDEADC0DE));

  // Subtract 0 - A (zero expressed as NULL array of length zero)
  mp_word_t z1[2];
  memset(z1, 0xFE, sizeof(z1));
  CU_ASSERT_TRUE(MpSub(z1, NULL, 0, A, 2)); // Subtract (borrow expected)
  CU_ASSERT(z1[0] == MP_WORD_C(0xEDCBA988));
  CU_ASSERT(z1[1] == MP_WORD_C(0x21523F21));
}

//----------------------------------------------------------------------
void TestMpAddBadDest(void)
{
  // Addition with bad destination operand (should behave as no-operation)

  // a = 2^32 + (2^32 - 1)
  static const mp_word_t A[2] = {
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0x00000001)
  };

  // b = 2^31
  static const mp_word_t B[1] = {
    MP_WORD_C(0x80000000)
  };

  CU_ASSERT_FALSE(MpAdd(NULL, A, 2, B, 1));
}

//----------------------------------------------------------------------.
void TestMpSubBadDest(void)
{
  // Subtraction with bad destination operand (should behave as no-operation)

  // a = 2^32 + (2^32 - 1)
  static const mp_word_t A[2] = {
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0x00000001)
  };

  // b = 2^31
  static const mp_word_t B[1] = {
    MP_WORD_C(0x80000000)
  };

  CU_ASSERT_FALSE(MpSub(NULL, A, 2, B, 1));
}

//----------------------------------------------------------------------
void TestMpAddBadSource(void)
{
  // Addition with bad source and/or destination operands.
  // (This testcase must complete without SEGFAULTs or aborts)

  // a = 0xDEADC0DE12345678
  static const mp_word_t A[2] = {
    MP_WORD_C(0x12345678),
    MP_WORD_C(0xDEADC0DE)
  };


  // Test with bad destination operand (should behave as no-op)
  CU_ASSERT_FALSE(MpAdd(NULL, A, 2, NULL, 3));
  CU_ASSERT_FALSE(MpAdd(NULL, NULL, 3, A, 2));
  CU_ASSERT_FALSE(MpAdd(NULL, NULL, 2, NULL, 3));

  // Test with valid destination operand
  //
  // The API specification does not specify explicitly rule out implementations,
  // which treat NULL arrays of non-zero length as zero values.
  //
  // We expect one of the following two behaviors here:
  //
  //  1. Treat a NULL array of non-zero length as invalid argument. In this
  //     case MpAdd should behave as no-op. For this variant, the returned carry
  //     value should always be false.
  //
  // -OR-
  //
  //  2. Treat a NULL array of non-zero length as zero value. In this case
  //     MpAdd should behave as normal addition with zero. For this variant,
  //     the returned carry value will always be false.
  //
  mp_word_t z0[3];

  CU_ASSERT_FALSE(MpAdd(z0, A, 2, NULL, 3));
  CU_ASSERT_FALSE(MpAdd(z0, NULL, 3, A, 2));
  CU_ASSERT_FALSE(MpAdd(z0, NULL, 2, NULL, 3));
}

//----------------------------------------------------------------------
void TestMpSubBadSource(void)
{
  // Subtraction with bad source and/or destination operands.
  // (This testcase must complete without SEGFAULTs or aborts)

  // a = 0xDEADC0DE12345678
  static const mp_word_t A[2] = {
    MP_WORD_C(0x12345678),
    MP_WORD_C(0xDEADC0DE)
  };

  // Test with bad destination operand (should behave as no-op)
  CU_ASSERT_FALSE(MpSub(NULL, A, 2, NULL, 3));
  MpSub(NULL, NULL, 3, A, 2); // Borrow depends on implementation
  CU_ASSERT_FALSE(MpSub(NULL, NULL, 2, NULL, 3));

  // Test with valid destination operand
  //
  // The API specification does not specify explicitly rule out implementations,
  // which treat NULL arrays of non-zero length as zero values.
  //
  // We expect one of the following two behaviors here:
  //
  //  1. Treat a NULL array of non-zero length as invalid argument. In this
  //     case MpSub should behave as no-op. For this variant, the returned borrow
  //     value should always be false.
  //
  // -OR-
  //
  //  2. Treat a NULL array of non-zero length as zero value. In this case
  //     MpSub should behave as normal addition with zero. For this variant, the
  //     returned borrow value will be false if the second operand is zero (x - 0).
  //
  mp_word_t z0[3];

  CU_ASSERT_FALSE(MpSub(z0, A, 2, NULL, 3));
  MpSub(z0, NULL, 3, A, 2); // Borrow depends on implementation
  CU_ASSERT_FALSE(MpSub(z0, NULL, 2, NULL, 3));
}

