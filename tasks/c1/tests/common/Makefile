#
# Makefile for the common testing library
#
# This makefile can safely be edited; it is not
# used by the automated test system.
#
LIBRARY  = testing
SOURCES  = testrunner.c testallocs.c testdefaults.c
DEFINES  = _GNU_SOURCE

#ifdef REFERENCE_C2
TASK_EXTRA_FILES = Makefile testhelpers.h
#endif
TOP      = ../..
include $(TOP)/build.mk
