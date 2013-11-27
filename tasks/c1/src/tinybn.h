///
/// \file
/// \brief Public API of the tinybn big integer library.
///
/// \warning
///  This header file defines the public API interface of the tinybn big integer
///  library. It is part of the assignment specification and MUST NOT be changed.
///  Details (declaration of internal functions of your implementation, defines,
///  type definitions, ...) should go into the \c tinybn_imp.h header file.
///
#ifndef TINYBN_H
#define TINYBN_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//----------------------------------------------------------------------
/// \defgroup LowLevelApi Low-level multi-precision arithemtic functions.
///@{

/// \brief Single-precision word (32-bit).
///
/// The \ref mp_word_t type is an unsigned integer type, used by the
/// big integer library to represent single precision words. It is currently
/// fixed to the standard unsigned 32-bit (\c uint32_t) integer data type.
typedef uint32_t mp_word_t;

/// \brief Declares a single-precision word constant.
///
/// This macro allows portable definition of (small) \c mp_word_t constants.
#define MP_WORD_C(x) UINT32_C(x)

/// \brief Bit-size of a single-precision word.
///
/// The \ref MP_WORD_BITS macro indicates the number of bits in a single
/// \ref mp_word_t word. Currently this value is fixed to 32 bits.
#define MP_WORD_BITS 32U

/// \brief Maximum value of a single-precision word.
///
/// The \ref MP_WORD_MAX macro denotes the largest value which can be stored
/// in a single \ref mp_word_t variable.
#define MP_WORD_MAX UINT32_MAX

/// \brief Double-precision word (64-bit).
///
/// The \ref mp_long_t type is an unsigned integer type with
/// twice the bit-size of \ref mp_long_t. It is used by the big integer library
/// to represent double precision values (such as the results of multiplying two
/// single precision words). This type is currently fixed to the standard
/// unsigned 64-bit (\c uint64_t) integer data type.
typedef uint64_t mp_long_t;

/// \brief Declares a double-precision word constant.
///
/// This macro allows portable definition of (small) \c mp_long_t constants.
#define MP_LONG_C(x) UINT64_C(x)

/// \brief Maximum value of a double-precision word.
///
/// The \ref MP_LONG_MAX macro denotes the largest value which can be stored
/// in a single \ref mp_long_t variable.
#define MP_LONG_MAX UINT64_MAX

/// \brief Adds two (unsigned) multi-precision integers.
///
/// This functions adds two multi-precision integers given by
/// the word arrays \c a ( of length \c len_a) and \c b (of length \c len_b).
/// The result is stored in the \c z word array, the length
/// of the result array is the larger of \c len_a and \c len_b.
///
/// In case of different operand lengths (\f$len\_a \neq len\_b\f$), the missing
/// words of the shorter operand are assumed to be zero.
///
/// \param[out] z points to the word array where the result (sum) will
///   be stored. The array must be large enough to hold \f$\max\{len\_a,len\_b\}\f$
///   elements.
///
/// \param[in] a points to the word array with the first addend (left operand). This pointer
///   may refer to the same memory region as the \c z pointer.
///
/// \param len_a is the length of the word array referenced by the \c a parameter.
///   This function only accesses elements of the \c a array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_a \f$. No access to the \c a array is performed
///   if \c len_a is zero.
///
/// \param[in] b points to the word array with the second addend (right operand). This pointer
///   may refer to the same memory region as the \c z pointer.
///
/// \param len_b is the length of the word array referenced by the \c b parameter.
///   This function only accesses elements of the \c b array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_b \f$. No access to the \c b array is performed
///   if \c len_a is zero.
///
/// \return The carry value of the multi-precision addition. The carry comes
///   from the final single-precision addition in the top word of the result, and
///   can be zero (result fits into \f$\max\{len\_a,len\_b\}\f$ words) or one,
///   (carry in top word).
bool MpAdd(mp_word_t *z, const mp_word_t *a, size_t len_a,
           const mp_word_t *b, size_t len_b);

