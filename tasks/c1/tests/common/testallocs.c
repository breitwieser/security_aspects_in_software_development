///
/// Memory allocation testing (GNU libc specific)
///
#define _GNU_SOURCE 1

#include "testhelpers.h"
#include <stdlib.h>

// These functions are GNU libc specific aliases for malloc,
// realloc and free.
//
// We intercept the standard malloc/calloc/free/realloc symbols and
// delegate to the GNU libc variants for the actual allocations.
extern __typeof (malloc)  __libc_malloc;
extern __typeof (calloc)  __libc_calloc;
extern __typeof (realloc) __libc_realloc;
extern __typeof (free)    __libc_free;

/// Current allocation size limit.
///
/// A value of zero causes every allocation to fail. A limit of SIZE_MAX
/// (default) effectively disables the limit and allows every allocation.
static size_t gAllocSizeLimit = SIZE_MAX;

/// Current allocation fault counter.
static size_t gAllocFaultCounter = 0;

//----------------------------------------------------------------------
void TestSetAllocSizeLimit(size_t limit)
{
  gAllocSizeLimit = limit;
}

//----------------------------------------------------------------------
void TestSetAllocFaultCountdown(size_t counter)
{
  gAllocFaultCounter = counter;
}

//----------------------------------------------------------------------
void TestNoAllocFaults(void)
{
  gAllocSizeLimit = SIZE_MAX;
  gAllocFaultCounter = 0;
}

//----------------------------------------------------------------------
static bool TestAllocShouldFail(size_t size)
{
  // Evaluate the allocation fault
  if (gAllocFaultCounter > 0) {
    // Counter is active
    gAllocFaultCounter -= 1;

    // Fail the allocation when the counter reaches zero
    if (gAllocFaultCounter == 0) {
      return true;
    }
  }

  // Fail if size limit has been exceeded
  if (gAllocSizeLimit < size) {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
__attribute__((visibility("hidden"))) void* malloc(size_t size)
{
  // Evaluate the error trigger
  if (TestAllocShouldFail(size)) {
    return NULL;
  }

  // Delegate to libc implementation of malloc
  void *ptr = __libc_malloc(size);
  return ptr;
}

//----------------------------------------------------------------------
__attribute__((visibility("hidden"))) void* calloc(size_t nmemb, size_t size)
{
  // Evaluate the error trigger
  if (TestAllocShouldFail(size)) {
    return NULL;
  }

  // Delegate to libc implementation of malloc
  void *ptr = __libc_calloc(nmemb, size);
  return ptr;
}

//----------------------------------------------------------------------
__attribute__((visibility("hidden"))) void* realloc(void *old_ptr, size_t size)
{
  // Evaluate the error trigger
  if (TestAllocShouldFail(size)) {
    return NULL;
  }

  // Delegate to libc implementation of realloc
  void *new_ptr = __libc_realloc(old_ptr, size);
  return new_ptr;
}

//----------------------------------------------------------------------
__attribute__((visibility("hidden"))) void free(void *ptr)
{
  // Delegate to libc implementation of free
  __libc_free(ptr);
}
