///
/// \file
/// \brief Public API interface of the tiny virtual machine.
///
#ifndef TINYVM_H
#define TINYVM_H 1

// Utility library (buffers, memory views)
#include "util/buffer.h"
#include "util/memview.h"

// Bytecodes (and bytecode revision) of the virtual machine
#include "bytecodes.h"

// Additional headers
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

/// \defgroup VmCoreApi TinyVM Virtual Machine Public API
//----------------------------------------------------------------------

/// \name Virtual machine control
/// \ingroup VmCoreApi
/// @{

/// \brief Virtual machine context.
///
/// This data structure encapsulates an instance of the tinyvm virtual
/// machine.
typedef struct VmContext VmContext;

/// \brief Read callback of the virtual machine.
///
/// This callback type is used by the the implementation of the
/// \ref VMC_READ virtual machine call to read the next character
/// from the virtual machine's input stream.
///
/// \param vm is the virtual machine context doing the read.
///
/// \return The value of the read byte as unsigned char (0-255).
///
/// \return -1 if an error occurred, while reading a byte.
///
/// \return -2 if no more data is available to read.
typedef int (*VmIoRead)(VmContext *vm);

/// \brief Creates a new tinyvm instance.

/// \brief Write callback of the virtual machine.
///
/// This callback type is used by the the implementation of the
/// \ref VMC_WRITE virtual machine call to write the next character
/// to the virtual machine's output stream.
///
/// \param vm is the virtual machine context doing the read.
///
/// \param c is the byte to be written to the output stream.
///
/// \return \c true on success.
///
/// \return \c false on error.
typedef bool (*VmIoWrite)(VmContext *vm, unsigned char c);

/// \brief Creates a new tinyvm instance.
///
/// This functions creates and initializes a new tinyvm instance.
/// No bytecode or data objects are loaded for the returned virtual
/// machine object.
///
/// \param max_stack is the maximum number of stack operand stack
///   slots the virtual machine instance will support.
///
/// \param appdata is an optional application data pointer to
///   be associated with the virtual machine. This pointer is stored
///   without any changes in the \ref VmContext structure and can
///   be retrieved using the \ref VmGetAppData function.
///
/// \param ioread is an optional callback of type \ref VmIoRead
///   which will be used by the \ref VMC_READ virtual machine call
///   to read the virtual machine's input stream.
///   (This parameter can be NULL, to indicate an empty input stream)
///
/// \param iowrite is an optional callback of type \ref VmIoWrite
///   which will be used by the \ref VMC_WRITE virtual machine call
///   to write to the virtual machine's output stream.
///   (This parameter can be NULL, to discard any \ref VMC_WRITE output)
///
/// \return The newly created virtual machine object on success.
///
/// \return NULL on error (for example if there is insufficient
///   memory to allocate the virtual machine)
VmContext* VmCreate(size_t max_stack, void *appdata,
                    VmIoRead ioread, VmIoWrite iowrite);

/// \brief Deletes a virtual machine instance.
///
/// This function deletes a virtual machine object and releases
/// all associated resources.
///
/// \param vm the virtual machine to be deleted.
void VmDelete(VmContext *vm);

/// \brief Gets the application data pointer of a virtual machine.
///
/// This functions retrieves the application specific \c appdata pointer
/// which was passed to \ref VmCreate when the given VM was created. The
/// interpretation of the returned pointer is up to the application.
///
/// \param vm is the virtual machine to query.
///
/// \return The \c appdata pointer which was passed to \ref VmCreate when
///   the virtual machine was created (the returned pointer may be NULL).
///
/// \return NULL if \c vm is NULL.
void* VmGetAppData(VmContext *vm);

/// \brief Executes the next bytecode instruction. (singlestep execution)
///
/// This function fetches and executes the next bytecode instruction. The
/// return value of this function indicates, if the instruction was executed
/// successfully, if the program completed execution, or if an error occurred.
///
/// This function is typically used in a loop, similar to the following
/// code snippet:
/// \code
/// ...
/// VmContext *vm = ...; // Initialization omitted for brevity
/// ...
/// // Execute until either the program is done or an error occurs
/// int status;
/// do {
///   status = VmStep(vm);
/// } while (status == 0);
/// ...
/// assert (status == 1 || status == -1);
/// ...
/// if (status == -1) {
///   ... // VM error detected
/// } else if (status == 1) {
///   ... // Bytecode program terminated (VM(VMC_EXIT))
/// }
/// ...
/// \endcode
///
/// \param vm is the virtual machine executing the instruction.
///
/// \return +1 if the bytecode program terminated (in response to a \c VM_EXIT
///    virtual machine call)
///
/// \return 0 if an instruction was executed successfully, and there is
///    more work to do.
///
/// \return -1 if the virtual machine encountered an error.
int VmStep(VmContext *vm);

