#
# Makefile for the big-integer arithmetic tests.
#
# This makefile can safely be edited; it is not
# used by the automated test system.
#

# Include directories for the assembler test suite.
INCLUDES  = ../../src ../common

PROGRAM   = tinybntests.c
SOURCES   = 		 	\
	test_mp_addsub.c 	\
	test_mp_mul.c		\
	test_mp_compare.c 	\
	test_bigint_basic.c	\
	test_bigint_io.c 	\
	test_bigint_internals.c \
	test_bigint_common.c

LIBS      = tinybn testing cunit

TOP       = ../..
include $(TOP)/build.mk
