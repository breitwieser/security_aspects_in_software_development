//
/// \file
/// \brief Constants and definitions of the tinyvm bytecode format.
///
/// This file defines the bytecode format and instruction set
/// of the tinyvm virtual machine. We expect your implementations to be
/// compatible with the bytecode instruction set described hereafter.
///
/// \note You can extend this format if you want to add your own
///  bytecodes, as long as your implementation remains backwards
///  compatible with the original format discussed in the assignment
///  specification.
///
#ifndef TINYVM_BYTECODE_H
#define TINYVM_BYTECODE_H

//----------------------------------------------------------------------
/// \name VM constants and definitions
///
/// The \ref VM_VERSION_MAJOR and \ref VM_VERSION_MINOR constants
/// contain the latest version of the tinyvm virtual machine described in this
/// header file. Currently the only defined version of the tinyvm architecture
/// is version \c 1.0.
///
/// Due to the design of the bytecode instruction set, the following limits
/// apply to bytecode programs:
///
/// \li The maximum number of local variable slots per function is restricted
///     to \ref VM_MAX_LOCALS (128).
///
/// \li The maximum number of argument slots per function is restricted to
///     \ref VM_MAX_ARGS (128).
///
/// \li The maximum number of result slots per function is restricted to
///     \ref VM_MAX_RESULTS (15).
///
/// \li The maximum offset of a branch target relative to the branch instruction
///     must fit into a signed (two's complement) 16-bit integer. The smallest
///     (most negative) branch offset if \ref VM_MIN_JUMP, the largest (most positive)
///     branch offset if \ref VM_MAX_JUMP.
///
/// The tinyvm instruction set can be encoded in big-endian or in little-endian
/// byte order. The byte order of a bytecode blob can be detected by looking at
/// the the first tag (\ref VM_TAG_UNIT) in the header.
///
/// @{

/// \brief Select big-endian byte order as default for tinyvm bytecodes.
///
/// This macro selects big endian byte-order for all 16-bit and
/// 32-bit quantities. Such values are stored with their most-significant
/// byte at the lowest memory address.
///
#define VM_BIGENDIAN 1

/// \brief Major version of the VM. (1)
///
/// This constant defines the major version of the tiny virtual
/// machine architecture described in this file.
#define VM_VERSION_MAJOR 0x01U

/// \brief Minor version of the VM. (0)
///
/// This constant defines the minor version of the tiny virtual
/// machine architecture described in this file.
#define VM_VERSION_MINOR 0x00U

/// \brief Maximum number of local variable slots of a function.
///
/// This constant gives the maximum number (128) of local variables
/// which can be used within function implemented in byte-code. The limit
/// is due to the use of a single byte for encoding the local index
/// in \ref VM_OP_LDVAR and \ref VM_OP_STVAR instructions.
#define VM_MAX_LOCALS 128U

/// \brief Maximum number of argument slots of a function
///
/// This constant gives the maximum number (128) of input arguments
/// which can be used within functions implemented in byte-code. The limit
/// is due to the use of a single byte for encoding the local index
/// in \ref VM_OP_LDVAR and \ref VM_OP_STVAR instructions.
#define VM_MAX_ARGS   128U

/// \brief Maximum number of result slots of a function
///
/// This constant gives the maximum number (128) of results, which can
/// by returned from a function implemented in byte-code.
#define VM_MAX_RESULTS 128U

/// \brief Minimum signed jump offset for branch instructions
///
/// The virtual machine jump instructions use signed (two's complement)
/// 16-bit offsets to address the branch targets relative to the virtual
/// program counter of the jump instruction.
///
/// This value is the smallest (most negative) value which can be
/// represented in a 16-bit two's complement integer.
#define VM_MIN_JUMP    INT16_MIN

/// \brief Maximum signed jump offset for branch instructions
///
/// The virtual machine jump instructions use signed (two's complement)
/// 16-bit offsets to address the branch targets relative to the virtual
/// program counter of the jump instruction.
///
/// This value is the largest (most positive) value which can be
/// represented in a 16-bit two's complement integer.
#define VM_MAX_JUMP    INT16_MAX

/// @}

//----------------------------------------------------------------------
/// \name Object handles
///
/// The TinyVM virtual machine uses handles to refer to data and code
/// objects within an application running on the VM. A handle is a 32-bit
/// value which acts as unique "name" of an object in a running VM.
///
/// Assignment of handle values to data and code objects is more or
/// less arbitrary.
///
/// \li The virtual null handle is fixed to \ref VM_NULL_HANDLE.
///
/// \li The tinyasm assembler by default assigns handle values in range \ref VM_DATA_HANDLE_FIRST
///     to \ref VM_DATA_HANDLE_LAST to data objects which are defined in assembler source files
///     via (".object data").
///
/// \li The tinyasm assembler by default assigns handle values in range \ref VM_CODE_HANDLE_FIRST
///     to \ref VM_CODE_HANDLE_LAST to functions (code objects) which are defined in assembler source
///     files via (".object code" and ".function").
///
/// \li (Almost) arbitrary handle values can be manually assigned to objects in assembler
///     source code. It is not possible to assign the null handle to valid object; all other
///     variants are valid.
///
/// \note The default ranges for code and data objects have been chosen to simplify debugging. In
///   big-enadian bytecode programs, the automatically assigned code and data handles will start
///   with a "0xC0" are "0xDA" byte in the hexdump, making them easy to sport in the hexdump.
///
/// @{

/// \brief Null handle.
///
/// This special handle value expresses the null handle. It references
/// "no object" or equivalently the "null object". (The null handle can
/// be is the virtual machine equivalent of a native NULL pointer)
#define VM_NULL_HANDLE              0x00000000U

/// \brief First automatically assigned code object handle.
///
/// The assembler by default allocates handles for code objects (functions
/// and objects with \c code qualifier) in the range between
/// \ref VM_CODE_HANDLE_FIRST and \ref VM_CODE_HANDLE_LAST.
#define VM_CODE_HANDLE_FIRST 0xC0000000U

