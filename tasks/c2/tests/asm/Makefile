#
# Makefile for the assembler tests
#
# This makefile can safely be edited; it is not
# used by the automated test system.
#

# Include directories for the assembler test suite.
INCLUDES  = ../../assembler ../common

PROGRAM   = asmtests.c
SOURCES   = tokens.c buffer.c lexer.c parser.c
LIBS      = assembler util testing cunit

# Resource files (text and binary) to be embedded
RESOURCES = src/simple.txt

TOP       = ../..
include $(TOP)/build.mk
