///
/// \file
/// \brief Unit tests for low-level multi-precision multiplication (MpMul)
///
#include "tests.h"

//----------------------------------------------------------------------
static bool CheckMpMul3x3(mp_word_t z5, mp_word_t z4, mp_word_t z3,
                          mp_word_t z2, mp_word_t z1, mp_word_t z0,
                          mp_word_t a2, mp_word_t a1, mp_word_t a0,
                          mp_word_t b2, mp_word_t b1, mp_word_t b0)
{
  const mp_word_t a[3] = { a0, a1, a2 };
  const mp_word_t b[3] = { b0, b1, b2 };
  mp_word_t z[6];

  memset(z, 0xA5, sizeof(z));

  MpMul(z, a, 3, b, 3);
  return  (z[0] == z0) && (z[1] == z1) && (z[2] == z2) &&
    (z[3] == z3) && (z[4] == z4) && (z[5] == z5);
}

//----------------------------------------------------------------------
#define CHECK_MP_MUL(z5,z4,z3,z2,z1,z0,a2,a1,a0,b2,b1,b0)               \
  CU_ASSERT_TRUE(CheckMpMul3x3(MP_WORD_C(z5), MP_WORD_C(z4), MP_WORD_C(z3), \
                               MP_WORD_C(z2), MP_WORD_C(z1), MP_WORD_C(z0), \
                               MP_WORD_C(a2), MP_WORD_C(a1), MP_WORD_C(a0), \
                               MP_WORD_C(b2), MP_WORD_C(b1), MP_WORD_C(b0)));

//----------------------------------------------------------------------
void TestMpMulSimple(void)
{
  // Simple multiplication tests

  // Multiply: 0 x 0 = 0
  CHECK_MP_MUL(0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000);

  // Multiply: 0 x 1 = 0
  CHECK_MP_MUL(0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000001);

  CHECK_MP_MUL(0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000000);

  // Multiply: 1 x 1 = 1
  CHECK_MP_MUL(0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000001,
               0x00000000, 0x00000000, 0x00000001);

  // Multiply: (2^64 + 2^31) x 2 = (2^65 + 2^32)
  CHECK_MP_MUL(0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000001, 0x00000000,
               0x00000001, 0x00000000, 0x80000000,
               0x00000000, 0x00000000, 0x00000002);

  CHECK_MP_MUL(0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000001, 0x00000000,
               0x00000000, 0x00000000, 0x00000002,
               0x00000001, 0x00000000, 0x80000000);

  // Multiply: (2^96 - 1) x (2^96 - 1) = (2^192 - 2^97 + 1)
  //         = 0xxFFFFFFFFFFFFFFFFFFFFFFFE000000000000000000000001
  CHECK_MP_MUL(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0x00000000, 0x00000000, 0x00000001,
               0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
               0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

  // Multiply: 0xDEADC0DEDEAFBEEFCAFEBABE x 0xFEEDBACCF00BAAAA12345678
  //         = 0xDDBF2EB530AB32465FD2322E6DD849DD2CD561A404BB5D10
  CHECK_MP_MUL(0xDDBF2EB5, 0x30AB3246, 0x5FD2322E, 0x6DD849DD, 0x2CD561A4, 0x04BB5D10,
               0xDEADC0DE, 0xDEAFBEEF, 0xCAFEBABE,
               0xFEEDBACC, 0xF00BAAAA, 0x12345678);
}

//----------------------------------------------------------------------
void TestMpMulVarLen(void)
{
  // Multiplication with two different length operands

  // a = 2^64 + (2^37 - 2^33) + (2^32 - 1) = 0x10000001EFFFFFFFF
  static const mp_word_t A[3] = {
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0x0000001E),
    MP_WORD_C(0x00000001)
  };

  // b = 2^51 + (2^16 - 2^11) = 0x800000000F800
  static const mp_word_t B[2] = {
    MP_WORD_C(0x0000F800),
    MP_WORD_C(0x00080000)
  };

  // Multiply A*B
  mp_word_t z0[5];
  memset(z0, 0xEF, sizeof(z0));
  MpMul(z0, A, 3, B, 2);
  CU_ASSERT(z0[0] == MP_WORD_C(0xFFFF0800));
  CU_ASSERT(z0[1] == MP_WORD_C(0x001607FF));
  CU_ASSERT(z0[2] == MP_WORD_C(0x00F8F800));
  CU_ASSERT(z0[3] == MP_WORD_C(0x00080000));
  CU_ASSERT(z0[4] == MP_WORD_C(0x00000000));

  // Multiply B*A
  mp_word_t z1[5];
  memset(z1, 0xAD, sizeof(z1));
  MpMul(z1, B, 2, A, 3);
  CU_ASSERT(z0[0] == MP_WORD_C(0xFFFF0800));
  CU_ASSERT(z0[1] == MP_WORD_C(0x001607FF));
  CU_ASSERT(z0[2] == MP_WORD_C(0x00F8F800));
  CU_ASSERT(z0[3] == MP_WORD_C(0x00080000));
  CU_ASSERT(z0[4] == MP_WORD_C(0x00000000));
}