/// \brief Last automatically assigned code object handle.
///
/// \see VM_CODE_HANDLE_FIRST for details on static code handles.
#define VM_CODE_HANDLE_LAST  0xC0FFFFFFU

/// \brief First automatically assigned data object handle.
///
/// The assembler by default allocates handles for code objects (functions
/// and objects with \c code qualifier) in the range between
/// \ref VM_DATA_HANDLE_FIRST and \ref VM_DATA_HANDLE_LAST.
#define VM_DATA_HANDLE_FIRST 0xDA000000U

/// \brief Last automatically assigned data object handle.
///
/// \see VM_DATA_HANDLE_FIRST for details on default data handle assignment.
#define VM_DATA_HANDLE_LAST  0xDAFFFFFFU

/// @}

//----------------------------------------------------------------------
/// \name Objects and binary format
///
/// The tiny virtual machine supports a simple binary representation for
/// virtual machine objects, which can be used to dump the state of the
/// virtual machine to a binary blob. The tinyasm assembler is able to
/// produce such binary blobs.
///
/// Each element in a tinyvm virtual machine binary is prefixed with a
/// 32-bit tag indicating the type of the element. Currently the following
/// tags are in use:
///
/// \li A single \ref VM_TAG_UNIT indicates the start of a program unit and
///     is accompanied by a short header giving the major and minor
///     VM version used to encode the remaining blob.
///
/// \li Zero or more \ref VM_TAG_OBJECT tags define code and data objects
///     which can be found in the given program unit.
///
/// \li A final \ref VM_TAG_END tag marks the end of the program unit.
///
/// An entire virtual machine binary can be described by the following
/// pseudo-C structures:
///
///\code
/// struct VmBinUnit {
///   uint32_t start_tag;      // Magic start tag bytes (VM_TAG_UNIT)
///   uint16_t vm_major;       // Major VM version (VM_VERSION_MAJOR)
///   uint16_t vm_minor;       // Minor VM version (VM_VERSION_MINOR)
///   uint32_t vm_entry;       // Handle of the entry-point function
///   VmBinObject[] objects;   // Objects defined in this unit
///   uint8_t  end_tag[4];     // Magic end tag bytes (VM_TAG_END)
/// };
///
/// struct VmBinObject {
///   uint32_t obj_tag;        // Magic object tag bytes (VM_TAG_OBJECT)
///   uint32_t qualifiers;     // Qualifiers of this object (VmQualifiers)
///   uint32_t handle;         // Handle of this object
///   uint32_t length;         // Length of the object content
///   uint8_t data[length];    // Payload data of the object (see remarks below)
/// };
///
/// // Valid code objects (functions) contain a small 4-byte header describing the
/// // number of arguments, local variables and results of the function. The payload
/// // data of a code object is a VmBinFunction structure:
/// struct VmBinFunction {
///   uint8_t reserved;        // Reserved (for function flags, should be zero)
///   uint8_t num_results;     // Number of result slots [0..VM_MAX_RESULTS]
///   uint8_t num_args;        // Number of argument slots [0..VM_MAX_ARGS]
///   uint8_t num_locals;      // Number of local slots [0..VM_MAX_LOCALS]
///   uint8_t bytecode[];      // Bytecode of this object (length given by enclosing object)
/// };
///\endcode
/// @{

/// \brief Binary tag at the start of a virtual machine program unit.
///
/// The binary representation of a program unit starts with this tag
/// (in big-endian order). It corresponds to the byte sequence \c 'tvm\0'.
#define VM_TAG_UNIT 0x74766D00U

/// \brief Binary tag of a virtual machine object.
///
/// The binary representation of a virtual machine object starts with
/// this tag.
#define VM_TAG_OBJECT 0x6F626A00U

/// \brief Binary tag of a virtual machine object.
///
/// The binary representation of a virtual machine object starts with
/// this tag.
#define VM_TAG_FUNCTION 0x66756E00U

/// \brief Offset of the "num_results" field in a function header
///
/// This constant gives the offset of the "num_results" field
/// in the payload data of a code object.
#define VM_FUNCTION_NUM_RESULTS_OFFSET 1

/// \brief Offset of the "num_args" field in a function header
///
/// This constant gives the offset of the "num_args" field
/// in the payload data of a code object.
#define VM_FUNCTION_NUM_ARGS_OFFSET 2

/// \brief Offset of the "num_locals" field in a function header
///
/// This constant gives the offset of the "num_locals" field
/// in the payload data of a code object.
#define VM_FUNCTION_NUM_LOCALS_OFFSET 3

/// \brief Offset of the first bytecode instrution in a function
///
/// This constant gives the offset of the first bytecode instruction
/// in the payload data of a code object. (it corresponds to virtual
/// program counter location 0 within the function).
#define VM_FUNCTION_BYTECODE_OFFSET 4

/// \brief Binary tag at the end of a virtual machine program unit.
///
/// The binary representation of a program unit ends with this tag. This
/// tag acts as terminator for the objects list contained in the program
/// unit.
#define VM_TAG_END 0x656E6400U

