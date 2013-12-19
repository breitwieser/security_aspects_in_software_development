///
/// \file
/// \brief Simple byte buffer abstraction.
///
/// \warning
///  This header file defines the public API interface of the tinyvm byte buffer
///  API. It is part of the assignment specification and MUST NOT be changed.
///  Details (declaration of internal functions of your implementation, defines,
///  type definitions, ...) should go into the \c buffer_imp.h header file.
///
#ifndef TINYVM_BUFFER_H
#define TINYVM_BUFFER_H 1

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//----------------------------------------------------------------------
/// \defgroup UtilBuffer Simple memory buffers
/// @{

/// \brief A simple byte buffer.
///
/// Byte buffers provide a simple abstraction for handling raw binary
/// data. The only size limitations of a byte buffer should be the
/// available amount of memory and the intrinsic limits of data-types
/// such as \c size_t.
///
/// A byte buffer may contains arbitrary binary data, so it is necessary
/// to store the length and the base pointer in the byte buffer structure.
typedef struct Buffer Buffer;

/// \brief Creates a new empty byte buffer.
///
/// This function allocated and initializes a new empty byte buffer
/// object.
///
/// \return A new empty byte buffer or NULL on error.
Buffer* BufferCreate(void);

/// \brief Frees a byte buffer and releases all associated resources.
///
/// This function deletes a given byte buffer object and releases all
/// associateds (memory) resources.
///
/// \param[in] buffer The byte buffer to be deleted.
void BufferFree(Buffer* buffer);

/// \brief Appends data to a byte buffer.
///
/// This functions appends data at the end of a byte buffer. The data
/// to be appended is specified by the \c data and \c len parameters.
///
/// \warning The \c data and \c len parameters may refer to a slice of the
///  byte buffer itself. Be careful when growing the buffer! (in particular
///  if you are planning to use realloc). The following code fragment
///  illustrates an example of this situation:
///
///  \code
///  ...
///  ByteBuffer *buffer = ...;
///
///  // Get a slice of the buffer
///  size_t len = BufferGetLength(buffer);
///  void *data = BufferGetBytes(buffer, 0, len);
///
///  // Append the buffer to itself
///  BufferAppend(buffer, data, len);
///  ...
///  \endcode
///
/// \param[in,out] buffer The destination byte buffer.
///
/// \param[in] data Data to be appended at the end of the buffer.
///
/// \param len Length (in bytes) of the data block to be appended.
///
/// \return \c true if the append operation succeeded.
///
/// \return \c false if the append operation failed, for example due
///  to bad arguments or an out of memory condition.
///
///
/// \warning Calling this function (may) invalidates all pointers which
///  have been returned by preceding \ref BufferGetBytes calls.
bool BufferAppend(Buffer* buffer, const void *data, size_t len);

/// \brief Skips bytes at the end of a buffer.
///
/// This functions "skips" a number of bytes at the end of a buffer, by
/// appending the \c filler value \c len times.
///
/// \param[in,out] buffer is the destination byte buffer.
///
/// \param[in] filler is the filler value for the buffer.
///
/// \param len Length (in bytes) of the data block to be appended.
///
/// \return \c true if the append operation succeeded.
///
/// \return \c false if the append operation failed, for example due
///  to bad arguments or an out of memory condition.
///
/// \warning Calling this function (may) invalidates all pointers which
///  have been returned by preceding \ref BufferGetBytes calls.
bool BufferSkip(Buffer* buffer, unsigned char filler, size_t len);

/// \brief Discards the content of a byte buffer.
///
/// This functions discards the current content of a byte buffer and
/// truncates the length of the buffer to zero.
///
/// \param[in,out] buffer The destination byte buffer.
///
/// \return \c true if the append operation succeeded.
///
/// \return \c false if the append operation failed, for example due
///  to bad arguments or an out of memory condition.
///
/// \warning Calling this function (may) invalidates all pointers which
///  have been returned by preceding \ref BufferGetBytes calls.
bool BufferClear(Buffer* buffer);

/// \brief Gets a pointer into the data region of the byte buffer.
///
/// This function returns a pointer to a slice of the data region
/// of the given byte buffer. The caller can use the returned pointer
/// to directly access parts of the byte buffer for reading and writing.
///
/// This function fails with a NULL result, if the caller tries to
/// access a region (given by \c offset and \c len) which lies outside
/// the current bounds of the byte buffer.
///
/// The pointer returned by this function remains valid until either
/// the buffer is freed (via \ref BufferFree) or until the size of the
/// buffer is changed, in response to a \ref BufferAppend, \ref BufferSkip
/// or \ref BufferClear call.
///
/// To get a raw pointer to the content of the full byte buffer the
/// following code fragment can be used:
/// \code
/// ...
/// Buffer* buffer = ...;
/// size_t len = BufferGetLength(buffer);
/// unsigned char* data = BufferGetBytes(buffer, 0, len);
/// ...
/// \endcode
///
/// \param[in,out] buffer The byte buffer to access.
///
/// \param[in] offset The offset of the first byte to be accessed in the
///   buffer. (An offset of zero refers to the beginning of the buffer).
///
/// \param[in] len The number of bytes to be accessed. The slice returned
///  by this function contains \c len bytes starting at \c offset.
///
/// \return On success, a valid pointer to the first byte of the requested
///  part of the byte buffer.
///
/// \return NULL if an emtpy slice (\c len == 0) is requested.
///
/// \return NULL on error, for example if \c buffer is NULL or if the function
///  is called with \c offset and/or \c size referring to a region outside
///  the bounds of the given byte buffer.
unsigned char* BufferGetBytes(Buffer* buffer, size_t offset, size_t len);

/// \brief Gets the length of a byte buffer.
///
/// \param[in] buffer The byte buffer to be queried.
///
/// \return The length of the byte buffer,
size_t BufferGetLength(const Buffer* buffer);

/// @}
#endif // TINYVM_BUFFER_H