//----------------------------------------------------------------------
void TestMpMulLarge(void)
{
  // Multiply two large numbers of different length

  // a = 2^96 - 1
  static const mp_word_t A[3] = {
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0xFFFFFFFF)
  };

  // b = 2^192 - 1
  static const mp_word_t B[6] = {
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0xFFFFFFFF),
    MP_WORD_C(0xFFFFFFFF)
  };

  // Multiply A*B
  mp_word_t z0[9];
  memset(z0, 0xDE, sizeof(z0));
  MpMul(z0, A, 3, B, 6);
  CU_ASSERT(z0[0] == MP_WORD_C(0x00000001));
  CU_ASSERT(z0[1] == MP_WORD_C(0x00000000));
  CU_ASSERT(z0[2] == MP_WORD_C(0x00000000));
  CU_ASSERT(z0[3] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z0[4] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z0[5] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z0[6] == MP_WORD_C(0xFFFFFFFE));
  CU_ASSERT(z0[7] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z0[8] == MP_WORD_C(0xFFFFFFFF));

  // Multiply B*A
  mp_word_t z1[9];
  memset(z1, 0xED, sizeof(z1));
  MpMul(z1, B, 6, A, 3);
  CU_ASSERT(z1[0] == MP_WORD_C(0x00000001));
  CU_ASSERT(z1[1] == MP_WORD_C(0x00000000));
  CU_ASSERT(z1[2] == MP_WORD_C(0x00000000));
  CU_ASSERT(z1[3] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z1[4] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z1[5] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z1[6] == MP_WORD_C(0xFFFFFFFE));
  CU_ASSERT(z1[7] == MP_WORD_C(0xFFFFFFFF));
  CU_ASSERT(z1[8] == MP_WORD_C(0xFFFFFFFF));
}

//----------------------------------------------------------------------
void TestMpMulNull(void)
{
  // Multiplication with zero (zero expressed as NULL array of zero length)

  // a = 0xFEDCBA9876543210
  static const mp_word_t A[2] = {
    MP_WORD_C(0x76543210),
    MP_WORD_C(0xFEDCBA98)
  };

  // Multiply A * 0
  mp_word_t z0[2];
  memset(z0, 0xAB, sizeof(z0));
  MpMul(z0, A, 2, NULL, 0);
  CU_ASSERT(z0[0] == MP_WORD_C(0x00000000));
  CU_ASSERT(z0[1] == MP_WORD_C(0x00000000));

  // Multiply 0 * A
  mp_word_t z1[2];
  memset(z1, 0xCD, sizeof(z1));
  MpMul(z1, NULL, 0, A, 2);
  CU_ASSERT(z1[0] == MP_WORD_C(0x00000000));
  CU_ASSERT(z1[1] == MP_WORD_C(0x00000000));
}

//----------------------------------------------------------------------
void TestMpMulBadDest(void)
{
  // Multiplication with bad destination operand

  // a = 0xFEDCBA9876543210
  static const mp_word_t A[2] = {
    MP_WORD_C(0x76543210),
    MP_WORD_C(0xFEDCBA98)
  };

  // b = 0x0123456789ABCDEF
  static const mp_word_t B[2] = {
    MP_WORD_C(0x01234567),
    MP_WORD_C(0x89ABCDEF)
  };

  // Multiply with NULL destination (should behave as no-operation)
  //
  // This (z=NULL) is the only case of bad destination pointer which
  // we can detect reliably and portably.
  MpMul(NULL, A, 2, B, 2);

  // NOTE: The following commented-out code snippet illustrates an undetecable
  // API misuse. There is NO reliable and/or portable way to detect the "writable"
  // size of an arbitrary C pointer ...
  //
  // mp_word_t too_small[1];
  // MpMul(too_small, A, 2, B, 2); // UNDETECTABLE API MISUSE
}

//----------------------------------------------------------------------
void TestMpMulBadSource(void)
{
  // Multiplication with bad source operand(2)

  // a = 0xFEDCBA9876543210
  static const mp_word_t A[2] = {
    MP_WORD_C(0x76543210),
    MP_WORD_C(0xFEDCBA98)
  };

  // Test for (detectable) instances of API misuse with NULL arrays and
  // non-zero size.
  //
  // All of these operations should behave as no-operation, since the destination
  // operand is NULL.
  MpMul(NULL, A, 2, NULL, 3);
  MpMul(NULL, NULL, 3, A, 2);

  // Test with valid destination operand
  //
  // The API specification does not specify explicitly rule out implementations,
  // which treat NULL arrays of non-zero length as zero values.
  //
  // We expect one of the following two behaviors here:
  //
  //  1. Treat a NULL array of non-zero length as invalid argument. In this
  //     case MpMul should behave as no-op.
  //
  // -OR-
  //
  //  2. Treat a NULL array of non-zero length as zero value. In this case
  //     MpSub should behave as normal multiplication with zero.
  //
  mp_word_t bad_result[5];

  memset(bad_result, 0, sizeof(bad_result)); // Force to zero (for variant 1)
  MpMul(bad_result, A, 2, NULL, 3);
  CU_ASSERT(bad_result[0] == MP_WORD_C(0x00000000)); // Valid for variant 1 and 2
  CU_ASSERT(bad_result[1] == MP_WORD_C(0x00000000)); // -"-
  CU_ASSERT(bad_result[2] == MP_WORD_C(0x00000000)); // -"-
  CU_ASSERT(bad_result[3] == MP_WORD_C(0x00000000)); // -"-
  CU_ASSERT(bad_result[4] == MP_WORD_C(0x00000000)); // -"-

  memset(bad_result, 0, sizeof(bad_result)); // Force to zero (for variant 1)
  MpMul(bad_result, NULL, 3,  A, 2);
  CU_ASSERT(bad_result[0] == MP_WORD_C(0x00000000)); // Valid for variant 1 and 2
  CU_ASSERT(bad_result[1] == MP_WORD_C(0x00000000)); // -"-
  CU_ASSERT(bad_result[2] == MP_WORD_C(0x00000000)); // -"-
  CU_ASSERT(bad_result[3] == MP_WORD_C(0x00000000)); // -"-
  CU_ASSERT(bad_result[4] == MP_WORD_C(0x00000000)); // -"-
}