/// \brief Qualifiers for virtual machine objects.
///
/// Qualifiers define the type and nature of virtual machine objects. This
/// enumeration lists the bit-masks for the individual object qualifiers.
///
typedef enum VmQualifiers {
  /// \brief No qualifiers set.
  ///
  /// This value indicates that no object qualifiers are set at all. Objects
  /// with this qualifier value are invalid. Valid objects always have at least
  /// the \ref VM_QUALIFIER_DATA or the \ref VM_QUALIFIER_CODE qualifier set.
  VM_QUALIFIER_NONE      = 0, ///< No qualifiers

  /// \brief Object contains data.
  ///
  /// This qualifier identifies an object as data object. Data objects can
  /// contain arbitrary bytecode program defined content. The virtual machine
  /// does not impose any structual restrictionas on the object content.
  ///
  /// Data objects are mutually exclusive with code objects. A data object,
  /// which has the \ref VM_QUALIFIER_DATA qualifier can not have
  /// the \ref VM_QUALIFIER_CODE qualifier.
  ///
  /// \see VM_QUALIFIER_TYPE for a convenience bitmask to test for code and data objects.
  VM_QUALIFIER_DATA      = 1,

  /// \brief Object contains code. (Function)
  ///
  /// This qualifier indicates that an object contains code for a function. The
  /// content of code objects consists of a valid virtual machine function header,
  /// followed by the bytecode of the function body.
  ///
  /// Typically code objects also have the \ref VM_QUALIFIER_INMUTABLE qualifier,
  /// to indicate that the function code is not modifiable.
  ///
  /// Code objects are mutually exclusive with data objects. A code object,
  /// which has the \ref VM_QUALIFIER_CODE qualifier can not have
  /// the \ref VM_QUALIFIER_DATA qualifier.
  ///
  /// \see VM_QUALIFIER_TYPE for a convenience bitmask to test for code and data objects.
  VM_QUALIFIER_CODE      = 2,

  /// \brief Object is inmutable (read only).
  ///
  /// The inmutable qualifier marks the content of an object as read-only. This
  /// qualifier is valid on code and data objects. Any attempts to modify the
  /// content of an inmutable object with one of the store byte-codes will by
  /// detected and reported by the virtual machine as error.
  VM_QUALIFIER_INMUTABLE = 4,

  /// \brief Object is protected against readback from bytecode programs.
  ///
  /// The unreadable qualifier marks the content of an object as not readable
  /// by bytecode instructions (\ref VM_OP_LDB, \ref VM_OP_LDW). This qualifier is
  /// valid on code and data objects. An object qualified with this qualifier
  /// can not only by read by the VM itself using the \ref VmAccessObject function.
  /// Any attempts to read the object with an \ref VM_OP_LDB or \ref VM_OP_LDW
  /// bytecode causes a virtual machine error.
  VM_QUALIFIER_PROTECTED = 8,

  /// \brief Mask to identify the type (code or data) of an object.
  ///
  /// This enum value defines a convenience bit-mask to simplify identification
  /// of an object as code or data object.
  VM_QUALIFIER_TYPE = VM_QUALIFIER_DATA | VM_QUALIFIER_CODE,

  /// \brief Mask with all reserved qualifiers.
  ///
  /// This enum value defines a bit-mask with all reserved qualifiers, which can
  /// not be used in virtual machine bytecode files.
  VM_QUALIFIER_RESERVED = ~(VM_QUALIFIER_TYPE | VM_QUALIFIER_INMUTABLE |
			    VM_QUALIFIER_PROTECTED)
} VmQualifiers;

/// @}

//----------------------------------------------------------------------
/// \name Bytecode instructions
///
/// The tiny virtual machine bytecode instruction consists of seven
/// larger functional groups:
///  \li Constant loading (\ref VM_OP_IMM32, \ref VM_OP_HANDLE)
///
///  \li Call and branch instructions (\ref VM_OP_CALL, \ref VM_OP_JMP,
///       \ref VM_OP_BT, \ref VM_OP_BF, \ref VM_OP_RET)
///
///  \li Operand stack manipulation (\ref VM_OP_POP, \ref VM_OP_PEEK)
///
///  \li Compare operations
///      (\ref VM_OP_CMPEQ, \ref VM_OP_CMPNE, \c VM_OP_UCMPLT,
///       \ref VM_OP_UCMPLE, \ref VM_OP_UCMPGE, \ref VM_OP_UCMPGT,
///       \ref VM_OP_SCMPLT, \ref VM_OP_SCMPLE, \ref VM_OP_SCMPGE,
///       \ref VM_OP_SCMPGT)
///
///  \li Load and store operations
//       (\ref VM_OP_LDB, \ref VM_OP_LDW,
///       \ref VM_OP_STB, \ref VM_OP_STW, \ref VM_OP_LDVAR,
///       \ref VM_OP_STVAR)
///
///  \li Arithmetic and logic operations
///      (\ref VM_OP_ADD, \ref VM_OP_SUB, \ref VM_OP_MUL, \ref VM_OP_UDIV,
///       \ref VM_OP_SDIV, \ref VM_OP_SHL, \ref VM_OP_SHR, \ref VM_OP_ASR,
///       \ref VM_OP_ROR, \ref VM_OP_NEG, \ref VM_OP_NOT, \ref VM_OP_AND,
///       \ref VM_OP_OR, \ref VM_OP_XOR)
///
///  \li Virtual machine calls
///      (\ref VM_OP_VMC)
///
/// All bytecode instructions are designed such that the type of operation and
/// the format of ths instruction can be determined uniquely by looking at the
/// first byte. Depending on the type and format a bytecode instruction may
/// be 1, 2, 3 or 5 bytes long.
///
/// @{

//----------------------------------------------------------------------
/// \brief Load a (signed or unsigned) 32-bit constant (\c IMM32)
///
/// \par Bytecode
///  \verbatim | 0x04 | imm[31:24] | imm[23:16] | imm[15:8] | imm[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots \rightarrow \ldots, z \f$
///
/// The \c IMM32 instruction pushes an arbitrary 32-bit value \f$z\f$
/// to the operand stack. Unsigned values are encoded in binary, signed values
/// are encoded in two's complement representation. It is neither possible nor necessary
/// to distinguish wether an \c IMM32 instruction encodes a signed or unsigned operand.
#define VM_OP_IMM32     0x04U

/// \brief Load object handle.
///
/// \par Bytecode
///  \verbatim | 0x05 | handle[31:24] | handle[23:16] | handle[15:8] | handle[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots \rightarrow \ldots handle \f$
///
/// The \c HANDLE instruction loads an object handle onto the operand stack. This
/// instruction behaves very similar to the \ref VM_OP_IMM32 instruction.
#define VM_OP_HANDLE    0x05U