/// \brief Loads bytecode from a memory buffer.
///
/// This function loads a chunk of bytecode found in the \c source
/// buffer into a virtual machine and creates appropriate function and
/// data objects.
///
/// This operation replaces any existing bytecode, which may have
/// been loaded earlier by the same virtual machine, clears the operand
/// stack and sets the virtual program counter to the entrypoint of the
/// loaded bytecode program.
///
/// \note The data blob contained in the source buffer must be in the
///  format produced by the tinyasm assembler. See the \c vm/bytecodes.h header
///  file and \c assembler/asm_output.c for more details on the exact format.
///
/// \param vm is the destination virtual machine context.
///
/// \param source is a buffer containing the chunk of bytecode to load.
///
/// \return \c true if the bytecode was loaded successfully.
///
/// \return \c false if the bytecode could not be loaded completely.
bool VmLoadByteCode(VmContext *vm, Buffer *source);

/// \brief Loads bytecode from a file.
///
/// This functions loads a chunk of bytecode from a normal disk
/// file. Internally this function first fills a byte buffer with the
/// content of the specified file and then delegates to \ref VmLoadByteCode
/// for the actual loading process.
///
/// \param vm is the destination virtual machine context.
///
/// \param filename is the name of the input file to be loaded.
///
/// \return \c true if the bytecode was loaded successfully.
///
/// \return \c false if the bytecode could not be loaded completely.
///
/// \see  VmLoadByteCode for more details on this function.
bool VmLoadFile(VmContext *vm, const char *filename);
/// @}

//----------------------------------------------------------------------
/// \name Access to the virtual machine execution state
///
/// The complete execution state of the tinyvm virtual machine is defined
/// by all loaded objects, the operand stack, the virtual frame pointer,
/// the handle of the currently executing function and the current program
/// counter.
///
/// The \ref VmRegisters data structure and the \ref VmGetRegs and
/// \ref VmSetRegs functions allow access and modification to the
/// virtual frame pointer, the active function handle and the current
/// program counter.
///
/// @{

/// \brief Register of the virtual machine.
///
/// This data structure contains all virtual register which are
/// required to capture the current execution state of the tinyvm
/// virtual machine.
typedef struct VmRegisters {
  /// \brief The currently executing function.
  ///
  /// This field contains the handle of the (code) object containing
  /// the currently executing function. It may be \c VM_NULL_HANDLE if
  /// the virtual machine is currently not executing any functions.
  ///
  /// The handle can be used with \ref VmGetObject to fetch the
  /// \ref VmObject corresponding to the function. This field
  /// is \ref VM_NULL_HANDLE on startup and will be set to the
  /// entrypoint function of a bytecode program by \ref VmLoadByteCode.
  uint32_t function;

  /// \brief Virtual program counter.
  ///
  /// This function contains the current virtual program counter
  /// of the virtual machine. The virtual program counter is an
  /// index into the byte-code region of the currently executing
  /// function. (Program counter value zero refers to the first
  /// bytecode instruction in the function; to get the raw index
  /// of the program counter within the data-region  of the function's
  /// \ref VmObject, the value \ref VM_FUNCTION_BYTECODE_OFFSET has
  /// to be added to this value),
  ///
  /// The \ref VmLoadByteCode function will reset this field
  /// to zero (to start execution at the first bytecode of the
  /// entrypoint function)
  uint32_t pc;

  /// \brief Virtual frame pointer.
  ///
  /// This field contains the stack index of the virtual frame
  /// pointer of the currently executing function. The index stored
  /// in this field is counted from the bottom of stack, and can
  /// be used in combination with \ref VmStackAt to access elements
  /// of the (virtual) stack frame.
  ///
  /// A virtual stack frame is laid out as follows:
  ///
  /// \li [fp-num_args-1] to [fp-1] contain the function
  ///     arguments (the first argument is in [fp-num_args-1])
  ///
  /// \li [fp+0] is the function handle of the calling function
  ///
  /// \li [fp+1] is the (return) program counter value in the valling function.
  ///
  /// \li [fp+2] is the (old) frame pointer value of the calling function.
  ///
  /// \li [fp+3] to [fp+num_locals-1] contain the local variables.
  ///
  /// \li Slots above [fp+num_locals+0] contain the evaluation stack.
  uint32_t fp;
} VmRegisters;

