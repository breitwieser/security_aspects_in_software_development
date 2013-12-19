///
/// \file
/// \brief Common header for the tinyvm virtual machine unit tests.
///
#ifndef VM_TESTS_H
#define VM_TESTS_H

// Common test helpers
#include "testhelpers.h"

// Public include file for the TinyVM assembler API.
#include "tinyvm.h"

// Simple test programs
int TestExecSimple(const char *name, size_t max_stack,
                   const void *input, size_t input_len,
                   const void *output, size_t output_len);

void TestVmExecHelloBigEndian(void);
void TestVmExecHelloLittleEndian(void);
void TestVmExecRot13BigEndian(void);
void TestVmExecRot13LittleEndian(void);

#endif // VM_TESTS_H