//----------------------------------------------------------------------
/// \name Calls and branches
///
/// Function calls are performed using the standard \ref VM_OP_CALL instruction. There is
/// currently no support for indirect ("function pointer") calls in the bytecode instruction set.
/// To return from a function the \ref VM_OP_RET instruction is used.
///
/// Unconditional branches are realized with the \ref VM_OP_JMP instruction. The \ref VM_OP_BT
/// and \ref VM_OP_BF instructions conditionally branch if their stack operand is
/// non-zero (respectively zero).
///
/// @{

/// \brief Call function (\c CALL)
///
/// \par Bytecode
///  \verbatim | 0x10 | handle[31:24] | handle[23:16] | handle[15:8] | handle[7:0] |\endverbatim
///
/// \par Stack
///   \f$ \ldots [, arg0 [, arg1 [, ...]] \rightarrow \ldots [, ret0 [, ret1, [, ...]] \f$
///
/// The \c CALL instruction calls the function identified by the function handle given in the
/// immediate operand. The number of stack operands and return values of the call can be
/// determined from the function header of the called function.
///
/// Call arguments are pushed onto the stack in left-to-right order, the first argument
/// is pushed first (and thus is the "farthest" element relative to the stack pointer).
///
/// \see VM_OP_RET for the return instruction.
#define VM_OP_CALL      0x10U

/// \brief Unconditional jump (\c JMP)
///
/// \par Bytecode
///  \verbatim | 0x12 | off[15:8] | off[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots \rightarrow \ldots \f$
///
/// The \c JMP instruction unconditionally branches to another byte-code position within
/// the current function. The branch target is constructed by adding the signed 16-bit \c off value
/// to the virtual program counter location of the offset field of the jump instruction.
///
/// It is illegal to jump to a target outside the bounds of the current function, any attempts
/// to branch before the beginning or past the end of a function within a byte-code program are
/// detected and reported as errors by the VM.
#define VM_OP_JMP       0x12U

/// \brief Branch if operand \c a is true (non-zero) (\c BT)
///
/// \par Bytecode
///  \verbatim | 0x15 |  off[15:8] | off[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a \rightarrow \ldots \f$
///
/// The \c BT instruction unconditionally branches to another byte-code position within
/// the current function, if the \f$a\f$ is non-zero (or if the \f$h_{a}\f$ handle is different
/// to the null handle).
///
/// The branch target is constructed by adding the signed 16-bit \c off value to the virtual
/// program counter location of the offset field of the jump instruction.
#define VM_OP_BT        0x15U

/// \brief Branch if operand \c a is false (zero) (\c BF)
///
/// \par Bytecode
///  \verbatim | 0x15 | off[15:8] | off[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a \rightarrow \ldots \f$
///
/// The \c BF instruction unconditionally branches to another byte-code position within
/// the current function, if the \f$a\f$ is zero. This instruction behaves like the \ref VM_OP_BT
/// instruction, except that the branch condition is negated.
#define VM_OP_BF        0x16U

/// \brief Return from current function. (\c RET)
///
/// \par Bytecode
///  \verbatim | 0x17 | \endverbatim
///
/// \par Stack
///   \f$ [rets] \qquad \textrm{(in current frame)} \rightarrow \ldots [, rets] \qquad \textrm{(in caller's frame)}\f$
///
/// The \c RET instruction returns from the current function and passes return values to the caller. As part
/// of the function return, the current stack frame is destroyed and execution continues in the calling function,
/// immediately after the calling \ref VM_OP_CALL instruction.
///
/// The number of result stack slots is encoded in the in the function header of the active function.
///
/// The local stack frame must contain \b exactly the number of result slots given in the function header, when
/// a \c RET instruction is executed. Any attempt to return, with more or less result slots should be detected
/// and reported as error by the VM.
#define VM_OP_RET       0x17U

/// @}

//----------------------------------------------------------------------

/// \name Stack operations
///
/// The tinyvm virtual machine only supports two functions to directly
/// manipulate the operand stack:
/// \li \ref VM_OP_POP discards the top of stack operand.
/// \li \ref VM_OP_PEEK duplicates a specific stack element.
///
/// @{

/// \brief Discard the \c n top-of-stack operands (\c POP)
///
/// \par Bytecode
///  \verbatim | 0x20 | n[7:0] | \endverbatim
///
/// \par Stack (example for n=1)
///   \f$ \ldots, a \rightarrow \ldots \f$
///
/// \par Stack (example for n=2)
///   \f$ \ldots, a, b \rightarrow \ldots \f$
///
/// The \c POP instruction discards \c n elements from at the top of
/// stack. This operation behaves as no-operation if \c n is zero.
#define VM_OP_POP       0x20U

/// \brief Duplicates the \c n-th element of the stack (\c DUPN)
///
/// \par Bytecode
///  \verbatim | 0x21 | n[7:0] | \endverbatim
///
/// \par Stack (example for n=0)
///   \f$ \ldots, a \rightarrow \ldots, a, a \f$
///
/// \par Stack (example for n=1)
///   \f$ \ldots, a, b \rightarrow \ldots, a, b, a \f$
///
/// The \c PEEK instruction duplicates the n-th element (counted from
/// top of stack) of the stack.
#define VM_OP_PEEK       0x21U

/// \brief Swaps the two top of stack operands
///
/// \par Bytecode
///  \verbatim | 0x22 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, b, a \f$
///
/// The \c SWAP instruction swaps the two top of stack operands.
#define VM_OP_SWAP       0x22U

/// @}

//----------------------------------------------------------------------
/// \name Compare instructions
///
/// Eight compare instructions allow relational comparison of unsigned
/// (\ref VM_OP_UCMPLT, \ref VM_OP_UCMPLE, \ref VM_OP_UCMPGE, \ref VM_OP_UCMPGT)
/// and signed (\ref VM_OP_SCMPLT, \ref VM_OP_SCMPLE, \ref VM_OP_SCMPGE, \ref VM_OP_SCMPGT)
/// word values.
///
/// Each of the compare operations accepts two single word stack operands
/// \f$a\f$ and \f$b\f$ and produces a single result word \f$z\f$ which reflects
/// the boolean outcome of the comparison. The boolean value \c true is encoded
/// as 1, the boolean value \c false is encoded as 0.
///
/// The result of compare operations can be used directly with conditional branches
/// (\ref VM_OP_BT and \ref VM_OP_BF) and logic operations (\ref VM_OP_AND,
/// \ref VM_OP_OR, \ref VM_OP_XOR) to realize complex conditional expressions.
///
/// @{

