///
/// \file
/// \brief IAIK's unit test runner
///
/// This program implements a simple unit test running for
/// executing CUnit unit tests either in interactive console
/// (default) or automated mode.
///
#include "testhelpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <CUnit/Automated.h> // CU_automated_run_tests()
#include <CUnit/Console.h>   // CU_console_run_tests()
#include <CUnit/Basic.h>     // CU_basic_run_tests()

/// \brief Enable automated testing mode.
static bool g_auto_test_enabled = false;

/// \brief Automated tests results prefix for \c CU_set_output_filename
static const char *g_auto_test_prefix = NULL;

/// \brief Enable basic testing mode.
static bool g_basic_test_enabled = false;

/// \brief Debug mode control.
static bool g_debug_mode = false;

/// \brief Quiet mode
static bool g_quiet_mode = false;

//----------------------------------------------------------------------
static void TestShowUsage(const char *progname)
{
  fprintf(stderr, "Usage: %s -a[<prefix>]] [-d] [-l] [<test1> [... <testN>]]\n"
	  "-a[<prefix>]     - Enables automated testing mode. (<prefix> is\n"
	  "                   the optional name prefix for the output log\n"
	  "                   file, the default is 'CUnitAutomated')\n"
	  "-b               - Enables basic (non-interactive) testing mode\n"
	  "-q               - Quiet mode (less output)\n"
	  "-l               - List all test-suites and test-cases (without\n"
	  "                   running anything)\n"
	  "-d               - Do not catch SIGSEGV and SIGFPE signales\n"
	  "                   (default: catch these signals and try to\n"
	  "                    continue with further tests.)\n"
	  "\n"
	  "The optional <testX> parameters selectively enable or disable\n"
	  "test-suites and test-cases. The default (if no <testX> parameters\n"
	  "are given) is to enable all test-cases in all test-suites.\n"
	  "\n"
	  "Syntax for <testX> parameters:\n"
	  "  <suite>         - Enable all tests in test-suite <suite>\n"
	  "  <suite>:[+]<test> - Enable test-case <test> in test-suite <suite>\n"
	  "  <suite>:-<test> - Disable test-case <test> in test-suite <suite>\n"
	  "\n"
	  "The <testX> parameters are processed in command-line order. Later\n"
	  "parameters override earlier ones.\n",
	  progname);
}

//----------------------------------------------------------------------
static bool TestEnableSelection(const char *suite_name, const char *test_name)
{
  CU_pTestRegistry registry = CU_get_registry();

  // Grab the suite
  CU_pSuite suite = CU_get_suite_by_name(suite_name, registry);
  if (!suite) {
    fprintf(stderr, "ERROR: no test-suite with name '%s' exists in the registry.\n",
	    suite_name);
    return false;
  }

  // Always activate the suite
  CU_set_suite_active(suite, CU_TRUE);

  if (test_name == NULL) {
    // Enable all test in this suite (if no test name is given)
    for (CU_pTest test = suite->pTest; test != NULL; test = test->pNext) {
      CU_set_test_active(test, CU_TRUE);
    }

  } else {
    // First character may be '+' or '-'
    CU_BOOL active = CU_TRUE;
    if (*test_name == '+' || *test_name == '-') {
      active = (*test_name == '+') ? CU_TRUE : CU_FALSE;
      test_name += 1;
    }

    // Enable an individual test
    CU_pTest test = CU_get_test_by_name(test_name, suite);
    if (test == NULL) {
      fprintf(stderr, "ERROR: no test-case with name '%s' exists in suite '%s'\n",
	      test_name, suite_name);
      return false;
    }

    CU_set_test_active(test, active);
  }


  return true;
}

