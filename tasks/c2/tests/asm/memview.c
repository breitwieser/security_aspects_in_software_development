///
/// \file
/// \brief Unit tests for the assembler memview
///
#include "tests.h"
#include "assembler_imp.h"
#include "util/memview.h" // Test assembler usage of the memview API

//----------------------------------------------------------------------
void TestMemViewCreateSimple(void)
{

  AsmBuffer buffer;

  // Init
  AsmBufferInit(&buffer, true);

  // Empty buffer, must return NULL
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Append some initialized data
  static const unsigned char BLOB_A[8] = "abcdefg";
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, BLOB_A, 8));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 8);

  MemView mem;
  // abcdefg
  // 0123456
  // ---^^^^
  MemInit(&mem, MEM_VIEW_BIGENDIAN, buffer.storage, 3, 4);
  uint8_t z;
  MemGetByte(&z, &mem, 0);
  // abcdefg
  //    0123
  CU_ASSERT_EQUAL(z, 'd');
  // abcdafg
  //    0123
  MemSetByte(&mem, 1, 'a');
  MemGetByte(&z, &mem, 1);
  CU_ASSERT_EQUAL(z, 'a');
  uint16_t c;
  MemGetHalf(&c, &mem, 0);
  // abcdafg
  //    0123
  // "ad" == 24932
  CU_ASSERT_EQUAL(c, 24932);
  MemSetHalf(&mem, 2, 28269);
  // abcdafg
  //    0123
  // abcdamn
  uint32_t d;
  MemGetWord(&d, &mem, 0);
  // "nmad" == 1852662116
  CU_ASSERT_EQUAL(d, 1852662116);
  // Cleanup
  AsmBufferClear(&buffer);
}