/// \brief Compare if operand \c a is equal to operand \c b. (\c CMPEQ)
///
/// \par Bytecode
///  \verbatim | 0x28 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c CMPEQ instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as
/// 32-bit integers and tests if \f$a\f$ is equal to (\f$a = b\f$) \f$b\f$. The result of the
/// comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_CMPEQ    0x2EU

/// \brief Compare if operand \c a is not equal to operand \c b. (\c CMPEQ)
///
/// \par Bytecode
///  \verbatim | 0x28 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c CMPNE instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as
/// 32-bit integers and tests if \f$a\f$ is not equal to (\f$a \neq b\f$) \f$b\f$. The result of the
/// comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_CMPNE    0x2FU

/// \brief Compare if unsigned operand \c a is strictly less than unsigned operand \c b. (\c UCMPLT)
///
/// \par Bytecode
///  \verbatim | 0x30 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c UCMPLT instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as unsigned
/// 32-bit integers and tests if \f$a\f$ is strictly less (\f$a < b\f$) than \f$b\f$. The result of the
/// comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_UCMPLT    0x30U

/// \brief Compare if unsigned operand \c a is less or equal than unsigned operand \c b. (\c UCMPLE)
///
/// \par Bytecode
///  \verbatim | 0x31 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c UCMPLE instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as unsigned
/// 32-bit integers and tests if \f$a\f$ is less or equal less (\f$a <= b\f$) than \f$b\f$. The result of the
/// comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_UCMPLE    0x31U

/// \brief Compare if unsigned operand \c a is greater or equal than unsigned operand \c b. (\c UCMPGE)
///
/// \par Bytecode
///  \verbatim | 0x32 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c UCMPGE instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as unsigned
/// 32-bit integers and tests if \f$a\f$ is greater or equal less (\f$a >= b\f$) than \f$b\f$. The result of the
/// comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_UCMPGE    0x32U

/// \brief Compare if unsigned operand \c a is strictly greater than unsigned operand \c b. (\c UCMPGT)
///
/// \par Bytecode
///  \verbatim | 0x33 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c UCMPGT instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as unsigned
/// 32-bit integers and tests if \f$a\f$ is strictly greater (\f$a > b\f$) than \f$b\f$. The result of the
/// comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_UCMPGT    0x33U

/// \brief Compare if signed operand \c a is strictly less than signed operand \c b. (\c SCMPLT)
///
/// \par Bytecode
///  \verbatim | 0x34 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c SCMPLT instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as signed
/// (two's complement) 32-bit integers and tests if \f$a\f$ is strictly less (\f$a < b\f$) than \f$b\f$.
/// The result of the comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_SCMPLT    0x34U

/// \brief Compare if signed operand \c a is less or equal than signed operand \c b. (\c SCMPLE)
///
/// \par Bytecode
///  \verbatim | 0x35 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c SCMPLE instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as signed
/// (two's complement) 32-bit integers and tests if \f$a\f$ is less or equal (\f$a <= b\f$) than \f$b\f$.
/// The result of the comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_SCMPLE    0x35U

/// \brief Compare if signed operand \c a is greater or equal than signed operand \c b. (\c SCMPGE)
///
/// \par Bytecode
///  \verbatim | 0x36 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c SCMPGE instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as signed
/// (two's complement) 32-bit integers and tests if \f$a\f$ is greater or equal (\f$a >= b\f$) than \f$b\f$.
/// The result of the comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_SCMPGE    0x36U

/// \brief Compare if signed operand \c a is strictly greater than signed operand \c b. (\c SCMPGT)
///
/// \par Bytecode
///  \verbatim | 0x37 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots, z \f$
///
/// The \c SCMPGT instruction interprets the two top of stack elements \f$a\f$ and \f$b\f$ as signed
/// (two's complement) 32-bit integers and tests if \f$a\f$ is strictly greater (\f$a > b\f$) than \f$b\f$.
/// The result of the comparison (true for false) is indicated by \f$z\f$.
#define VM_OP_SCMPGT    0x37U

/// @}

//----------------------------------------------------------------------

/// \name Load/store instructions
///
/// The tiny virtual machine provides a total of six instructions for
/// loading or storing stack values to memory locations. Local variables
/// and call arguments can be access via the \ref VM_OP_LDVAR
/// and \ref VM_OP_STVAR instructions, which statically encode the index of
/// the variable or argument directly into the byte-code. There are no instructions
/// to "indirectly" access local variables and arguments.
///
/// The content of data and code objects can be accessed via dedicated
/// load word (\ref VM_OP_LDW), store word (\ref VM_OP_STW), load unsigned byte (\ref VM_OP_LDB),
/// store unsigned byte (\ref VM_OP_STB) instructions.
///
/// @{

/// \brief Load from local variable or argument. (\c LDVAR)
///
/// \par Bytecode
///  \verbatim | 0x40 | idx[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots \rightarrow \ldots, z \f$
///
/// The \c LDVAR instruction loads a value \f$z\f$ from a local variable or argument referenced
/// by local slot index \c idx. The first local variable is at index zero, the first argument is
/// at index \ref VM_MAX_LOCALS.
///
/// The number of local variables and arguments of the current function can be found in the
/// function header. It is an error to access local variable or argument slot indices which are greater
/// then the respective maximums set for the function in the function header.
#define VM_OP_LDVAR     0x40U

