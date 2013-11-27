///
/// \file
/// \brief Common test helpers.
///
#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H 1

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>

//----------------------------------------------------------------------
/// Test suites to be executed by the \ref TestMain.
///
/// This array defines the test suites to be registered by \ref TestMain
/// when a test bench is started.
///
/// To implement a simple test bench it is sufficient to create a C source
/// file with the definition of the \ref UNIT_TEST_SUITES array and to link
/// the your program against the \c libtesting.a library.
///
extern CU_SuiteInfo UNIT_TEST_SUITES[];

/// Default main function for \c libtesting.a based test benches.
///
/// You normally do not need to call this function when linking
/// against the \c libtesting.a library, as the library provides
/// a (weak) alias from "main" to this function.
///
/// The test suites registerd at startup are defined in \ref UNIT_TEST_SUITES.
int TestMain(int argc, char **argv);

/// Default test suite initialization.
int TestSuiteInit(void);

/// Default test suite cleanup.
int TestSuiteCleanup(void);

//----------------------------------------------------------------------
/// Sets the allocation size limit for the memory fault injector.
///
/// Memory allocation with malloc/realloc will fail if the requested
/// block size is greater or equal larger than the current allocation limit.
///
/// A limit of SIZE_MAX indicates that no
///
/// \param limit The new allocation size limit. Zero indicates that no
///   allocation size limit is in place.
///
void TestSetAllocSizeLimit(size_t limit);

/// Configures the allocation fault countdown.
///
/// The allocation fault countdown decreases each time before call
/// to \c malloc or \c realloc is passed from the testing framework to
/// the C runtime library. An out of memory error is simulated, when
/// the counter transitions from one to zero.
///
/// \param counter New value of the allocation counter. (A value of zero
///   disables the counter)
void TestSetAllocFaultCountdown(size_t counter);

/// Resets all memory fault injector triggers to their default values.
///
/// This function is equivalent to performing the following steps:
/// \li Call \ref TestSetAllocSizeLimit with a limit of \c SIZE_MAX.
/// \li Call \ref TestSetAllocFaultCountdown with a counter value of zero.
void TestNoAllocFaults(void);

//----------------------------------------------------------------------
/// \brief Indicates if the test environment catches \c SIGSEGV and \c SIGFPE.
///
/// This function is normally provided by \c testrunner.c and indicates if the
/// test-suite setup function (\ref TestSuiteInit) and cleanup
/// function (\ref TestSuiteCleanup) should install signal handlers for
/// segmentation faults (\c SIGSEGV) and arithmetic exceptions (\c SIGFPE).
///
bool TestCatchSignals(void);

#endif // TEST_HELPERS_H
