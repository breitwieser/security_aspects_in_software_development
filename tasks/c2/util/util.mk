#
# Fixed build configuration for the tinyvm virtual machine.
#
# WARNING: DO NOT CHANGE THIS FILE UNLESS IT IS
# ABSOLUTELY NECESSARY (WHICH NEVER SHOULD BE THE
# CASE). CHANGES TO THIS FILE MAY BREAK THE BUILD
# SYSTEM.
#

# Path to the top-level directory
TOP = ..

# Name of the assembler library without ("lib" prefix and ".a" suffix)
#
# WARNING: DO NOT CHANGE THIS VALUE. CHANGES _WILL_
# BREAK YOUR BUILD ON THE BUILD SYSTEM.
LIBRARY = util

# Extra include directories (none by default)
#
# Only relative directories may be specified by this variable. The
# build systems allows "../" only at the beginning of a directory
# name.
#
# WARNING: DO NOT CHANGE THIS VALUE UNLESS YOU _REALLY_ KNOW WHAT
# YOU ARE DOING. IMPROPER MODIFICATIONS TO THIS VARIABLE MAY BREAK
# YOUR BUILD ON THE BUILD SYSTEM.
INCLUDES =

include $(TOP)/build.mk