/// \brief Store to local variable or argument. (\c STVAR)
///
/// \par Bytecode
///  \verbatim | 0x41 | idx[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a \rightarrow \ldots \f$
///
/// The \c STVAR instruction stores a value \f$a\f$ found at the top of stack into the local
/// variable or argument slot denoted by \c idx.
///
/// \see VM_OP_LDVAR for more details on the indexing scheme and on error handling.
#define VM_OP_STVAR     0x41U

/// \brief Load 32-bit word from memory. (\c LDW)
///
/// \par Bytecode
///  \verbatim | 0x42 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, offset, handle \rightarrow \ldots, z \f$
///
/// The \c LDW instruction loads a 32-bit word from the memory location denoted by
/// \c handle and \c offset. \c handle specifies the source object for the operation,
/// while \c offset is an offset into the object's payload data. The virtual machine
/// must detect and report any attempts to access invalid objects or out of bound
/// offsets.
///
/// The content of any (code or data) object can be read, unless the object has
/// the \ref VM_QUALIFIER_PROTECTED qualifier. Any attempts to read from a
/// protected object should be detected and reported by the VM as error.
#define VM_OP_LDW       0x42U

/// \brief Store 32-bit word to memory. (\c SDW)
///
/// \par Bytecode
///  \verbatim | 0x43 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, index, handle \rightarrow \ldots \f$
///
/// The \c STW instruction stores a 32-bit \f$a\f$ to the memory location denoted by
/// \c handle and \c offset. \c handle specifies the destination object for the operation,
/// while \c offset is an offset into the object's payload data. The virtual machine
/// must detect and report any attempts to access invalid objects or out of bound
/// offsets.
///
/// The content of any (code or data) object can be written, unless the object has
/// the \ref VM_QUALIFIER_INMUTABLE qualifier. Any attempts to write to an
/// inmutable object will be detected and reported by the VM as error.
#define VM_OP_STW       0x43U

/// \brief Load 8-bit byte from memory. (\c LDB)
///
/// \par Bytecode
///  \verbatim | 0x44 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, o_{a}, h_{a} \rightarrow \ldots, z \f$
///
/// The \c LDB instruction loads an unsigned 8-bit byte from the memory location denoted
/// by \c handle and \c offset. \c handle specifies the source object for the operation,
/// while \c offset is an offset into the object's payload data. The virtual machine
/// must detect and report any attempts to access invalid objects or out of bound
/// offsets. The loaded bit is zero-extended to the 32-bit unsigned result \c z.
///
/// The content of any (code or data) object can be read, unless the object has
/// the \ref VM_QUALIFIER_PROTECTED qualifier. Any attempts to read from a
/// protected object should be detected and reported by the VM as error.
#define VM_OP_LDB       0x44U

/// \brief Store 8-bit byte to memory. (\c STB)
///
/// \par Bytecode
///  \verbatim | 0x45 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, a, offset, handle \rightarrow \ldots \f$
///
/// The \c STB instruction stores an unsigned 8-bit byte \f$a\f$ to the memory location
/// denoted by \c handle and \c offset. \c handle specifies the destination object for
/// the operation, while \c offset is an offset into the object's payload data. The
/// virtual machine must detect and report any attempts to access invalid objects
/// or out of bound offsets.
///
/// The content of any (code or data) object can be written, unless the object has
/// the \ref VM_QUALIFIER_INMUTABLE qualifier. Any attempts to write to an
/// inmutable object will be detected and reported by the VM as error.
#define VM_OP_STB       0x45U

/// @}

//----------------------------------------------------------------------
/// \name Arithmetic and logic instructions
///
/// The tiny virtual machine includes a standard set of arithmetic
/// (\ref VM_OP_ADD, \ref VM_OP_SUB, \ref VM_OP_MUL, \ref VM_OP_SDIV,
///  \ref VM_OP_UDIV, \ref VM_OP_NEG), shift (\ref VM_OP_SHL, \ref VM_OP_SHR,
/// \ref VM_OP_ASR, \ref VM_OP_ROR) and logic (\ref VM_OP_AND, \ref VM_OP_OR,
/// \ref VM_OP_XOR, \ref VM_OP_NOT) instructions, which are encoded with a single byte.
///
/// All arithmetic and logic instruction take two stack operands \f$a\f$
/// and \f$b\f$ on input and produce a single stack operand \f$z\f$ on output.
/// Stack operands are generally treated as 32-bit unsigned integers, and
/// overflows lead to silent truncation of results.
///
/// @{

/// \brief Add operand \c a to operand \c b. (\c ADD)
///
/// \par Bytecode
///  \verbatim | 0x50 |\endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// This instruction adds its stack operands \f$z = a + b\f$ and
/// pushes the result (truncated to a 32-bit unsigned integer) onto
/// the stack.
#define VM_OP_ADD       0x50U

/// \brief Subtract operand \c b from operand \c a. (\c SUB)
///
/// \par Bytecode
///  \verbatim | 0x50 |\endverbatim
///
/// \par Stack
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// This instruction subtracts its stack operands \f$z = a - b\f$ and
/// pushes the result (truncated to a 32-bit unsigned integer) onto
/// the stack.
#define VM_OP_SUB       0x51U

/// \brief Multiplies operand \c a with operand \c b. (\c MUL)
///
/// \par Bytecode
///  \verbatim | 0x53 |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
///
/// The \c MUL instruction can be used to multiply two unsigned or two signed
/// (two's complement) operands \f$a\f$ and \f$b\f$. Mixing between unsigned and two's
/// complement numbers in one multiplication is not supported. Internally the \c MUL opcode
/// always performs an unsigned multiplication and truncates the result to a single 32-bit
/// word and pushes it as \f$z\f$ result on the stack.
///
/// \note It can be shown that the lower \f$n\f$-bit half of an \f$n \times n \rightarrow 2n\f$ bit
///   multiplication can be computed in the same way for both the unsigned and the two's complement case.
///   (Consequently there is no need to have two different mulitply instructions).
#define VM_OP_MUL       0x53U

