///
/// \file
/// \brief Unit tests for implementation specific details of the big integers.
///
/// This file can be used to test internal details of your own big-integer
/// library, which are not part of the public API.
#include "tests.h"

// Include the private implementation details the big integer library.
#include "tinybn_imp.h"

/// \todo Add test for specific implementation details here, and register them
///   int he BIGINT_INTERNAL_TESTS array below. See the DemoDummyTest for
///   an example.

// A simple "dummy" demo test
static void DemoDummyTest(void)
{
  // Just assert that universe is sane, i.e. that one is not zero.
  // (your tests should probably do something more useful)
  CU_ASSERT(1 != 0);
}

CU_TestInfo BIGINT_INTERNAL_TESTS[] = {
      /// \todo List your own tests, which depend on implementations
      ///    details of your API here.
      { "demo.dummy", DemoDummyTest },

      // End of list marker (keep this one!)
      CU_TEST_INFO_NULL,
};
