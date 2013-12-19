#
# Makefile for the virtual machine tests
#
# This makefile can safely be edited; it is not
# used by the automated test system.
#

# Include directories for the virtual machine test suite.
INCLUDES  = ../../vm ../common

PROGRAM   = vmtests.c
SOURCES   = exec.c
LIBS      = vm util testing cunit

# Test resources
RESOURCES = bin/hello.bin.be \
	    bin/hello.bin.le \
            bin/rot13.bin.be \
            bin/rot13.bin.le

TOP       = ../..
include $(TOP)/build.mk
