#
# Common make rules used by IAIK's build infrastructure.
#
# WARNING: DO NOT CHANGE THIS FILE UNLESS IT IS
# ABSOLUTELY NECESSARY (WHICH NEVER SHOULD BE THE
# CASE). CHANGES TO THIS FILE MAY BREAK THE BUILD
# SYSTEM.
#

#
# The TOP make variable must be defined as relative path of the
# assignment top directory.
#
ifndef TOP
$(error The TOP make variable must be defined as relative path to the \
  top-level build directory)
endif

#----------------------------------------------------------------------
# Default values for customizable variables
#

# INCLUDES: Extra include directories
INCLUDES ?=

# LIBDIRS: Extra library search directories
LIBDIRS ?=

# LDLIBS: Extra libraries
LIBS ?=

# DEFINES: Extra preprocessor defines
#
#
DEFINES ?=

# COVERAGE: Compile coverage testing support
#
# Allowed values:
#   no   - (default) build without coverage testing
#   gcov - Build for coverage testing with gcov/lcov
#
COVERAGE ?= no

# Extra compiler and linker flags
EXTRA_CFLAGS  ?=
EXTRA_LDFLAGS ?=

# Debugger flags
DBG_CFLAGS  ?= -g
DBG_LDFLAGS ?= -g

#----------------------------------------------------------------------
# Add default make settings
#

# Build output directory
BINDIR := $(TOP)/bin

# Add public include directory to default search path
INCLUDES += $(TOP)

# Add binaries directory as default library search directory
LIBDIRS += $(BINDIR)

#----------------------------------------------------------------------
# Compiler and toolchain base configuration
#

# Toolchain profile (default: clang)
COMPILER ?= clang

# Cross-compiler prefix
CROSS_COMPILER ?=

# Flags common to all toolchains
CFLAGS  := -Wall -Wextra -Wno-unused-parameter -pedantic -std=c99 -MMD
LDFLAGS :=
LDLIBS  := $(LIBS:%=-l%)

# Toolchain specific
ifeq ("$(COMPILER)", "gcc")

 # GCC toolchain profile
 CC := $(CROSS_COMPILE)gcc
 LD := $(CROSS_COMPILE)gcc
 AR := $(CROSS_COMPILE)ar rcs

 EXESUFFIX :=
 OBJSUFFIX := .o
 DEPSUFFIX := .d
 LIBNAMESUFFIX :=
 LIBSUFFIX := .a

else ifeq ("$(COMPILER)", "clang")

 # LLVM native (clang) toolchain profile
 CC := $(CROSS_COMPILE)clang
 LD := $(CROSS_COMPILE)clang
 AR := $(CROSS_COMPILE)ar rcs

 EXESUFFIX :=
 OBJSUFFIX := .o
 DEPSUFFIX := .d
 LIBNAMESUFFIX :=
 LIBSUFFIX := .a

else ifeq ("$(COMPILER)","bitcode")

 # LLVM bitcode (clang) toolchain profile
 #
 # UNSUPPORTED. Requires gold and the LLVMgold.so plugin (llvm-dev).
 $(warning Using UNSUPPORTED LLVM/Clang bitcode configuration)
 CC := $(CROSS_COMPILE)clang
 LD := $(CROSS_COMPILE)clang
 AR := $(CROSS_COMPILE)llvm-link -o

 LLVM_GOLD_PLUGIN ?= /usr/lib/LLVMgold.so
 CFLAGS  += -emit-llvm -Wall -Wextra -pedantic -std=c99 -MMD
 LDFLAGS += -Xlinker -plugin=$(LLVM_GOLD_PLUGIN) -Xlinker -plugin-opt=emit-llvm

 EXESUFFIX     := .bc
 OBJSUFFIX     := .bc.o
 DEPSUFFIX     := .bc.d
 LIBNAMESUFFIX := .bc
 LIBSUFFIX     := .bc.a

else
 # Sorry, unknown compiler profile
 $(error Unknown toolchain configuration '$(COMPILER)' selected by COMPILER \
  make variable. (Valid choices are: clang gcc bitcode))
endif

# Lemon parser generator
LEMON := lemon

# GNU gperf perfect hash function generator
GPERF := gperf

# Sed
SED := sed

# Perl and the bin2res.pl script
PERL    := perl -w
BIN2RES := $(PERL) $(TOP)/bin2res.pl

#----------------------------------------------------------------------
# Support for coverage testing with gcov/lcov
#

ifeq ("$(COVERAGE)", "gcov")
 # Coverage testing with gcov
 #
 # NOTE: May be problematic with certain version of clang
 ifneq ("$(COMPILER)", "gcc")
   $(warning Coverage testing builds may fail with clang configurations. \
     Either set COMPILER=gcc or disable coverage disable coverage testing \
     with COVERAGE=no.)
 endif

 # Enable coverage testing "--coverage" is understood by gcc and clang.
 # Clang builds however may fail (e.g. on Ubuntu and Debian systems) if
 # libprofile_rt.a is not installed as part of the clang package.
 #
 # Note that we build with "-DNDEBUG" to disable C "asserts" (which
 # would create a lot of branch coverage clutter)
 CFLAGS  += --coverage
 LDFLAGS += --coverage

