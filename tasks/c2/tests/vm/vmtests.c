///
/// Test suite definitions for the tinyvm virtual machine.
///
#include "tests.h"

//----------------------------------------------------------------------
CU_SuiteInfo UNIT_TEST_SUITES[] = {
  // Simple programs
  {
    .pName       = "vm/exec",
    .pInitFunc    = &TestSuiteInit,
    .pCleanupFunc = &TestSuiteCleanup,
    .pTests = (CU_TestInfo[]) {
      { "hello.be", &TestVmExecHelloBigEndian },
      { "hello.le", &TestVmExecHelloLittleEndian },
      { "rot13.be", &TestVmExecRot13BigEndian },
      { "rot13.le", &TestVmExecRot13LittleEndian },
      CU_TEST_INFO_NULL,
    }
  },

  // End of virtual machine test suites
  CU_SUITE_INFO_NULL
};