//----------------------------------------------------------------------
static bool TestParseSelection(size_t arg_count, char **args)
{
  // First disable all test in all test-suites
  CU_pTestRegistry registry = CU_get_registry();
  for (CU_pSuite suite = registry->pSuite; suite != NULL; suite = suite->pNext) {
    CU_set_suite_active(suite, CU_FALSE);

    for (CU_pTest test = suite->pTest; test != NULL; test = test->pNext) {
      CU_set_test_active(test, CU_FALSE);
    }
  }

  // Inactive tests are NOT considered as errors
  // (this allows simple automated testing of subsets of the test-cases)
  CU_set_fail_on_inactive(CU_FALSE);

  // Now process the selection in order
  for (size_t n = 0; n < arg_count; ++n) {
    // First part is the test-suite
    char *suite = args[n];

    // The part after the ':' is the (optional) test-case
    char *test = strchr(suite, ':');
    if (test != NULL) {
      *test = '\0';
      test += 1;
    }

    // And enable the selection
    if (!TestEnableSelection(suite, test)) {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------
static void TestShowSuites(void)
{
  CU_pTestRegistry registry = CU_get_registry();
  for (CU_pSuite suite = registry->pSuite; suite != NULL; suite = suite->pNext) {
    printf("suite: %s\n", suite->pName);

    for (CU_pTest test = suite->pTest; test != NULL; test = test->pNext) {
      printf("  test: %s\n", test->pName);
    }
  }
}

//----------------------------------------------------------------------
static bool TestParseCmdLine(int argc, char **argv)
{
  int c;

  // Parse command line parameters
  while ((c = getopt(argc, argv, "ha::bdlq")) != -1) {
    switch (c) {
    case 'a': // Prefix for automated testing mode
      g_auto_test_prefix = optarg;
      g_auto_test_enabled = true;
      break;

    case 'b':
      g_basic_test_enabled = true;
      break;

    case 'd': // Enable debug mode
      g_debug_mode = true;
      break;

    case 'q':
      g_quiet_mode = true;
      break;

    case 'l': // List test and suites
      TestShowSuites();
      return false;

    case 'h': // Show command line help
      TestShowUsage(argv[0]);
      return false;

    default:
      fprintf(stderr, "ERROR: See '%s -h' for command line usage.\n", argv[0]);
      return false;
    }
  }

  // Basic and automated testing mode can not be enabled at the same time
  if (g_auto_test_enabled && g_basic_test_enabled) {
    fprintf(stderr, "ERROR: Basic (-b) and automated (-a) test mode are mutually exclusive.\n");
    return false;
  }

  // Parse the test-suit selection (if given)
  if (optind < argc) {
    if (!TestParseSelection(argc - optind, argv + optind)) {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------
static bool RunTests(void)
{
  if (g_debug_mode && !g_quiet_mode) {
    fprintf(stderr, "INFO: debug support mode is enabled. "
	    "(no signal handlers will be installed)\n");
  }

  // Run the tests
  if (g_auto_test_enabled) {
    // Automated test run
    if (g_auto_test_prefix) {
      CU_set_output_filename(g_auto_test_prefix);
    }

    CU_automated_run_tests();

  } else if (g_basic_test_enabled) {
    // Non-interactive basic test mode    
    CU_basic_set_mode(g_quiet_mode ? CU_BRM_SILENT : CU_BRM_VERBOSE);
    CU_basic_run_tests();

  } else {
    // Interactive console mode (default)
    CU_console_run_tests();
  }

  return CU_get_number_of_failure_records() == 0;
}

//----------------------------------------------------------------------
bool TestCatchSignals(void)
{
  return !g_debug_mode;
}

//----------------------------------------------------------------------
int TestMain(int argc, char **argv)
{
  bool success = false;

  // Initialize the test registry
  if (CU_initialize_registry() != CUE_SUCCESS) {
    fprintf(stderr, "ERROR: Failed to initialize CUnit test registry.");
    return EXIT_FAILURE;
  }

  // Test suite registration
  success = (CU_register_suites(UNIT_TEST_SUITES) == CUE_SUCCESS);
  if (!success) {
    fprintf(stderr, "ERROR: Test suite registration failed.");
    success = false;
  }

  // Process the command line (and enable/disable suites as needed)
  if (success) {
    success = TestParseCmdLine(argc, argv);
  }

  // Test execution
  if (success) {
    success = RunTests();
    if (!success) {
      fprintf(stderr, "ERROR: Test execution failed with errors.\n");
    }
  }

  // Cleanup the test registry
  CU_cleanup_registry();

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Provide a weak alias to the main function.
int main(int argc, char **argv) __attribute__((weak, alias("TestMain")));