else ifeq ("$(COVERAGE)", "no")
 # No coverage testing (nothing to do)

else
 # Sorry, unknown coverage testing options profile
 $(error Unknown configuration testing configuation '$(COVERAGE)' selected by COVERAGE \
  make variable. (Valid choices are: no gcov))
endif

#----------------------------------------------------------------------
# Assemble final compiler and linker flags
#

CFLAGS  += $(EXTRA_CFLAGS) $(DBG_CFLAGS) $(INCLUDES:%=-I%) $(DEFINES:%=-D%)
LDFLAGS += $(EXTRA_LDFLAGS) $(DBG_CFLAGS) $(LIBDIRS:%=-L%)

# Automatically generated files
GEN_FILES =

# Extra files and directories clean
BUILD_TARGETS     =
CLEAN_FILES       =
CLEAN_TARGETS     =
DISTCLEAN_TARGETS =
DISTCLEAN_FILES   =

# Object files
OBJECTS      = $(SOURCES:%.c=%$(OBJSUFFIX))
DEPENDS      = $(SOURCES:%.c=%$(DEPSUFFIX))
CLEAN_FILES += $(OBJECTS)

#----------------------------------------------------------------------
# Compile rules
#

#
# Default make target ("make help")
#
help:
	@echo "Supported build targets:"
	@echo " make all          - Build all targets"
	@echo " make help         - Show this help file (default target)"
	@echo " make describe     - Describe the build steps taken by 'make all'."
	@echo " make clean        - Clean normal build outputs"
	@echo " make distclean    - Clean generated source files and build outputs"
	@echo ""
	@echo "Use 'make all' to build all targets in the current directory and its"
	@echo "subdirectories. A human readable description of the build steps is shown"
	@echo "by 'make describe'."
	@$(MAKE) -s help-local

help-local:

#
# Build subdirectories first
#
ifdef SUBDIRS
  BUILD_TARGETS += $(SUBDIRS:%=subdir-build-%)
  CLEAN_TARGETS += $(SUBDIRS:%=subdir-clean-%)
  DISTCLEAN_TARGETS += $(SUBDIRS:%=subdir-distclean-%)

  subdir-build-%:
	$(MAKE) -C $(@:subdir-build-%=%) all

  subdir-clean-%:
	-$(MAKE) -C $(@:subdir-clean-%=%) clean

  subdir-distclean-%:
	-$(MAKE) -C $(@:subdir-distclean-%=%) distclean

  subdir-describe-%:
	-@$(MAKE) --silent -C $(@:subdir-describe-%=%) describe

  # Force non-parallel execution of current Makefile when building subdirectories.
  # (Sub-makefile may still use subdirectories)
  .NOTPARALLEL:
endif

#
# Lemon parsers
#
ifdef PARSERS
 # Lemon generated parsers
 GEN_FILES     += $(PARSERS:%.y=%.c)
 GEN_FILES     += $(PARSERS:%.y=%.h)
 GEN_FILES     += $(PARSERS:%.y=%.out)
 SOURCES       += $(PARSERS:%.y=%.c)

 # Special make override to silence unused variable warnings
 # in lemon parser. (May be the case for token destructors which
 # do not need their extra argument)
 $(PARSERS:%.y=%.o): CFLAGS += -Wno-unused-variable \
	 -Wno-unused-but-set-variable
endif

#
# gperf perfect hash functions
#
ifdef HASHES
 # gperf generated static lookup tables
 GEN_FILES     += $(HASHES:%.gperf=%.c)
 SOURCES       += $(HASHES:%.gperf=%.c)
endif

#
# Build a static library if LIBRARY is given.
#
ifdef LIBRARY
 LIBRARY_FILE  := $(BINDIR)/lib$(LIBRARY)$(LIBSUFFIX)
 CLEAN_FILES   += $(LIBRARY_FILE)
 BUILD_TARGETS  += library

 library: $(LIBRARY_FILE)

 $(LIBRARY_FILE): $(OBJECTS)
	$(AR) $@ $(OBJECTS)
endif

