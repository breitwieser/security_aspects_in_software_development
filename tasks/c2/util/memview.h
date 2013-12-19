///
/// \file
/// \brief Memory views on top of byte buffers.
///
#ifndef MEMVIEW_H
#define MEMVIEW_H 1

#include "buffer.h"


//----------------------------------------------------------------------
/// \defgroup UtilMemView Memory views
/// @{

/// \brief Flags for memory views.
///
/// The values of this enumeration are bit-masks used
/// as flags of memory view objects. Each memory view
/// can have one or more of the flags given here.
typedef enum MemViewFlags {
  /// \brief Indicates a normal, writable memory view.
  ///
  /// This flag value indicates a normal writable memory view.
  /// The endianess depends on wether the \ref MEM_VIEW_BIGENDIAN
  /// flag is set or not.
  MEM_VIEW_NORMAL = 0,

  /// \brief Indicates a read-only memory view.
  ///
  /// This flag is mutually exclusive with the \ref MEM_VIEW_NORMAL
  /// flag, and indicates a read-only memory view. All attempts to
  /// write to the memory view will fail.
  MEM_VIEW_READONLY  = 1,

  /// \brief Indicates a big-endian memory view.
  ///
  /// This flag indicates that a memory view uses big-endian byte
  /// (instead of default little endian) byte order to store multi-
  /// byte values.
  MEM_VIEW_BIGENDIAN = 2,

  /// \brief Bit-mask with the reserved flags.
  ///
  /// This special value contains a bit-mask with all currently
  /// reserved flags of the memory view API.
  MEM_VIEW_RESERVED = ~(MEM_VIEW_READONLY | MEM_VIEW_BIGENDIAN)
} MemViewFlags;

/// \brief View of a region of memory.
///
/// Memory views provide a light-weigth memory abstraction
/// on top of byte buffers. A memory view exposes parts of the
/// contents of a byte buffer. In addition to byte-lveel access,
/// memory views provide functions to read and write half-word
/// (16-bit) as well as word (32-bit) qunatities.
///
/// Memory views can either use big-endian orlittle-endian byte
/// ordering when accessing the underlying buffers. The endianess
/// of a memory view is defined at construction time.
///
/// The same byte buffer can be used as backing store by an arbitrary
/// number of memory views. There NO restrictions on overlaps between
/// views or on treating the same area of a buffer as little-endian
/// data in one view and as big-endian data in another view.
///
/// \note Life-time management of the memory views and their underlying
///  byte buffers is up to the API user. It is an undetectable usage
///  error to free a byte buffer, while it still may be used by memory
///  views.
///
typedef struct MemView MemView;

// The memory view structure is part of the public API.
struct MemView {
  /// \brief Flags of this memory view.
  ///
  /// This field contains the bit-wise OR of one or more of the
  /// memory view flags defined in the \ref MemViewFlags enumeration.
  MemViewFlags flags;

  /// \brief Backing-store byte buffer.
  ///
  /// This field refers to the byte buffer acting as backing
  /// store for this memory view.
  ///
  /// \note It is the API users responsbility to ensure that the
  ///  byte buffer is not freed while the memory view may be still
  ///  in use. (Such misusage can not be detected by the memory view
  ///  abtsraction).
  Buffer *buffer;

  /// \brief Base offset of this memory view.
  ///
  /// The base offset of a memory view is the offset of the
  /// first byte in the underlying byte buffer, which belongs
  /// to this memory view.
  size_t base;

  /// \brief Size of this memory view.
  ///
  /// The length of a memory view is the number of bytes,
  /// which are accessible via a given memory view.
  size_t length;
};

/// \brief Initializes a memory view for use.
///
/// This function initializes the memory view given by \c view for use
/// and attaches it to the given buffer. Additionally the function checks,
/// that the base offset and length of the memory view denote a valid
/// slice in the underlying buffer.
///
/// \param[out] view The memory view to be initialized.
///   On success the \c view object is initialized and ready to use. On error
///   the state of the \c view object may be undefined.
///
/// \param[in] flags The flags for the new memory view. This value may be any
///   valid combination of the flags defined in the \ref MemViewFlags enumeration.
///   This parameter is invalid, if any of the bits given in \ref MEM_VIEW_RESERVED
///   are set.
///
/// \param[in,out] buffer The backing store byte buffer of the memory
///   view. (It is the API user's responsibility to ensure that the buffer
///   remains valid, while the memory view may be in use.)
///
/// \param[in] base Base offset fo the first byte of the memory view within
///   the given byte buffer.
///
/// \param[in] length Size of this memory view in bytes. (Number of bytes
///   to be exposed from the underlying byte buffer).
///
/// \return \c true if the memory view was initialized successfully.
///
/// \return \c false if initialization of the memory view failed, for example
///  due to invalid arguments. Initialization always fails if \c base and/or
///  \c length refer to a slice outside the (current) bounds of the underlying
///  byte buffer.
bool MemInit(MemView *view, MemViewFlags flags,
             Buffer *buffer, size_t base, size_t length);

