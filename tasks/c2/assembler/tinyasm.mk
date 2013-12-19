#
# Fixed build configuration for the tinyasm assembler.
#
# WARNING: DO NOT CHANGE THIS FILE UNLESS IT IS
# ABSOLUTELY NECESSARY (WHICH NEVER SHOULD BE THE
# CASE). CHANGES TO THIS FILE MAY BREAK THE BUILD
# SYSTEM.
#

# Path to the top-level directory
TOP = ..


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

# List of input files to the gperf static hash table generator
#
# This variable lists a number of input files for the GNU gperf
# tool. Each listed file will be processed with "gperf" to
# generate a C source file. The generated source files are
# automatically appended to the "SOURCES" variable by the build
# system.
HASHES  = asm_keywords.gperf asm_vmcallids.gperf

# List of grammar input files for the Lemon parser generator.
#
# This variable lists a number of input files for the Lemon parser
# generator tool. Each file listed here will be processed with
# "lemon" to generate a C source file, a C header file and a
# gramamr log file. The generated C source files are automatically
# appended to the "SOURCES" variable by the build system.
PARSERS = grammar.y

# Extra library search paths (none by default)
LIBDIRS =

# Additional libraries (the assembler links against the
# libutil.a library)
LIBS = util

include $(TOP)/build.mk