/// \brief Subtracts two (unsigned) multi-precision integers.
///
/// This function subtracts two multi-precision integers given by
/// the word arrays \c a (minuend of length \c len_a) and \c b (subtrahend of length \c len_b).
/// The result is stored in the \c z word array, the length
/// of the result array is the larger of \c len_a and \c len_b.
///
/// In case of different operand lengths (\f$len\_a \neq len\_b\f$), the missing
/// words of the shorter operand are assumed to be zero.
///
/// \note Individual words of the operands are subtracted as \em unsigned integers,
///  and have C unsigned subtraction overflow semantics. (e.g. subtracting 1 from 0 will
///  produce \f$2^{32}-1\f$.
///
/// \param[out] z points to the word array where the result (sum) will
///   be stored. The array must be large enough to hold \f$\max\{len\_a,len\_b\}\f$
///   elements.
///
/// \param[in] a points to the word array with the minuend (left operand). This pointer
///   may refer to the same memory region as the \c z pointer.
///
/// \param len_a is the length of the word array referenced by the \c a parameter.
///   This function only accesses elements of the \c a array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_a \f$. No access to the \c a array is performed
///   if \c len_a is zero.
///
/// \param[in] b points to the word array with the subtrahend (right operand). This pointer
///   may refer to the same memory region as the \c z pointer.
///
/// \param len_b is the length of the word array referenced by the \c b parameter.
///   This function only accesses elements of the \c b array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_b \f$. No access to the \c b array is performed
///   if \c len_a is zero.
///
/// \return The borrow value of the multi-precision subtraction. The borrow comes
///   from the final single-precision subtraction in the top word
///   of the result, and can be zero (\f$a \geq b\f$ or one (\f$a < b\f$).
bool MpSub(mp_word_t *z, const mp_word_t *a, size_t len_a,
           const mp_word_t *b, size_t len_b);

/// \brief Compares two multi-precision integers.
///
/// This functions compares the magnitude of two unsigned multi-precision
/// integers. the return value is \c 0 if both integers are equal, \c 1
/// if \c a is greater than \c b, or -1 if \c a is less than \c b.
///
/// In case of different operand lengths (\f$len\_a \neq len\_b\f$), th
/// missing words of the shorter operand are assumed to be zero.
///
///
/// \param[in] a points to the word array with the first (left) operand.
///
/// \param len_a is the length of the word array referenced by the \c a parameter.
///   This function only accesses elements of the \c a array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_a \f$. No access to the \c a array is performed
///   if \c len_a is zero.
///
/// \param[in] b points to the word array with the second (right).
///
/// \param len_b is the length of the word array referenced by the \c b parameter.
///   This function only accesses elements of the \c b array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_b \f$. No access to the \c b array is performed
///   if \c len_a is zero.
///
/// \return 0 if both multi-precision integers are equal.
/// \return 1 if big-integer \c a is strictly greater than big-integer \c b.
/// \return -1 if big-integer \c a is strictly less than big-integer \c b.
int MpCompare(const mp_word_t *a, size_t len_a,
              const mp_word_t *b, size_t len_b);

/// \brief Multiplies two (unsigned) multi-precision integers.
///
/// This function multiplies two multi-precision integers given by
/// the word arrays \c a (of length \c len_a) and \c b (of length \c len_b).
/// The result is stored in the \c z word array, the length
/// of the result array is the sum of \c len_a and \c len_b.
///
/// \note Multi-precision multiplication needs to access both operand arrays
///  while computing the product. This function requires that the result array \c z
///  does not overlap with the operand array \c a or \c b. Violation of this restriction
///  will produce incorrect results in the \c z array.
///
/// \param[out] z points to the word array where the result (product) will
///   be stored. The array must be large enough to hold \f$len\_a + len\_b\f$
///   elements. The memory region referenced by this pointer MUST NOT overlap
///   the memory regions referenced by the \c a or \c b parameters.
///
/// \param[in] a points to the word array with the first factor (left operand).
///   This pointer MUST NOT not refer to the same memory region as the \c z pointer.
///
/// \param len_a is the length of the word array referenced by the \c a parameter.
///   This function only accesses elements of the \c a array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_a \f$. No access to the \c a array is performed
///   if \c len_a is zero.
///
/// \param[in] b points to the word array with the second factor (right operand).
///   This pointer MUST NOT refer to the same memory region as the \c z pointer.
///
/// \param len_b is the length of the word array referenced by the \c b parameter.
///   This function only accesses elements of the \c b array with index \f$i\f$ in
///   range \f$ 0 \leq i < len\_b \f$. No access to the \c b array is performed
///   if \c len_a is zero.
void MpMul(mp_word_t *restrict z, const mp_word_t *a, size_t len_a,
           const mp_word_t *b, size_t len_b);

