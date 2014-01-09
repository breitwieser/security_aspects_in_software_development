///
/// \file
/// \brief Simple programs
///
#include "tests.h"

// Embedded resource lookup (generated by bin2res.pl, see _resources.c)
const void* EmbeddedResourceLookup(size_t *length, const char *name);

//----------------------------------------------------------------------
typedef struct TestSimpleAppData {
  /// \brief Input data (for VM_READ)
  const unsigned char *input;

  /// \brief Input data length (for VM_READ)
  size_t input_len;

  /// \brief Expected output data (for VM_WRITE)
  const unsigned char *output;

  /// \brief Expected output data length (for VM_READ)
  size_t output_len;

  /// \brief Checks if output still matches
  bool output_matches;
} TestSimpleAppData;

//----------------------------------------------------------------------
static int TestSimpleIoRead(VmContext *vm)
{
  TestSimpleAppData *app = VmGetAppData(vm);
  if (!app) {
    CU_FAIL_FATAL("VmGetAppData is not working");
  }

  // Assume end of input
  int ret = -2;
  if (app->input_len > 0) {
    ret = *app->input;
    app->input += 1;
    app->input_len -= 1;
  }

  return ret;
}

//----------------------------------------------------------------------
static bool TestSimpleIoWrite(VmContext *vm, unsigned char c)
{
  TestSimpleAppData *app = VmGetAppData(vm);
  if (!app) {
    CU_FAIL_FATAL("VmGetAppData is not working");
  }

  // Detect an output mismatch (only report the first one)
  if (app->output_matches) {
    if (app->output_len == 0) {
      // No more output expected
      CU_FAIL("unexpected extra output produced by bytecode program");
      app->output_matches = false;

    } else {
      // No mismatch found yet, compare this
      if (*app->output++ != c) {
        CU_FAIL("bytecode program output does not match expected output");
        app->output_matches = false;
      }

      app->output_len -= 1;
    }
  }

  // We can sink any number of characters
  return true;
}

//----------------------------------------------------------------------
int TestExecSimple(const char *name, size_t max_stack,
                   const void *input, size_t input_len,
                   const void *output, size_t output_len)
{
  // Lookup the resource
  size_t res_length = 0;
  const void *res_data = EmbeddedResourceLookup(&res_length, name);
  CU_ASSERT_PTR_NOT_NULL_FATAL(res_data);

  // Setup our context data
  TestSimpleAppData testdata = {
    .input      = input,
    .input_len  = input_len,
    .output     = output,
    .output_len = output_len,
    .output_matches = true,
  };

  // Populate a temporary buffer with the resource data, then load
  Buffer *bytecode = BufferCreate();
  CU_ASSERT_PTR_NOT_NULL_FATAL(bytecode);
  CU_ASSERT_TRUE_FATAL(BufferAppend(bytecode, res_data, res_length));

  // Create a VM and load the bytecode
  VmContext *vm = VmCreate(max_stack, &testdata,
                           TestSimpleIoRead,
                           TestSimpleIoWrite);
  CU_ASSERT_PTR_NOT_NULL_FATAL(vm);
  CU_ASSERT_TRUE_FATAL(VmLoadByteCode(vm, bytecode));

  // Discard the input buffer
  BufferFree(bytecode);

  // Main execute loop
  int ret;
  do {
    ret = VmStep(vm);
  } while (ret == 0);

  // Input must have been consumed (fully)
  CU_ASSERT(testdata.input_len == 0);

  // Output must match (fully)
  CU_ASSERT(testdata.output_matches && testdata.output_len == 0);

  // Destroy the VM
  VmDelete(vm);

  // Return the exec result
  return ret;
}

//----------------------------------------------------------------------
void TestVmExecHelloBigEndian(void)
{
  static const char HELLO_WORLD[13] = "Hello World!\n";

  // Try the big endian bytecode version
  CU_ASSERT(1 == TestExecSimple("bin/hello.bin.be", 256,
                                NULL, 0,
                                HELLO_WORLD, sizeof(HELLO_WORLD)));
}

//----------------------------------------------------------------------
void TestVmExecHelloLittleEndian(void)
{
  static const char HELLO_WORLD[13] = "Hello World!\n";

  // Try the little endian bytecode version
  CU_ASSERT(1 == TestExecSimple("bin/hello.bin.le", 256,
                                NULL, 0,
                                HELLO_WORLD, sizeof(HELLO_WORLD)));
}


//----------------------------------------------------------------------
void TestVmExecRot13BigEndian(void)
{
  static const char INPUT[]  = "Uryyb sebz shmml'f iveghny jbeyq.";
  static const char OUTPUT[] = "Hello from fuzzy's virtual world.";

  // Try the big endian bytecode version
  CU_ASSERT(1 == TestExecSimple("bin/rot13.bin.be", 256,
                                INPUT, strlen(INPUT),
                                OUTPUT, strlen(OUTPUT)));
}

//----------------------------------------------------------------------
void TestVmExecRot13LittleEndian(void)
{
  static const char INPUT[]  = "Uryyb sebz shmml'f iveghny jbeyq.";
  static const char OUTPUT[] = "Hello from fuzzy's virtual world.";

  // Try the big endian bytecode version
  CU_ASSERT(1 == TestExecSimple("bin/rot13.bin.le", 256,
                                INPUT, strlen(INPUT),
                                OUTPUT, strlen(OUTPUT)));
}
