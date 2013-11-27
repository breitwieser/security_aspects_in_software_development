#
# Makefile for the tinybn big integer library.
#
# WARNING: THIS MAKEFILE IS PARSED BY THE TEST SYSTEM. IMPROPER
# MODIFICATIONS TO THIS FILE MAY BREAK YOUR BUILD ON IAIK'S TEST
# SYSTEM.
#
# YOU (ONLY) HAVE TO CHANGE THE MAKE VARIABLES MARKED WITH "TODO"
# TO MATCH YOUR IMPLEMENTATION OF THE TINYBN LIBRARY.

# Name of the tiny big integer library
#
# WARNING: DO NOT CHANGE THIS MAKE VARIABLE.
LIBRARY = tinybn

#
# Source files for the tinybn library (libtinybn.a)
#
# TODO: Edit this variable to include all source files required
# to compile your big integer (libtinybn.a) library. Note that
# the files listed in this make variable MUST NOT contain a
# "main" function.
#
SOURCES = 						\
	mp_add.c mp_sub.c mp_mul.c mp_cmp.c		\
	bigint_alloc.c bigint_misc.c bigint_compare.c	\
	bigint_addsub.c bigint_mul.c bigint_io.c


# Extra preprocessor macros for building the tinybn library.
#
# This make variable can be used if your program requires
# certain feature test macros to compile. See the feature_test_macros(7)
# manpage for more details. The _GNU_SOURCE macro enables various extension
# to the C99 standard (including functions like strdup, strndup, ...).
#
# TODO: Edit this variable if you use special preprocessor macros (e.g. "DEBUG")
# to control conditional compilation of your code.
DEFINES = _GNU_SOURCE

# Functions to be formally verified for the optional Frama-C bonus task.
#
# TODO: Leave the PROOF variable commented out, if you did not do the
# Frama-C bonus task.
#
# If you did the Frama-C bouns task: Uncomment the PROOF variable and
# list the functions in your implementation, for which you wrote
# provable ACSL annotations.
#
# PROOF =MpAdd MpSub MpMul MpCompare

include rules.mk