/// \brief Gets the length of a memory view in bytes.
///
/// This function gets the length of a memory view in bytes.
///
/// \param[in] view The memory view to be queried.
///
/// \return The length of the memory view in bytes.
size_t MemGetLength(const MemView *view);

/// \brief Tests if a memory view is marked as read only.
///
/// This function tests the \ref MEM_VIEW_READONLY flag of the given memory
/// view to determine if the view is write-protected.
///
/// \param[in] view The view to be tested.
///
/// \return \c true if the memory view is marked as read only.
///
/// \return \c false if the memory view is writable (or if \c view is NULL).
bool MemIsReadOnly(const MemView *view);

/// \brief Tests if a memory view uses big-endian byte-order.
///
/// This function tests the \ref MEM_VIEW_BIGENDIAN flag of the given memory
/// view to determine if the view uses big-endian byte order.
///
/// \param[in] view The view to be tested.
///
/// \return \c true if the memory view is marked as big-endian view.
///
/// \return \c false if the memory view is writable (or if \c view is NULL).
bool MemIsBigEndian(const MemView *view);

/// \brief Enables or disables big-endian byte-order of a memory view.
///
/// This view sets or clears the \ref MEM_VIEW_BIGENDIAN flag of
/// a given memory view. Changing the endianess mode of a memory view
/// affects all future calls to the \ref MemGetWord, \ref MemSetWord,
/// \ref MemGetHalf and \ref MemSetHalf functions.
///
/// \note This function does NOT modify the content of a memory view. It
///  only affects the byte order used by accessor function used later on.
///
/// \param[in,out] view is the memory view to be modified.
///
/// \param enable is the new value for the \ref MEM_VIEW_BIGENDIAN flag
///   (\c true if the flag should be set, \c false otherwise).
///
/// \return \c true on success (if the mode was set).
///
/// \return \c false if \c view was NULL).
bool MemSetBigEndian(MemView *view, bool enable);

/// \brief Gets a raw pointer to a range of bytes in a memory view.
///
/// This functions returns a raw pointer to a range of bytes visible
/// in a memory view. The offset accepted by this function is relative
/// to the beginning of the memory view. (The base offset of the memory
/// is automatically added when calling \ref BufferGetBytes of the underlying
/// byte buffer.)
///
/// \note This function does not automatically check the \ref MemIsReadOnly
///  flag. It is the API user's responsibility to perform such a check if
///  desired.
///
/// \param[in] view The memory view to be accessed.
///
/// \param offset Offset of the first byte (relative to the start if the
///   memory view).
///
/// \param length Length of the byte range to be accessed.
///
/// \return A pointer to the byte range in the underlying byte buffer on
///  success. See the remarks on \ref BufferGetBytes for details on the validity
///  of the returned pointer. (e.g. calling \ref BufferAppend on the underlying
///  buffer will invalidate the returned pointer).
unsigned char* MemGetBytes(MemView *view, size_t offset, size_t length);

/// \brief Reads a single byte (unsigned 8-bit integer) from a memory view.
///
/// This functions reads a single byte at offset \c offset from the memory view
/// given by \c view and stores the result in \c z.
///
/// \param[out] z Pointer to the variable receiving the result byte
///
/// \param[in] view The memory view to be accessed.
///
/// \param[in] offset The (byte) offset of the half-word to be accessed (relative
///   to the beginning of the view; offset zero referes to the first byte in the
///   view.)
///
/// \return \c true if the read operation was successful.
///
/// \return \c false if the read operation failed, for examnple due to an
///  out of bounds access or other invalid parameters. The variable pointed to
///  by \c z must not be modified in case of an error.
bool MemGetByte(uint8_t *z, MemView *view, size_t offset);

/// \brief Writes a single byte (unsigned 8-bit integer) to a memory view.
///
/// This functions stores a single byte at offset \c offset of the memory view
/// given by \c view. The store operation fails if the storage location of the
/// byte falls outside the bounds of the memory view or the underlying byte buffer.
///
/// This functions honors the \ref MEM_VIEW_READONLY flag of the memory
/// view, to determine wether the memory view is write-protected. Attempts to
/// write to a write-protected memory view will always fail.
///
/// \param[in] view The memory view to be modified.
///
/// \param[in] offset The offset of the byte to be stored (relative to the beginning
///   of the view; offset zero referes to the first byte in the view.)
///
/// \param[in] value The value to be stored.
///
/// \return \c true if the read operation was successful.
///
/// \return \c false if the read operation failed, for examnple due to an
///  out of bounds access or other invalid parameters.
bool MemSetByte(MemView *view, size_t offset, uint8_t value);

