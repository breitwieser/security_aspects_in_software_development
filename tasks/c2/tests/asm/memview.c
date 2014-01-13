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

  uint32_t result = 0x0D0C0B0A;
  // Append some initialized data
  static const unsigned char LITTLE_ENDIAN[4] = {0x0A , 0x0B, 0x0C, 0x0D};
  static const unsigned char BIG_ENDIAN[4] = {0x0D , 0x0C, 0x0B, 0x0A};
  static const unsigned char ZERO[4] = {0x0 , 0x0, 0x0, 0x0};
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, LITTLE_ENDIAN, 4));

  MemView mem;
  MemInit(&mem, MEM_VIEW_NORMAL, buffer.storage, 0, 4);
  uint32_t z;
  MemGetWord(&z, &mem, 0);
  CU_ASSERT_EQUAL(z, result);
  z = 0;
  AsmBufferClear(&buffer);
  AsmBufferAppend(&buffer, BIG_ENDIAN, 4);
  MemInit(&mem, MEM_VIEW_BIGENDIAN, buffer.storage, 0, 4);
  MemGetWord(&z, &mem, 0);
  CU_ASSERT_EQUAL(z, result);

  AsmBufferClear(&buffer);
  AsmBufferAppend(&buffer, ZERO, 4);
  MemInit(&mem, MEM_VIEW_NORMAL, buffer.storage, 0, 4);
  MemSetWord(&mem, 0, result);

  MemGetWord(&z, &mem, 0);

  CU_ASSERT_EQUAL(z, result);

  z = 0;
  AsmBufferClear(&buffer);
  AsmBufferAppend(&buffer, ZERO, 4);
  MemInit(&mem, MEM_VIEW_BIGENDIAN, buffer.storage, 0, 4);
  MemSetWord(&mem, 0, result);

  MemGetWord(&z, &mem, 0);

  CU_ASSERT_EQUAL(z, result);
  // Cleanup
  AsmBufferClear(&buffer);
}