/// \brief Gets the execution state of the virtual machine.
///
/// This functions gets the active virtual registers of the
/// virtual machine.
///
/// \param[out] regs is the data structure reciving a copy of the
///   current virtual register values.
///
/// \param[in] vm is the virtual machine to query.
///
/// \return \c true if the virtual register were read successfully.
///
/// \return \c false in case of an error.
bool VmGetRegs(VmRegisters *regs, VmContext *vm);

/// \brief Sets the execution state of the virtual machine.
///
/// This functions sets the active virtual registers of the
/// virtual machine. This function is atomic: Either
/// all virtual register are updated or no virtual register
/// are updated.
///
/// \param[in] vm is the virtual machine to modify.
///
/// \param[in] regs is the data structure with the new values of the
///   virtual registers.
///
/// \return \c true if the virtual register were successfully set
///   to the values in \c regs.
///
/// \return \c false in case of an error.
bool VmSetRegs(VmContext *vm, const VmRegisters *regs);

/// @}

//----------------------------------------------------------------------
/// \name Objects
///
/// From the bytecode perspective virtual machine objects can be referenced
/// throught 32-bit integer object handles. The virtual machine APi includes
/// functions which allow direct creation, inspection, and manipulation
/// of objects from within native code.
///
/// @{

/// \brief Virtual machine objects.
///
/// This data structure encapsulates a virtual machine object.
typedef struct VmObject VmObject;

/// \brief Creates a new object in context of the given virtual machine.
///
/// This function creates a new object in context of the given virtual
/// machine. The handle for the new object is given by the \c handle
/// parameter. The type and nature of the new object is defined by the
/// qualifiers given in \c qualifiers. The (code or data) conrent of the
/// new object is defined by \c content (the length is given by \c length).
///
/// \note See the comments in \c bytecodes.h for more details on the
///   contents of function and data object objects. Data objects just
///   contain the raw data. Code objects contain a small header, followed
///   by the bytecode of the function.
///
/// \param vm is the virtual machine for which the object will be created.
///
/// \param handle is the handle of the object to be created. Object creation
///   will fail, if the handle is either the null handle(\ref VM_NULL_HANDLE)
///   or if an object with the same handle already exists in this VM.
///
/// \param qualifiers are the qualifiers of the new object.
///
/// \param content is the initial content data of the new object. (This
///    parameter can be NULL to create a zero-initialized object).
///
/// \param length is the length of the object content.
///
/// \return \c true if the object was created successfully.
///
/// \return \c false if the object could not be created due to an error.
///   Some possible error cases would be out of memory conditions, bad
///   arguments, duplicate object handles, etc.
///
bool VmCreateObject(VmContext *vm, uint32_t handle, VmQualifiers qualifiers,
                    const unsigned char *content, size_t length);

/// \brief Looks up a virtual machine object by its handle.
///
/// This function searches for an object using the object's handle. On
/// success this function returns a pointer to the virtual machine object
/// in question. The returned \ref VmObject pointer is guranteed to remain
/// valid until either the virtual machine is deleted, or until the next
/// call of \ref VmLoadByteCode on the same virtual machine.
///
/// \param vm is the virtual machine to scan.
///
/// \param handle is the handle of the virtual machine object to find.
///
/// \return A pointer to the \ref VmObject structure if an object associated
///   with the given handle was found.
///
/// \return NULL if no object was found, if \ref VM_NULL_HANDLE was given
///   as \c handle parameter, or if an error occurred.
VmObject* VmGetObject(VmContext *vm, uint32_t handle);

/// \brief Gets the handle of a virtual machine object.
///
/// This function returns the handle of a given virtual machine object. The
/// NULL pointer is translated to the \ref VM_NULL_HANDLE handle.
///
/// \param obj is the object to be queried.
///
/// \return The handle of the given object.
///
/// \return NULL if and only if \c NULL was passed for \c obj.
uint32_t VmGetHandle(VmObject *obj);

/// \brief Gets the parent virtual machine context of an object.
///
/// This functions returns the parent virtual machine context of a given
/// object. The parent context of an object is set at creation time
/// (when the object is constructed with \ref VmCreateObject) and stays
/// the same over the entire lifetime of the object.
///
/// \param obj is the object to be queried.
///
/// \return The parent context of the object on success.
///
/// \return NULL if and only if \c obj is NULL.
VmContext* VmGetContext(VmObject *obj);