/// \brief Divide unsigned operand \c a by unsigned operand \c b. (\c UDIV)
///
/// \par Bytecode
///  \verbatim | 0x54 |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c UDIV instruction interprets operands \f$a\f$ and \f$b\f$ as unsigned 32-bit
/// integers and pushes the result \f$z\f$ of the division \f$a / b\f$ on the operand stack.
/// Attempts to divide by zero are detected and reported as errors by the virtual machine.
#define VM_OP_UDIV      0x54U

/// \brief Divide signed operand \c a by signed operand \c b. (\c SDIV)
///
/// \par Bytecode
///  \verbatim | 0x55 |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c SDIV instruction interprets operands \f$a\f$ and \f$b\f$ as signed (two's complement)
/// 32-bit integers and pushes the result \f$z\f$ of the division \f$a / b\f$ on the operand stack.
/// Attempts to divide by zero, and attempts to produce a division result which overflows the 32-bit
/// signed integer range are detected and reported as errors by the virtual machine.
#define VM_OP_SDIV      0x55U

/// \brief Logic shift left of operand \c a by the number of bits given in operand \c b. (\c SHL)
///
/// \par Bytecode
///  \verbatim | 0x56 |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c SHL instruction interprets both operands as unsigned 32-bit integers and performs a logical
/// shift left of operand \f$a\f$ by \f$b\f$ bit positions. The result \f$z\f$ of the shift operation (truncated
/// to 32-bit) is pushed on the stack.  Newly shifted-in bit positions are filled with zero bits.
/// Shifting operand \f$a\f$ by more than 31 bits produces a zero result.
///
/// \note Logical shift left of an unsigned or signed integer by \f$n\f$ bits is equivalent to multiplication
///   by \f$2^n\f$ bits, assuming that no overflows occur.
#define VM_OP_SHL       0x56U

/// \brief Logic shift right of operand \c a by the number of bits given in operand \c b. (\c SHR)
///
/// \par Bytecode
///  \verbatim | 0x57 |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c SHR instruction interprets both operands as unsigned 32-bit integers and performs a logical
/// shift right of operand \f$a\f$ by \f$b\f$ bit positions. The result \f$z\f$ of the shift operation
/// is pushed on the stack.  Newly shifted-in bit positions are filled with zero bits.
/// Shifting operand \f$a\f$ by more than 31 bits produces a zero result.
/// Newly shifted-in bit positions are filled with zero bits.
//
/// \note Logical shift right of an unsigned integer by \f$n\f$ bits is equivalent to truncating
///   (floor) division by \f$2^n\f$ bits.
#define VM_OP_SHR       0x57U

/// \brief Arithmetic shift right of operand \c a by the number of bits given in operand \c b. (\c ASR)
///
/// \par Bytecode
///  \verbatim | 0x58 |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c ASR instruction interprets operand \f$a\f$ as signed 32-bit integer and operand \f$b\f$ as
/// as unsigned 32-bit shift quantity integers. It then performs an arithemtic shift right of operand \f$a\f$
/// by \f$b\f$ bit positions. The result \f$z\f$ of the shift operation is pushed on the stack. Newly shifted-in
/// bit positions are filled with the sign bit of the original \f$a\f$ operand.
///
/// \note Arithmetic shift right of an signed integer by \f$n\f$ bits is equivalent to truncating
///   division by \f$2^n\f$ bits. Positive values are rounded towards zero. Negative values are rounded
///   towards \f$-\infty\f$, thus arithmetic shifting \f$-1\f$ by any number of bits again results in \f$-1\f$.
#define VM_OP_ASR       0x58U

/// \brief Logic rotate right of operand \c a by the number of bits given in operand \c b. (\c ROR)
///
/// \par Bytecode
///  \verbatim | 0x59 |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c ROR instruction interprets both operands as unsigned 32-bit integers and performs a logical
/// rotate right of operand \f$a\f$ by \f$b\f$ bit positions. The result \f$z\f$ of the shift operation
/// is pushed on the stack.  Newly shifted-in bit positions are filled with the shifted-out bit positions.
/// The shift-amount (\f$b\f$) is reduced modulo 32. (Shifting operand \f$a\f$ by any even multiple
/// of 32 bits does not change the result value).
#define VM_OP_ROR       0x59U

/// \brief Bit-wise logical AND of operand \c and operand \c b. (\c AND)
///
/// \par Bytecode
///  \verbatim | 0x5A |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c AND instruction interprets both operands as unsigned 32-bit integers and performs a bit-wise
/// AND \f$a \& b\f$ of both operands; the result \f$z\f$ is pushed on the stack.
#define VM_OP_AND       0x5AU

/// \brief Bit-wise inclusive OR of operand \c and operand \c b. (\c OR)
///
/// \par Bytecode
///  \verbatim | 0x5B |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c OR instruction interprets both operands as unsigned 32-bit integers and performs a bit-wise
/// inclusive OR \f$a | b\f$ of both operands; the result \f$z\f$ is pushed on the stack.
#define VM_OP_OR        0x5BU

/// \brief Bit-wise exclusive OR of operand \c and operand \c b. (\c XOR)
///
/// \par Bytecode
///  \verbatim | 0x5C |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a, b \rightarrow \ldots z\f$
///
/// The \c XOR instruction interprets both operands as unsigned 32-bit integers and performs a bit-wise
/// exclusive OR \f$a \oplus b\f$ of both operands; the result \f$z\f$ is pushed on the stack.
///
#define VM_OP_XOR       0x5CU

/// \brief Bit-wise NOT (one's complement) of operand \c a. (\c NOT)
///
/// \par Bytecode
///  \verbatim | 0x5D |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a \rightarrow \ldots z\f$
///
#define VM_OP_NOT       0x5DU

/// \brief Bit-wise NEG (two's complement) of operand \c a. (\c NOT)
///
/// \par Bytecode
///  \verbatim | 0xED |\endverbatim
///
/// \par Stack (value operands)
///   \f$ \ldots, a \rightarrow \ldots z\f$
///
/// The \c NEG instruction interprets its operand as  32-bit integer and performs
///  a unary minus (two's complement) operation; the result \f$z\f$ is pushed on the stack.
///
#define VM_OP_NEG       0x5EU

