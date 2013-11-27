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