#
# Link a standalone application if BUILD_PROGRAM
# is given.
#
ifdef PROGRAM
 PROGRAM_FILE  := $(BINDIR)/$(PROGRAM:%.c=%$(EXESUFFIX))
 MAIN_OBJECTS  := $(PROGRAM:%.c=%$(OBJSUFFIX))
 CLEAN_FILES   += $(PROGRAM_FILE) $(MAIN_OBJECTS)
 DEPENDS       += $(PROGRAM:%.c=%$(DEPSUFFIX))
 BUILD_TARGETS += program

 # Embedded binary resources
 ifdef RESOURCES
   # use the bin2res.pl script for conversion
   GEN_FILES    += _resources.c
   SOURCES      += _resources.c

   # generate the resources
   _resources.c: $(RESOURCES)
	$(BIN2RES) -i $(RESOURCES) > $@
 endif


 program: $(PROGRAM_FILE)

 ifdef LIBRARY
  # We are building the program and a static library,
  # link against the static library
  $(PROGRAM_FILE): $(MAIN_OBJECTS) $(LIBRARY_FILE)
	$(LD) -o $@ $(LDFLAGS) $(MAIN_OBJECTS) -L. -l$(LIBRARY)$(LIBNAMESUFFIX) $(LDLIBS)
 else
  # We are only building the program, link directly against
  # the object files.
  $(PROGRAM_FILE): $(MAIN_OBJECTS) $(OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $(MAIN_OBJECTS) $(OBJECTS) $(LDLIBS)
 endif
endif

# Refuse the build if we have resource but no program
# (this avoids all kind of string linker problems when having multiple
#  resource lookup routines with the same name in libraries and programs.)
ifdef RESOURCES
 ifndef PROGRAM
$(error Embedding of binary resources with RESOURCES is only possible when building
  a program (PROGRAM))
 endif
endif

#
# Build all default targets
#
all: $(BUILD_TARGETS)

#
# Clean build products
#
clean: $(CLEAN_TARGETS)
	rm -f $(CLEAN_FILES) vgcore.* core.* core

#
# Clean build products and generated files
#
distclean: $(DISTCLEAN_TARGETS)
	rm -f $(CLEAN_FILES) $(DISTCLEAN_FILES) $(GEN_FILES) $(DEPENDS) vgcore.* core.* core *.gcda *.gcno

#
# Generate all auto-generated source files
#
generate: $(GEN_FILES)

#
# Scan with the clang static analyzer
#
clang-scan:
	scan-build -o _scans -k $(MAKE) COMPILER=clang clean all

#
# Describes the build-steps taken by "make all"
#
describe: describe-local $(SUBDIRS:%=subdir-describe-%)

#
# Show the build steps in this directory (used by describe
# to show the step in the current directory)
#
describe-local:
	@echo "==== Build steps in directory $(CURDIR) ===="
ifdef SUBDIRS
	@echo " * Sub-directories: $(SUBDIRS)"
	@echo ""
endif
ifneq ("$(strip $(OBJECTS) $(MAIN_OBJECT))","")
	@echo " * Compiler:  $(CC)"
	@echo "    Options: $(CFLAGS)"
	@echo ""
  ifneq ("$(strip $(PROGRAM_FILE))","")
	@echo " * Linker: $(LD)"
    ifneq ("$(strip $(LDFLAGS))","")
	@echo "    Options: $(LDFLAGS)"
    endif
    ifneq ("$(strip $(LDLIBS))", "")
	@echo "    Static libraries: $(LDLIBS)"
    endif
	@echo ""
  endif
  ifdef LIBRARY
	@echo " * Librarian: $(AR)"
	@echo ""
  endif
endif
ifdef PARSERS
	@echo " * Generate Lemon parsers from: $(PARSERS)"
	@echo ""
endif
ifdef LIBRARY
	@echo " * Build static library '$(LIBRARY)':"
	@echo "     Output: $(abspath $(LIBRARY_FILE))"
	@echo "     Sources: $(OBJECTS:%$(OBJSUFFIX)=%.c)"
	@echo ""
endif
ifdef PROGRAM
	@echo " * Build standalone program '$(PROGRAM:%.c=%)'"
	@echo "     Output: $(abspath $(PROGRAM_FILE))"
	@echo "     Sources: $(PROGRAM) $(OBJECTS:%$(OBJSUFFIX)=%.c)"
	@echo ""
endif

#----------------------------------------------------------------------
#
# Compile C files
#
# We seralize source file compilation with auto-generation of
# source files (i.e. Lemon parser) using an order-only prerequisite
# (see GNU make info page; section "4.3")
#
%$(OBJSUFFIX): %.c | generate
	$(CC) -o $@ -c $(CFLAGS) $<

#
# Generate Lemon parsers
#
# Lemon generates three output files for a grammar:
#  * C file with the parser implementation
#  * Header file with the token type definitions
#  * Log file with information about the generated parser
#
%.c %.h %.out: %.y
	$(LEMON) -s -l $<

#
# Generate gperf static lookup tables
#
# GNU gperf produces a single output file with the implementation
# of the lookup table.
#
# Note that we strip "#line" directives from the output to produce
# more readable coverage and debug information.
#
%.c: %.gperf
	$(GPERF) --output-file=$@ $<
	$(SED) -i -e '/^\#line.*$$/d' $@

# Include header dependency tracking data
-include $(DEPENDS)