/// \brief Reads a single unsigned half-word (16-bit integer) from a memory view.
///
/// This functions reads a single unsigned half-word at offset \c offset
/// from the memory view given by \c view and stores the result in \c z. This function
/// honors the \ref MEM_VIEW_BIGENDIAN flag of the memory view, to determine wether
/// the half-word is stored in big-endian byte order (most significant byte first) or in
/// little-endian byte order (least significant byte first).
///
/// \param[out] z Pointer to the variable receiving the result half-word.
///
/// \param[in] view The memory view to be accessed.
///
/// \param[in] offset The (byte) offset of the half-word to be accessed (relative
///   to the beginning of the view; offset zero referes to the first byte in the
///   view.)
///
/// \return \c true if the read operation was successful.
///
/// \return \c false if the read operation failed, for examnple due to an
///  out of bounds access or other invalid parameters. The variable pointed to
///  by \c z must not be modified in case of an error.
bool MemGetHalf(uint16_t *z, MemView *view, size_t offset);

/// \brief Writes a single unsigned half-word (16-bit integer) to a memory view.
///
/// This functions stores a single unsigned half-word at offset \c offset
/// of the memory view given by \c view. The store operation fails if the storage
/// location of the half-word falls outside the bounds of the memory view or
/// the underlying byte buffer.
///
/// The function honors the \ref MEM_VIEW_BIGENDIAN flag of the memory view, to
/// determine wether the half-word is stored in big-endian byte order (most significant
/// byte first) or in little-endian byte order (least significant byte first).
///
/// Additionally this functions honors the \ref MEM_VIEW_READONLY flag of the memory
/// view, to determine wether the memory view is write-protected. Attempts to write
/// to a write-protected memory view will always fail.
//////
/// \param[in] view The memory view to be modified.
///
/// \param[in] offset The (byte) offset of the half-word to be stored (relative
///   to the beginning of the view; offset zero referes to the first byte in the
///   view.)
///
/// \param[in] value The value to be stored.
///
/// \return \c true if the read operation was successful.
///
/// \return \c false if the read operation failed, for examnple due to an
///  out of bounds access or other invalid parameters.
bool MemSetHalf(MemView *view, size_t offset, uint16_t value);

/// \brief Reads a single unsigned word (32-bit integer) from a memory view.
///
/// This functions reads a single unsigned word at offset \c offset
/// from the memory view given by \c view and stores the result in \c z. This function
/// honors the \ref MEM_VIEW_BIGENDIAN flag of the memory view, to determine wether
/// the word is stored in big-endian byte order (most significant byte first) or in
/// little-endian byte order (least significant byte first).
///
/// \param[out] z Pointer to the variable receiving the result half-word.
///
/// \param[in] view The memory view to be accessed.
///
/// \param[in] offset The (byte) offset of the word to be accessed (relative
///   to the beginning of the view; offset zero referes to the first byte in the
///   view.)
///
/// \return \c true if the read operation was successful.
///
/// \return \c false if the read operation failed, for examnple due to an
///  out of bounds access or other invalid parameters. The variable pointed to
///  by \c z must not be modified in case of an error.
bool MemGetWord(uint32_t *z, MemView *view, size_t offset);

/// \brief Writes a single unsigned word (32-bit integer) to a memory view.
///
/// This functions stores a single unsigned word at offset \c offset
/// of the memory view given by \c view. The store operation fails if the storage
/// location of the word falls outside the bounds of the memory view or
/// the underlying byte buffer.
///
/// The function honors the \ref MEM_VIEW_BIGENDIAN flag of the memory view, to
/// determine wether the word is stored in big-endian byte order (most significant
/// byte first) or in little-endian byte order (least significant byte first).
///
/// Additionally this functions honors the \ref MEM_VIEW_READONLY flag of the memory
/// view, to determine wether the memory view is write-protected. Attempts to write
/// to a write-protected memory view will always fail.
///
/// \param[in] view The memory view to be modified.
///
/// \param[in] offset The (byte) offset of the word to be stored (relative
///   to the beginning of the view; offset zero referes to the first byte in the
///   view.)
///
/// \param[in] value The value to be stored.
///
/// \return \c true if the read operation was successful.
///
/// \return \c false if the read operation failed, for examnple due to an
///  out of bounds access or other invalid parameters.
bool MemSetWord(MemView *view, size_t offset, uint32_t value);

/// @}
#endif // MEMVIEW_H
