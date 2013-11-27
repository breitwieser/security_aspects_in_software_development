///
/// \file
/// \brief Unit tests for integer comparison
///
#include "tests.h"


//----------------------------------------------------------------------
static bool CheckMpCmp(int result,
                       mp_word_t a2, mp_word_t a1, mp_word_t a0,
                       mp_word_t b2, mp_word_t b1, mp_word_t b0)
{
  const mp_word_t a[3] = { a0, a1, a2 };
  const mp_word_t b[3] = { b0, b1, b2 };

  int ret = MpCompare(a, 3, b, 3);
  return (result == ret);
}

//----------------------------------------------------------------------
#define CHECK_MP_CMP(expected,a0,a1,a2,b0,b1,b2)                        \
  CU_ASSERT_TRUE(CheckMpCmp(expected,                                   \
                            MP_WORD_C(a0), MP_WORD_C(a1), MP_WORD_C(a2), \
                            MP_WORD_C(b0), MP_WORD_C(b1), MP_WORD_C(b2)));


//----------------------------------------------------------------------
void TestMpCompareSimple(void)
{
  // Test 0 == 0
  CHECK_MP_CMP(0,
               0x00000000,  0x00000000, 0x00000000,
               0x00000000,  0x00000000, 0x00000000);

  // Test 1 > 0
  CHECK_MP_CMP(1,
               0x00000000,  0x00000000, 0x00000001,
               0x00000000,  0x00000000, 0x00000000);

  // Test 0 < 1
  CHECK_MP_CMP(-1,
               0x00000000,  0x00000000, 0x00000000,
               0x00000000,  0x00000000, 0x00000001);

  // Test 2^32 > 1
  CHECK_MP_CMP(1,
               0x00000000,  0x00000001, 0x00000000,
               0x00000000,  0x00000000, 0x00000001);

  // Test 1 < 2^32
  CHECK_MP_CMP(-1,
               0x00000000,  0x00000000, 0x00000001,
               0x00000000,  0x00000001, 0x00000000);

  // Test 2^64 > 1
  CHECK_MP_CMP(1,
               0x00000001,  0x00000000, 0x00000000,
               0x00000000,  0x00000000, 0x00000001);

  // Test 1 < 2^64
  CHECK_MP_CMP(-1,
               0x00000000,  0x00000000, 0x00000001,
               0x00000001,  0x00000000, 0x00000001);

  // Test 2^64 > 2^32
  CHECK_MP_CMP(1,
               0x00000001,  0x00000000, 0x00000000,
               0x00000000,  0x00000001, 0x00000000);

  // Test 2^32 < 2^64
  CHECK_MP_CMP(-1,
               0x00000000,  0x00000001, 0x00000000,
               0x00000001,  0x00000000, 0x00000000);

  // Test (2^96-1) > 0
  CHECK_MP_CMP(1,
               0xFFFFFFFF,  0xFFFFFFFF, 0xFFFFFFFF,
               0x00000000,  0x00000000, 0x00000000);

  // Test 0 < (2^96-1)
  CHECK_MP_CMP(-1,
               0x00000000,  0x00000000, 0x00000000,
               0xFFFFFFFF,  0xFFFFFFFF, 0xFFFFFFFF);
}