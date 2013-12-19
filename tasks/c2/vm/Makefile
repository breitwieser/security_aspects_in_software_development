#
# Makefile for the tinyvm virtual machine
#

# List of source files for the virtual machine (libvm.a)
#
# TODO: Edit this variable to include all source files required
# to compile your virtual machine (libvm.a) library. Note that
# the files listed in this make variable MSUT NOT contain a
# "main" function.
#
SOURCES = vm_context.c vm_objects.c vm_bytecodes.c vm_error.c   \
	  vm_loader.c vm_stack.c vm_call.c vm_ldst.c vm_arith.c \
	  vm_extra.c

# Extra preprocessor macros for building the tiny VM assembler.
#
# This make variable can be used if your program requires
# certain feature test macros to compile. See the feature_test_macros(7)
# manpage for more details. The _GNU_SOURCE macro enables various extension
# to the C99 standard (including functions like strdup, strndup, ...).
DEFINES = _GNU_SOURCE

# Main source file for standalone "tinyvm" application.
#
# This make variable should list EXACTLY one source file,
# which MUST contain a "main" routine.
PROGRAM = tinyvm.c


# Extra include directories (none)
INCLUDES =

# Extra library search paths (none)
LIBDIRS =

# Extra libraries (libutil.a)
LIBS = util

# Name of the virtual machine library without ("lib" prefix and ".a" suffix)
LIBRARY = vm


include tinyvm.mk