///@}

//----------------------------------------------------------------------
/// \defgroup BigIntApi High-level big-integer arithmetic functions.
///@{

/// \brief An arbitrary-precision big integer.
///
/// The BigInteger data-type represents a signed arbitrary precision
/// big-integer.
///
/// \note The internal details of this data-structure are not part of
///  the public big integer API.
///
typedef struct BigInteger BigInteger;

/// \brief Allocates a new big integer.
///
/// This function allocates a new big-integer object and initializes it
/// to represent the value zero.
///
/// \return the newly allocated big-integer object. The returned object
///   should deleted with \ref BigIntFree when it is no longer needed,
///   in order to prevent memory leaks.
///
/// \return \c NULL if the allocation failed (e.g. due to an out of memory
///   condition)
BigInteger* BigIntAlloc(void);

/// \brief Deallocates a big integer.
///
/// This function deletes a big-integer, which had been previously allocated
/// with \ref BigIntAlloc, and releases all associated (memory) resources.
///
/// \param[in,out] z is the big-integer to be deleted.
void BigIntFree(BigInteger *z);

/// \brief Loads the value of a big-integer from a byte array.
///
/// This function sets the magnitude of the big-integer \c z to the unsigned
/// value represented by the \c data byte array. The \c len parameter gives
/// the number of valid data bytes found at \c data.
///
/// The \c data array encodes the unsigned value in big-endian byte-order.
/// The most significant byte being at index zero. The \c data array may
/// contain one or more leading zeros.
///
/// The following code snippet demonstrates how the big integer value
/// \c 0x123456789ABCDEF can be loaded into a big-integer \c p.
/// \code
/// ...
///
/// unsigned char raw[9] = {
///    0x01, 0x23, 0x45, 0x67,
///    0x89, 0xAB, 0xCD, 0xEF
/// };
///
/// BigInteger *p = BigIntAlloc();
/// BigIntLoad(p, raw, 9);
/// ...
/// \endcode
///
/// \note This function only loads the magnitude (absolute value) of \c z. The
///  sign of \c z is assumed to be positive (or zero if the magnitude is zero).
///  It is the API user's responsibility to restore the sign, if needed.
///
/// \param[in,out] z is the valid big-integer to be set to a new value.
///
/// \param[in] data is a pointer to the byte-array representing the new
///    value of \c z. This value may be \c NULL if \c len is zero.
///
/// \param[in] len is the length of the byte-array referenced by the
///    \c data parameter. This function only accesses elements of
///    the byte-array with indices \f$i\f$ in range \f$0 \leq i < len\f$.
///    No array access is done if \c len is zero.
///
/// \return \c true if the new value of the big-integer was successfully
///   assigned from the byte array.
///
/// \return \c false in case of an error. (e.g. out of memory, bad parameters)
bool BigIntLoad(BigInteger *z, const unsigned char *data, size_t len);