/// @}

//----------------------------------------------------------------------
/// \name Virtual Machine Calls
///
/// @{

/// \brief Virtual Machine Call (\c VMC).
///
/// \par Bytecode
///  \verbatim | 0x60 | call[7:0] | \endverbatim
///
/// \par Stack
///   \f$ \ldots [, args] \rightarrow \ldots [, rets] \qquad \textrm{(call dependent)}\f$
///
/// Virtual machine calls provide an extension mechanism for adding up to 256
/// application specific instructions to the tinyvm virtual machine. Logically
/// virtual machine calls are organized in 16 groups of up to 16 instructions
/// per group.
///
/// The inputs and outputs of this operation depend on the actual call. The
/// following calls are currently defined:
///
/// \li \ref VMC_EXIT (Exit from bytecode program)
/// \li \ref VMC_ASSERT (Assertion in a bytecode program)
/// \li \ref VMC_READ (Read a byte from the virtual input stream)
/// \li \ref VMC_WRITE (Write a byte to the virtual output stream)
///
/// \note Virtual machine calls can be used to add additional (application specific)
///   instructions in a simple way. The \c VMC instruction is implemented in \c vm_extra.c.
///   See the \ref VMC_CODE macro for a note on reserved and available call numbers.
///
#define VM_OP_VMC 0x60U // First virtual machine call instruction

/// \brief Helper macro to define virtual machine call codes.
///
/// This macro constructs a call number for use as immediate operand of
/// a \ref VM_OP_VMC instruction. There are 16 \c major call groups each allowing for
/// up to \c 16 minor calls.
///
/// The major call group 0 is reserved for the assignment specification. Major
/// groups 1-15 can be freely used (in case you want to add your own \c VMC
/// instructions).
///
/// \param major is the major call number.
///
/// \param minor is the minor call number
#define VMC_CODE(major,minor) (((major) << 4) | (minor))

/// \brief Exit from bytecode program. \c VMC(VM_EXIT)
///
/// \par Bytecode
///  \verbatim | 0x60 | 0x00 | \endverbatim
///
/// \par Stack
///   \f$ \ldots \rightarrow \ldots \f$
///
/// The \c VMC_EXIT virtual machine call signals the termination of the bytecode
/// program to the tinyvm virtual machine. Execution of the bytecode program is
/// immediately halted upon execution of this virtual machine call. (The \ref VmStep
/// function will return with exit status 1).
#define VMC_EXIT     VMC_CODE(0, 0)

/// \brief Assertion in a bytecode program. \c VMC(VM_ASSERT)
///
/// \par Bytecode
///  \verbatim | 0x60 | 0x01 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, test \rightarrow \ldots \f$
///
/// The \c VMC_ASSERT virtual machine call allows bytecode programs to perfom the
/// equivalent of a C \c assert assertions: The \c test operand is removed from the
/// operand stack and compared to zero. If the \c test condition is non-zero, execution
/// of the bytecode program will proceed normally (the assertion passes). If the \c test
/// condition is zero, execution of the bytecode program stops and the \ref VmStep
/// function will fail with exit status -1. (the assertion failed).
#define VMC_ASSERT   VMC_CODE(0, 1)

/// \brief Read a byte from the virtual input stream. \c VMC(VM_READ)
///
/// \par Bytecode
///  \verbatim | 0x60 | 0x02 | \endverbatim
///
/// \par Stack
///   \f$ \ldots \rightarrow \ldots, in \f$
///
/// The \c VMC_READ virtual machine call reads the next byte from the bytecode program's
/// virtual input stream and pushes it into the operand stack. The \c in result is either
/// the value of the read byte (0-255) or 0xFFFFFFFF if no (more) bytes are available
/// in the virtual machine's input stream. (This virtual machine call resembles the
/// standard C \c getchar function).
#define VMC_READ     VMC_CODE(0, 2)

/// \brief Write a byte to the virtual output stream. \c VMC(VM_WRITE)
///
/// \par Bytecode
///  \verbatim | 0x60 | 0x03 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, out \rightarrow \ldots \f$
///
/// The \c VMC_WRITE virtual machine call grabs the byte value to be written from the top
/// of the operand stack and writes it to the bytecode program's virtual output stream.
/// The \c out byte value is silently truncated to an 8-bit unsigned value. (This virtual
/// machine call resembles the standard C \c putchar function).
#define VMC_WRITE    VMC_CODE(0, 3)

/// @}

//----------------------------------------------------------------------
/// \name Optional extensions
///
/// @{
///

/// \brief Lowest object handle, which may be allocated by the \c VMC(VM_ALLOC)
///   function.
#define VM_DYNAMIC_HANDLE_FIRST 0x40000000U

/// \brief Highest object handle, which may be allocated by the \c VMC(VM_ALLOC)
///   function.
#define VM_DYNAMIC_HANDLE_LAST  0x40FFFFFFU

/// \brief (Permanently) allocates a new virtual machine object. \c VMC(VM_ALLOC)
///
/// \par Bytecode
///  \verbatim | 0x60 | 0x04 | \endverbatim
///
/// \par Stack
///   \f$ \ldots, size \rightarrow \ldots, handle \f$
///
/// The \c VMC_ALLOC virtual machine call permanently allocates a new data object of the
/// given size (in bytes) and pushes the handle of the new object to the virtual machine stack.
/// The handle returned by this opcode will be in range \ref VM_DYNAMIC_HANDLE_FIRST to
/// \ref VM_DYNAMIC_HANDLE_LAST.
///
/// Note that there is currently no way to deallocate the object. (It will remain visible
/// until the virtual machine is terminated).
#define VMC_ALLOC   VMC_CODE(0, 4)

/// @}

#endif // TINYVM_BYTECODE_H
