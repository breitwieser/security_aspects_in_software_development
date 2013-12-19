#
# Makefile for the tinyasm assembler
#

# List of source files for the assembler library (libassembler.a)
#
# TODO: Edit this variable to include all source files required
# to compile your assembler (libassembler.a) library. Note that
# the files listed in this make variable MUST NOT contain a
# "main" function.
#
SOURCES = 						\
	asm_lexer.c asm_tokens.c asm_buffer.c 		\
	asm_string_lexer.c asm_file_lexer.c 		\
	asm_parser_utils.c asm_errors.c asm_misc.c	\
	asm_variables.c 				\
	asm_object.c asm_code.c asm_relocation.c        \
	asm_forward.c asm_util_function.c		\
	asm_linker.c asm_output.c asm_tobuffer.c


# Main source file for standalone "tinyasm" application.
#
# This make variable should list EXACTLY one source file,
# which MUST contain a "main" routine.
#
# WARNING: DO NOT CHANGE THIS VALUE. CHANGES _WILL_  BREAK
# YOUR BUILD ON THE BUILD SYSTEM.
PROGRAM = tinyasm.c

# Extra preprocessor macros for building the tiny VM assembler.
#
# This make variable can be used if your program requires
# certain feature test macros to compile. See the feature_test_macros(7)
# manpage for more details. The _GNU_SOURCE macro enables various extension
# to the C99 standard (including functions like strdup, strndup, ...).
DEFINES = _GNU_SOURCE

# Name of the assembler library without ("lib" prefix and ".a" suffix)
#
# WARNING: DO NOT CHANGE THIS VALUE. CHANGES _WILL_ BREAK
# YOUR BUILD ON THE BUILD SYSTEM.
LIBRARY = assembler


include tinyasm.mk