/// \brief Stores the value of a big-integer to a byte array.
///
/// This function writes the \c len least-significant bytes of the magnitude of
/// the big-integer \c z to the byte array described by \c data and \c len. The result
/// will be padded with leading zeros if \c len is greater than the minimum required
/// value. The result will be silently truncated, if the length is less than the minimum
/// required value. The maximum number of bytes required to store a big integer can be
/// estimated by multiplying the result of \ref BigIntGetWordCount with the size
/// of \ref mp_word_t.
///
/// \note This function only stores the magnitude (absolute value) of \c z. It is the
///  API user's responsibility to separately save the sign, if needed.
///
/// \param[in] data is a pointer to the byte-array receiving the \c len least-signifcant
///    bytes of the magnitude of \c z. This pointer may be \c NULL if \c len is zero.
///
/// \param[in] len is the length of the byte-array referenced by the
///    \c data parameter. This function will accesses all elements of
///    the byte-array with indices \f$i\f$ in range \f$0 \leq i < {len}\f$.
///    No array access is done if \c len is zero.
///
/// \param[in,out] z is the valid big-integer to be set to a new value.
///
/// \return \c true if \c len least-significant bytes of the magnitude of
///    the big-integer was successfully written to the byte array.
///
/// \return \c false in case of an error. (e.g. out of memory, bad parameters)
///
/// \see \ref BigIntLoad for additional details on the format of the byte array.
bool BigIntSave(unsigned char *data, size_t len, const BigInteger *z);

/// \brief Gets the minimum number of words required to represent the magnitude
///   of big-integer \c z.
///
/// This function returns the minimum number of \ref mp_word_t words, that is
/// required to represent the magnitude (absolute value) of \c z. The return value
/// of this function is greater or equal than 1 for any non-null \c z object.
///
/// \param[in] z the big integer to be queried.
///
/// \return the minimum number of words required to represent the magnitude of
///    the given big integer, if \c z is not \c NULL. The returned value is
///    greater or equal than 1.
///
/// \return 0, if and only if \c z is \c NULL
size_t BigIntGetWordCount(const BigInteger *z);

/// \brief Gets a word from the word array of a big-integer.
///
/// This function returns the word at position \c index of the word-array of
/// big integer \c z. The least significant word is at index \c 0.
///
/// \param[in] z the big integer to access.
///
/// \param[in] index the zero-based index of the word to be accessed.
///
/// \return the word at the given index of the word array of big integer
///   \c z, if \c z and \c index are valid. (the returned value may be zero)
///
/// \return 0, if \c z is \c NULL or \c index is outside the word array bounds.
mp_word_t BigIntGetAt(const BigInteger *z, size_t index);

/// \brief Sets a word in word array of a big-integer.
///
/// This function sets the word at position \c index of the word-array of
/// big integer \c z to \c value. The least significant word is at index \c 0.
/// The word array of the big integer automatically grows, if the given index
/// is outside the current bounds.
///
/// \param[in] z the big integer to access.
///
/// \param[in] index the zero-based index of the word to be set.
///
/// \return \c true if the word was successfully set.
///
/// \return \c false in case of an error. (e.g. bad parameter, out of memory)
bool BigIntSetAt(BigInteger *z, size_t index, mp_word_t value);

/// \brief Adds two big integers. (\f$z = a + b\f$)
///
/// This function adds the big integers \c a and \c b and stores the
/// result in \c z. This operation honors the signs of its operands; e.g. adding
/// a positive and a negative big integer effectively behaves as subtraction.
///
/// \param[in,out] z points to the big-integer receiving the result. \c z
///   may refer to the same object as \c a and/or \c b.
///
/// \param[in] a points to the first (left) big integer operand.
///
/// \param[in] b points to the second (right) big integer operand.
///
/// \return \c true if the operation succeeded.
///
/// \return\c false if the operation failed. (e.g. bad parameters, out of memory).
bool BigIntAdd(BigInteger *z, const BigInteger *a, const BigInteger *b);

/// \brief Subtracts two big integers. (\f$z = a - b\f$)
///
/// This function subtracts the big integers \c a and \c b and stores the
/// result in \c z. This operation honors the signs of its operands; e.g. subtracting
/// a negative from a positive integer effectively behaves as addition.
///
/// \param[in,out] z points to the big-integer receiving the result. \c z
///   may refer to the same object as \c a and/or \c b.
///
/// \param[in] a points to the first (left) big integer operand.
///
/// \param[in] b points to the second (right) big integer operand.
///
/// \return \c true if the operation succeeded.
///
/// \return \c false if the operation failed. (e.g. bad parameters, out of memory).
bool BigIntSub(BigInteger *z, const BigInteger *a, const BigInteger *b);

