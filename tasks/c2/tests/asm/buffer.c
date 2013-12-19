///
/// \file
/// \brief Unit tests for the assembler buffer
///
#include "tests.h"
#include "assembler_imp.h" // Test assembler usage of the buffer API

//----------------------------------------------------------------------
void TestBufferCreateSimple(void)
{
  AsmBuffer buffer;

  // Init
  AsmBufferInit(&buffer, true);

  // Empty buffer, must return NULL
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Append some initialized data
  static const unsigned char BLOB_A[8] = "ab\0cdefg";
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, BLOB_A, 8));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 8);

  unsigned char* data = AsmBufferBytes(&buffer);
  CU_ASSERT_PTR_NOT_NULL_FATAL(data);
  CU_ASSERT_PTR_NOT_EQUAL(data, BLOB_A);
  CU_ASSERT_EQUAL(memcmp(data, BLOB_A, 8), 0);

  // Cleanup
  AsmBufferClear(&buffer);
}

//----------------------------------------------------------------------
void TestBufferClearZero(void)
{
  // Clear zero-initialized buffer structure
  AsmBuffer buffer;

  // Clear the buffer structure with memset ...
  memset(&buffer, 0, sizeof(buffer));

  // ... try to free the cleared structure.
  AsmBufferClear(&buffer);
}

//----------------------------------------------------------------------
void TestBufferAppendEmpty(void)
{
  AsmBuffer buffer;

  // Init
  AsmBufferInit(&buffer, true);

  // Try to append zero bytes (data non-NULL)
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, "test", 0));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Try to append zero bytes (data is NULL)
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, NULL, 0));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Try to skip zero bytes
  CU_ASSERT_TRUE(AsmBufferSkip(&buffer, 1, 0));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Cleanup
  AsmBufferClear(&buffer);
}


//----------------------------------------------------------------------
void TestBufferAppendGrowing(void)
{
  AsmBuffer buffer;

  // Init
  AsmBufferInit(&buffer, true);

  // Prepare a test pattern
  unsigned char test_pattern[64];
  for (size_t n = 0; n < sizeof(test_pattern); ++n) {
    test_pattern[n] = (unsigned char) n;
  }

  // Step 1: Append the some initial bytes of the test pattern.
  // (data fits into the in-place buffer, one more byte free; no allocation is done)
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, test_pattern, sizeof(test_pattern) - 1));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(test_pattern) - 1);

  unsigned char *inplace1 = AsmBufferBytes(&buffer);
  CU_ASSERT_PTR_NOT_NULL_FATAL(inplace1);
  CU_ASSERT_EQUAL(0, memcmp(inplace1, test_pattern, sizeof(test_pattern) - 1));

  // Step 2: Append the last byte of the test pattern
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, &test_pattern[sizeof(test_pattern) - 1], 1));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(test_pattern));

  unsigned char *inplace2 = AsmBufferBytes(&buffer);
  CU_ASSERT_PTR_NOT_NULL_FATAL(inplace2);
  CU_ASSERT_EQUAL(0, memcmp(inplace2, test_pattern, sizeof(test_pattern)));

  // Step 3: Append some additional bytes.
  static const unsigned char another_pattern[] = "fuzzy strikes back!";
  const size_t new_size = sizeof(test_pattern) + sizeof(another_pattern);

  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, another_pattern, sizeof(another_pattern)));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), new_size);

  unsigned char *heap1 = AsmBufferBytes(&buffer);
  CU_ASSERT_EQUAL(0, memcmp(heap1, test_pattern, sizeof(test_pattern)));
  CU_ASSERT_EQUAL(0, memcmp(heap1 + sizeof(test_pattern), another_pattern,
			    sizeof(another_pattern)));

  // Cleanup
  AsmBufferClear(&buffer);
}


