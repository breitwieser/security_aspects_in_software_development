#
# Subproject build configuration for the tinybn big integer library.
#
# WARNING: THIS FILE IS PART OF THE ASSIGNMENT SPECIFICATION AND
# MODELS THE FUNCTIONALITY AND BEHAVIOR OF IAIK'S TEST SYSTEM. ANY
# CHANGES TO THIS FILE WILL BE IGNORED BY THE ACTUAL TEST SYSTEM.
#

# Path to the top-level directory
TOP = ..

# Extra include directories (none)
INCLUDES =

# No additional library
LIBDIRS =

# Name of the virtual machine library without ("lib" prefix and ".a" suffix)
LIBRARY = tinybn

include $(TOP)/build.mk