/// \brief Initializes a memory view to access the content of an object.
///
/// This function initializes the memory view given in the \c view parameter,
/// to allow access to the object's content. The memory view is initialized
/// using \ref MemInit.
///
/// \param[out] view is the memory view to be initialized by this function.
///   The state of the memory view is undefined, if this function fails. (The
///   caller must not attempt to call any memory view functions other than
///   \ref MemInit on the view, if this function fails).
///
/// \param obj is the object to be accessed.
///
/// \return \c true if the memory view was initialized successfully.
///
/// \return \c false on error.
bool VmAccessObject(MemView *view, VmObject *obj);

/// @}

//----------------------------------------------------------------------
/// \name Operand stack access
/// @{

/// \brief Pushes values to the virtual machine stack.
///
/// This functions pushes \c count values found in the \c values array
/// to the operand stack of a virtual machine. Values are pushed in the
/// order in which they appear on the array. (the first element of the array
/// is pushed first, ...)
///
/// This operation is atomic: Either all values are pushed successfully,
/// or no change is made to the stack (in case of an error).
///
/// \param[in,out] vm is the virtual machine context which owns the target stack.
///
/// \param[in] values is an optional array of values to be pushed onto the
///   operand stack. (This parameter can be NULL to push a given number of
///   zero slots onto the stack).
///
/// \param count is the number of values in the \c values array.
///
/// \return \c true if all \c count values in the \c values array have been
///   pushed to the operand stack.
///
/// \return \c false if the operation failed. (in case of an error, no values
///   will be pushed to the stack.)
bool VmStackPush(VmContext *vm, const uint32_t *values, size_t count);

/// \brief Pops values from the virtual machine stack.
///
/// This function removes \c count values found at the top of virtual machine
/// and optionally stores them in the \c values array. The order of elements
/// corresponds to the order in which the values are removed from the stack.
/// (The previous top element will be stored in the first position of the
/// values array).
///
/// The \c values array can be set to NULL, if the values from the stack
/// are not needed. A simple way to clear the operand stack (remove all
/// elements) is:
/// \code
/// ...
/// VmStackPop(NULL, vm, VmStackGetUsage(vm));
/// ...
/// \endcode
///
/// This operation is atomic: Either all values are removed successfully,
/// or no change is made to the stack (in case of an error).
///
/// \param[out] values is an optional array receiving the values from the
///  top of the operand stack.
///
/// \param[in,out] vm is the virtual machine which owns the target stack.
///
/// \param count is the number of elements to be removed from the target
///   stack.
///
/// \return \c true if all \c count values in the \c values array have been
///   removed from the operand stack (and stored in the \c values array, if
///   a non-NULL \c values array was given).
///
/// \return \c false if the operation failed, for example if the stack
///   contains less than \c count values, or if an invlaid parameter was
///   specified. (in case of an error, no values  will be removed from the stack.)
bool VmStackPop(uint32_t *values, VmContext *vm, size_t count);

/// \brief Returns the number of currently used stack slots of a virtual machine.
///
/// \param[in,out] vm is the virtual machine which owns the target stack to query.
///
/// \return The number of stack currently used stack slots of a virtual machine.
///
/// \return Zero if either the stack of the given virtual machine is empty or
///   if \c NULL is given as \c vm parameter.
size_t VmStackGetUsage(VmContext *vm);

/// \brief Gets a pointer to a stack element, index relatively to the bottom
///   of the stack.
///
/// This function returns a pointer to a stack element, indexed relative to
/// the bottom of the operand stack. The \c index parameter gives the distance of
/// the requested element from the bottom of the stack; index 0 refers to the bottom
/// of the stack, the top of stack element is at index \ref VmStackGetUsage(...)-1.
///
/// The pointer returned by this function may become invalid if the stack
/// grows or shrinks (due to \ref VmStackPush and \ref VmStackPop calls).
///
/// \param vm is the virtual machine which owns the target stack.
///
/// \param index is the index of the element to retrieve.
///
/// \return A pointer to the requested stack element. This pointer can be
///  used to read or write the given element. It may becomes invalid if the size
///  of the stack changes due to a push or pop operation.
uint32_t* VmStackAt(VmContext *vm, size_t index);

/// @}

//----------------------------------------------------------------------
/// \name Auxilary logging functions
/// @{