//----------------------------------------------------------------------
void TestBufferAppendBytewise(void)
{
  AsmBuffer buffer;

  // Init
  AsmBufferInit(&buffer, true);

  // Append the test pattern byte-by-byte
  static const unsigned char GENERATOR[] = "pattern of !$\377#\n\0\t!\"$§(! bugs";
  const size_t PATTERN_LENGTH = 96;
  bool pattern_appended = true;
  for (size_t n = 0; pattern_appended && n < PATTERN_LENGTH; ++n) {
    pattern_appended = AsmBufferAppend(&buffer, GENERATOR + (n % sizeof(GENERATOR)), 1);
  }

  // Check that all bytes have been appended
  CU_ASSERT_TRUE(pattern_appended);

  // Check the contents
  unsigned char *buf = AsmBufferBytes(&buffer);
  size_t buf_length = AsmBufferLength(&buffer);
  CU_ASSERT_PTR_NOT_NULL_FATAL(buf);
  CU_ASSERT_EQUAL(buf_length, PATTERN_LENGTH);

  // Compare the content
  bool pattern_equal = true;
  for (size_t n = 0; pattern_equal && n < buf_length; ++n) {
    pattern_equal = (buf[n] == GENERATOR[n % sizeof(GENERATOR)]);
  }
  CU_ASSERT_TRUE(pattern_equal);

  // Cleanup
  AsmBufferClear(&buffer);
}


//----------------------------------------------------------------------
void TestBufferAppendMixed(void)
{
  AsmBuffer buffer;

  // Test in big endian mode
  AsmBufferInit(&buffer, true);

  CU_ASSERT_TRUE(AsmBufferIsBigEndian(&buffer));

  // Append a 16-bit offset and a 32-bit word
  CU_ASSERT_TRUE(AsmBufferAppendOffset(&buffer, 0x1234U));
  CU_ASSERT_TRUE(AsmBufferAppendWord(&buffer, 0x789ABCDEU));

  // Test against expected data
  static const uint8_t expected[] = {
    0x12, 0x34, 0x78, 0x9A, 0xBC, 0xDE
  };

  // And compare
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(expected));
  CU_ASSERT_PTR_NOT_NULL_FATAL(AsmBufferBytes(&buffer));
  CU_ASSERT_EQUAL(memcmp(AsmBufferBytes(&buffer), expected,
			 sizeof(expected)), 0);

  AsmBufferClear(&buffer);
}

//----------------------------------------------------------------------
void TestBufferAppendMixedLittleEndian(void)
{
  AsmBuffer buffer;

  // Test little-endian variant of the buffer
  AsmBufferInit(&buffer, false);

  CU_ASSERT_FALSE(AsmBufferIsBigEndian(&buffer));

  // Append a 24-bit offset and a 32-bit word
  CU_ASSERT_TRUE(AsmBufferAppendOffset(&buffer, 0x1234U));
  CU_ASSERT_TRUE(AsmBufferAppendWord(&buffer, 0x789ABCDEU));

  // Test against expected data
  static const uint8_t expected[] = {
    0x34, 0x12, 0xDE, 0xBC, 0x9A, 0x78
  };

  // And compare
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(expected));
  CU_ASSERT_PTR_NOT_NULL_FATAL(AsmBufferBytes(&buffer));
  CU_ASSERT_EQUAL(memcmp(AsmBufferBytes(&buffer), expected,
			 sizeof(expected)), 0);

  AsmBufferClear(&buffer);
}

//----------------------------------------------------------------------
void TestBufferBadCreateNull(void)
{
  // Try to create the NULL buffer
  AsmBufferInit(NULL, true);
}

//----------------------------------------------------------------------
void TestBufferBadDeleteNull(void)
{
  // Try to delete the NULL buffer
  AsmBufferClear(NULL);
}

//----------------------------------------------------------------------
void TestBufferBadGettersNull(void)
{
  // Try to access buffer properties of the NULL buffer
  CU_ASSERT_PTR_EQUAL(AsmBufferBytes(NULL), NULL);
  CU_ASSERT_EQUAL(AsmBufferLength(NULL), 0);
  CU_ASSERT_FALSE(AsmBufferIsBigEndian(NULL));
}

//----------------------------------------------------------------------
void TestBufferBadAppendNull(void)
{
  AsmBuffer buffer;

  // Init
  AsmBufferInit(&buffer, true);

  // Try to append bytes with NULL data pointer
  CU_ASSERT_FALSE(AsmBufferAppend(&buffer, NULL, 1));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Try to append bytes with NULL data pointer
  CU_ASSERT_FALSE(AsmBufferAppend(&buffer, NULL, 32));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Try to append bytes with NULL data pointer
  CU_ASSERT_FALSE(AsmBufferAppend(&buffer, NULL, 64));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Cleanup
  AsmBufferClear(&buffer);

  // Try to append bytes to the NULL buffer
  CU_ASSERT_FALSE(AsmBufferAppend(NULL, NULL, 1));
  CU_ASSERT_FALSE(AsmBufferAppend(NULL, NULL, 32));
  CU_ASSERT_FALSE(AsmBufferAppend(NULL, NULL, 64));
  CU_ASSERT_FALSE(AsmBufferAppend(NULL, "", 0));
  CU_ASSERT_FALSE(AsmBufferAppend(NULL, "foo", 3));

  // Try to append offset and words to the NULL buffer
  CU_ASSERT_FALSE(AsmBufferAppendOffset(NULL, 0x1234U));
  CU_ASSERT_FALSE(AsmBufferAppendWord(NULL, 0x1234U));

  // Try to skip bytes in the NULL buffer
  CU_ASSERT_FALSE(AsmBufferSkip(NULL, 1, 2));
}