/// \brief Multiplies two big integers. (\f$z = a \cdot b\f$)
///
/// This function multiplies the big integers \c a and \c b and stores the
/// result in \c z. This operation honors the signs of its operands; e.g. multiplying
/// two negative integer yields a positive integer.
///
/// \note The high-level allows \c a and/or \c b to be the same object as the
///   result \c z (\c a and/or \c b may alias with \c z).  In order to produce
///   correct results in this case, this function may need to create a temporary
///   big integer for holding the result, while the multiplication is in progress.
///
/// \param[in,out] z points to the big-integer receiving the result. The \c z
///   may refer to the same object as \c a and/or \c b.
///
/// \param[in] a points to the first (left) big integer operand.
///
/// \param[in] b points to the second (right) big integer operand.
///
/// \return \c true if the operation succeeded.
///
/// \return \c false if the operation failed. (e.g. bad parameters, out of memory).
bool BigIntMul(BigInteger *z, const BigInteger *a, const BigInteger *b);

/// \brief Copies the sign and magnitude of a big integer. (\f$z = a\f$)
///
/// This function copies the sign and magnitude of \c a to \c z. It behaves as
/// a no-operation if \c a and \c z refer to the same object.
///
/// \param[in,out] z points to the destination integer. \c z may refer to the
///  same object as \c a.
///
/// \param[in] a points to the source integer.
///
/// \return \c true if the operation succeeded.
///
/// \return \c false if the operation failed. (e.g. bad parameters, out of memory).
bool BigIntCopy(BigInteger *z, const BigInteger *a);

/// \brief Negates a big integer. (\f$z = -a\f$)
///
/// This function negates the sign of \c a and stores the negated big integer in \c z.
/// The magnitude of \c a is copied to \c z without any changes.
///
/// \param[in,out] z points to the destination integer. \c z may refer to the
///  same object as \c a.
///
/// \param[in] a points to the source integer.
///
/// \return \c true if the operation succeeded.
///
/// \return \c false if the operation failed. (e.g. bad parameters, out of memory).
bool BigIntNeg(BigInteger *z, const BigInteger *a);

/// \brief Gets the absolute value of a big integer. (\f$z = |a|\f$)
///
/// This function returns the absolute value of \c a, by copying the magnitude
/// to \c z and making the sign of \c z positive (respectively zero, if the
/// magnitude is zero).
///
/// \param[in,out] z points to the destination integer. \c z may refer to the
///  same object as \c a.
///
/// \param[in] a points to the source integer.
///
/// \return \c true if the operation succeeded.
///
/// \return \c false if the operation failed. (e.g. bad parameters, out of memory).
bool BigIntAbs(BigInteger *z, const BigInteger *a);

/// \brief Gets the sign of a big integer. (\f$z = \textrm{sgn} a\f$)
///
/// This function implements the signum function for big integer. It returns a
/// value indicating if the given big integer \c a is positive (+1),
/// negative (-1), or zero (0).
///
/// \param[in] a points to the source integer.
///
/// \return -1, if and only if \f$a < 0\f$.
///
/// \return 0, if \f$a = 0\f$ or if the \c a pointer is \c NULL.
///
/// \return +1, if and only if \f$a > 0\f$.
int BigIntSgn(const BigInteger *a);

/// \brief Compares the sign and magnitude of two big integers.
///
/// This function compares the sign and magnitude of big integers \c a
/// and \c b. The return value indicates if \c a is less than \c b (-1),
/// greater than \c b (+1), or equal to \c b (0).
///
/// \param[in] a points to the first (left) big integer operand.
///
/// \param[in] b points to the second (right) big integer operand.
///
/// \return -1, if and only if \f$a < b\f$.
///
/// \return 0, if \f$a = b\f$ or if at least one of the operands is \c NULL.
///
/// \return +1, if and only if \f$a > b\f$.
int BigIntCompare(const BigInteger *a, const BigInteger *b);

///@}

#endif // TINYBN_H
