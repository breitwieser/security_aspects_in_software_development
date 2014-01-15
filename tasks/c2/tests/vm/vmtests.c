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

  // Simple programs
    {
      .pName       = "vm/security",
      .pInitFunc    = &TestSuiteInit,
      .pCleanupFunc = &TestSuiteCleanup,
      .pTests = (CU_TestInfo[]) {
        { "udivby0.le", &TestVmExecUdivBy0LittleEndian },
        { "sdivby0.le", &TestVmExecSdivBy0LittleEndian },
        { "sdiv_overflow.le", &TestVmExecSdivOverflowLittleEndian },
        { "mjmp.le",  &TestVmExecMJmpLittleEndian },
        { "aprot.le", &TestVmExecAProtLittleEndian },
        { "w2ro_var.be", &TestVmExecW2ROVarLittleEndian },

        CU_TEST_INFO_NULL,
      }
    },
  // End of virtual machine test suites
  CU_SUITE_INFO_NULL
};