//----------------------------------------------------------------------
void TestBufferBadAppendOverflow(void)
{
  AsmBuffer buffer;

  // Init
  AsmBufferInit(&buffer, true);

  // Append some test data
  static const char test_data[] = "overflows!$%# grrrr!\0 and NULs!";
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, test_data, sizeof(test_data)));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(test_data));
  CU_ASSERT_PTR_NOT_NULL_FATAL(AsmBufferBytes(&buffer));
  CU_ASSERT_EQUAL(0, memcmp(AsmBufferBytes(&buffer), test_data, sizeof(test_data)));

  // Step 1: Now try to append *much* more data ...
  CU_ASSERT_FALSE(AsmBufferAppend(&buffer, test_data + 2, SIZE_MAX));

  // ... buffer must stay unchanged on error!
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(test_data));
  CU_ASSERT_PTR_NOT_NULL_FATAL(AsmBufferBytes(&buffer));
  CU_ASSERT_EQUAL(0, memcmp(AsmBufferBytes(&buffer), test_data, sizeof(test_data)));

  // Step 2: Now try to append *much* more data (but a little bit less) ...
  CU_ASSERT_FALSE(AsmBufferAppend(&buffer, test_data + 3, SIZE_MAX-1));

  // ... buffer must stay unchanged on error!
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(test_data));
  CU_ASSERT_PTR_NOT_NULL_FATAL(AsmBufferBytes(&buffer));
  CU_ASSERT_EQUAL(0, memcmp(AsmBufferBytes(&buffer), test_data, sizeof(test_data)));

  // Step 3: Now try to append *much* more data (but a slightly larger bit less) ...
  CU_ASSERT_FALSE(AsmBufferAppend(&buffer, test_data + 1, SIZE_MAX-sizeof(test_data)+1));

  // ... buffer must stay unchanged on error!
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(test_data));
  CU_ASSERT_PTR_NOT_NULL_FATAL(AsmBufferBytes(&buffer));
  CU_ASSERT_EQUAL(0, memcmp(AsmBufferBytes(&buffer), test_data, sizeof(test_data)));

  // Cleanup
  AsmBufferClear(&buffer);
}


//----------------------------------------------------------------------
void TestBufferBadOutOfMemorySingle(void)
{
  AsmBuffer buffer;

  // Setup out of memory on next allocation
  // (AsmBufferInit must not allocate memory)
  TestSetAllocFaultCountdown(1);

  // Init
  AsmBufferInit(&buffer, true);

  // Try to append twice the inplace block
  unsigned char test_data[64];
  memset(test_data, 42, sizeof(test_data));

  // Setup for out of memory condition:
  //  * At next+1 memory allocation
  //
  // (The first memory allocation will be done by the AsmBufferPrepare
  //  method when setting up the underlying storage buffer. The second
  //  allocation will be done by the storage buffer itself.)
  TestSetAllocFaultCountdown(2);

  // This allocation will fail with out of memory
  CU_ASSERT_FALSE(AsmBufferAppend(&buffer, test_data, sizeof(test_data)));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), 0);
  CU_ASSERT_PTR_NULL(AsmBufferBytes(&buffer));

  // Remove memory pressure (do not generate any more faults)
  TestNoAllocFaults();

  // This allocation will succeed, buffer should work again
  CU_ASSERT_TRUE(AsmBufferAppend(&buffer, test_data, sizeof(test_data)));
  CU_ASSERT_EQUAL(AsmBufferLength(&buffer), sizeof(test_data));
  CU_ASSERT_PTR_NOT_NULL(AsmBufferBytes(&buffer));

  // Cleanup
  AsmBufferClear(&buffer);
}
