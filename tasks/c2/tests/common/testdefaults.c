///
/// Default test-suite cleanup and tear-down.
///
#include "testhelpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


/// \brief Previous SIGSEGV handler (saved by \ref TestSuiteInit, 
///   restored by \ref TestSuiteCleanup)
static struct sigaction g_old_segv_handler;

/// \brief Previous SIGFPE handler (saved by \ref TestSuiteInit, 
///   restored by \ref TestSuiteCleanup)
static struct sigaction g_old_fpe_handler;

/// \brief Indicates that the old SIGSEGV handler is valid.
static bool g_old_segv_valid = false;

/// \brief Indicates that the old SIGFPE handler is valid.
static bool g_old_fpe_valid = false;

//----------------------------------------------------------------------
static void TestSuiteSigSegV(int signo, siginfo_t *info, void *ctx)
{
  // Flush standard error and standard output
  fprintf(stderr, 
	  "\n\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	  "\t!!!! TEST FAILED WITH SEGMENTATION VIOLATION  !!!!\n"
	  "\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  // Unblock SIGSEGV (to re-arm the handler)
  //
  // NOTE: Not race-free if SIGSEGV is delivered between sigprocmask
  // and the setjmp/longjmp pair used by CU_FAIL_FATAL. (This is the
  // best we can achieve without patching libcunit).
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGSEGV);
  sigprocmask(SIG_UNBLOCK, &sigs, NULL);

  // Fail the test (CUnit uses setjmp/longjmp to implement the 
  // failed fatal assertions, which allows us to recover from signals)
  CU_FAIL_FATAL("!!! TEST FAILED WITH SEGMENTATION VIOLATION !!!");
}

//----------------------------------------------------------------------
static void TestSuiteSigFPE(int signo, siginfo_t *info, void *ctx)
{
  // Flush standard error and standard output
  fprintf(stderr, 
	  "\n\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
	  "\t!!!! TEST FAILED WITH FLOATING POINT EXCEPTION !!!\n"
	  "\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  // Unblock SIGFPE (to re-arm the handler)
  //
  // NOTE: Similar race-condition reasoning as for SIGSEGV above.
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGFPE);
  sigprocmask(SIG_UNBLOCK, &sigs, NULL);

  // Fail the test (CUnit uses setjmp/longjmp to implement the 
  // failed fatal assertions, which allows us to recover from signals)
  CU_FAIL_FATAL("!!! TEST FAILED WITH FLOATING POINT EXCEPTION !!!");
}

//----------------------------------------------------------------------
static bool TestSuiteSetSignalHandler(struct sigaction *old, 
				      int signo,
				      void (*handler)(int, siginfo_t*, void*))
{
  // Setup the signal handler
  struct sigaction act = {
    .sa_flags = SA_SIGINFO,
    .sa_sigaction = handler
  };
 
  sigemptyset(&act.sa_mask);  // Do not block any other signals

  if (sigaction(signo, &act, old) != 0) {
    perror("warning: failed to install signal handler");
    return false;
  }
  
  return true;
}

//----------------------------------------------------------------------
static void TestSuiteRestoreSignalHandler(int signo, const struct sigaction *old)
{
  if (sigaction(signo, old, NULL) != 0) {
    perror("warning: failed to restore old signal handler");
  }
}

//----------------------------------------------------------------------
int TestSuiteInit(void)
{
  if (TestCatchSignals()) {
    // Install signal handlers (GDB nicely interacts with them)
    g_old_segv_valid = TestSuiteSetSignalHandler(&g_old_segv_handler, SIGSEGV, &TestSuiteSigSegV);
    g_old_fpe_valid = TestSuiteSetSignalHandler(&g_old_fpe_handler, SIGFPE, &TestSuiteSigFPE);

  } else {
    // Running on valgrind (let valgrind report the signals)
    g_old_segv_valid = false;
    g_old_fpe_valid = false;
  }

  // Clear all allocation fault triggers
  TestNoAllocFaults();
  return 0;
}

//----------------------------------------------------------------------
int TestSuiteCleanup(void)
{
  // Disable any (residual) allocation fault triggers
  TestNoAllocFaults();

  // Restore the old signal handlers
  if (g_old_fpe_valid) {
    TestSuiteRestoreSignalHandler(SIGFPE, &g_old_fpe_handler);
    g_old_fpe_valid = false;
  }

  if (g_old_segv_valid) {
    TestSuiteRestoreSignalHandler(SIGSEGV, &g_old_segv_handler);
    g_old_segv_valid = false;
  }

  return 0;
}
