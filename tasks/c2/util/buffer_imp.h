///
/// \file
/// \brief Implementation details of the byte buffer abstraction.
///
/// \note This header file contains any implementation details
///  of your byte buffer. You can put declarations of
///  internal helper functions, type and structure definitions,
///  constants, etc. in this file.
///
#ifndef TINYVM_BUFFER_IMP_H
#define TINYVM_BUFFER_IMP_H 1

#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Implementation details of your byte buffer.
struct Buffer {
  /// \todo Add the implementation details of your byte buffer.
	unsigned char* data;
	size_t len;
};

/// \todo Add your own private helper functions, constants, ...
///  to this file.

#endif // TINYVM_BUFFER_IMP_H