/// \brief Message log level of the virtual machine.
typedef enum VmLogLevel {
  /// \brief Message is an error message.
  VM_LOG_ERROR = 0,

  /// \brief Message is a warning message.
  VM_LOG_WARNING = 1,

  /// \brief Message is an informational message.
  VM_LOG_INFO  = 2,

  /// \brief Message is an execution trace message.
  VM_LOG_TRACE = 3,

  /// \brief Messahe is a debug message.
  VM_LOG_DEBUG = 4,
} VmLogLevel;

/// \brief Sets the log stream of a virtual machine.
///
/// This function sets a new log stream for the virtual machine. Ownership
/// of the given stream remains with the caller. It is the caller's
/// responsibility to ensure that the stream remains valid and open for the
/// remaining lifetime of the virtual machine, or at least until a new log
/// stream has been set successfully.
///
/// \param vm the virtual machine to modify.
///
/// \param stm the new log output stream of the virtual machine. This
///   parameter can be NULL to completely disable log output from the
///   virtual machine.
///
/// \return \c true if the log stream was set successfully.
///
/// \return \c false if the log stream could not be set (
bool VmSetLogStream(VmContext *vm, FILE *stm);

/// \brief Gets the current log stream of a virtual machine.
///
/// This function queries the current log stream of a virtual machine.
/// Calling this function with a NULL argument returns the default log
/// stream (stderr).
///
/// \param vm is the virtual machine to query, or NULL to get the default
///  log stream.
///
/// \return The current log stream of the given virtual machine (or the
///   default log stream if NULL was passed as argument).
FILE* VmGetLogStream(VmContext *vm);

/// \brief Sets the current log level of a virtual machine.
///
/// This function sets a new log level stream for the given virtual machine.
/// The log level controls the amount of log output produced by a virtual
/// machine.
///
/// \param vm is the virtual machine to modify.
///
/// \param level is the new log level for the virtual machine.
///
/// \return \c true if the log level was set successfully.
///
/// \return \c false if the log stream level couuld not be set, either
///   because \c vm was NULL, or because \c level was not valid.
bool VmSetLogLevel(VmContext *vm, VmLogLevel level);

/// \brief Gets the current log level of a virtual machine.
///
/// This functions gets the current log level of a virtual machine. The
/// current log level is the numerically greatest log level, which is
/// logged.
///
/// \param vm is the virtual machine to query, or NULL to get the default
///  log level.
///
/// \return The current log level of the given virtual machine, or the
///   default log level if NULL was given as log level.
VmLogLevel VmGetLogLevel(VmContext *vm);

/// \brief Logs a diagnostic message for a virtual machine.
///
/// This function logs a diagnostic message for a virtual machine. It
/// uses \ref VmGetLogLevel and \ref VmGetLogStream to determine the
/// log level filter and log message target.
///
/// No output is produced if the log level given in \c level
/// is more verbose than the active log level, or if the active
/// log stream is NULL.
///
/// \param vm is the virtual machine generating the log message. This
///   parameter can be NULL if no virtual machine object is available.
///
///   The VM parameter is used to determine the log level filter and
///   and log stream. See \ref VmGetLogLevel and \ref VmGetLogStream for
///   details on handling of NULL values for this parameter.
///
/// \param level is the log level of the message being logged.
///   The actual log level given in \c level must be numerically less
///   or equal than the log level report by \ref VmGetLogLevel to
///   produce any output.
///
/// \param message is a format string for the log message, which will
///   be directly passed to the \c vfprintf function.
///
/// \param ... is the format specific argument list, which will be
//    directly passed to the  \c vfprintf function.
void VmLog(VmContext *vm, VmLogLevel level, const char *message, ...)
  __attribute__((format(printf, 3, 4)));

/// \brief Logs a VM error message.
///
/// \see \ref VmLog for more details.
#define VmLogError(vm,...) VmLog((vm), VM_LOG_ERROR, __VA_ARGS__)

/// \brief Logs a VM warning message.
///
/// \see \ref VmLog for more details.
#define VmLogWarn(vm,...) VmLog((vm), VM_LOG_WARNING, __VA_ARGS__)


/// \brief Logs a VM info message.
///
/// \see \ref VmLog for more details.
#define VmLogInfo(vm,...) VmLog((vm), VM_LOG_INFO, __VA_ARGS__)

/// \brief Logs a VM trace message.
///
/// \see \ref VmLog for more details.
#define VmLogTrace(vm,...) VmLog((vm), VM_LOG_TRACE, __VA_ARGS__)

/// \brief Logs a VM debug message.
///
/// \see \ref VmLog for more details.
#define VmLogDebug(vm,...) VmLog((vm), VM_LOG_DEBUG, __VA_ARGS__)

/// @}
#endif // TINYVM_H
