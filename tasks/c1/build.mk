#
# Common make rules used to emulate IAIK's build infrastructure.
#
# WARNING: THIS FILE IS PART OF THE ASSIGNMENT SPECIFICATION AND
# MODELS THE FUNCTIONALITY AND BEHAVIOR OF IAIK'S TEST SYSTEM. ANY
# CHANGES TO THIS FILE WILL BE IGNORED BY THE ACTUAL TEST SYSTEM.
#

#
# The TOP make variable must be defined as relative path of the
# assignment top directory.
#
ifndef TOP
$(error The TOP make variable must be definebd as relative path to the \
  top-level build directory)
endif

#----------------------------------------------------------------------
# Default values for customizable variables
#

# INCLUDES: Extra include directories
INCLUDES ?=

# LIBDIRS: Extra library search directories
LIBDIRS ?=

# LIBS: Extra libraries
LIBS ?=

# DEFINES: Additional preprocessor defines (from makefiles)
#
DEFINES ?=

# EXTRA_DEFINES: More additional preprocesosr defines (e.g. from command line)
#
# This variable can be overridden from the make command line, to specify
# additional preprocessor defines for testing, without interferring with
# existing defines in the DEFINES variables.
#
EXTRA_DEFINES ?=

# COVERAGE: Compile coverage testing support
#
# Allowed values:
#   no   - (default) build without coverage testing
#   gcov - Build for coverage testing with gcov/lcov
#
COVERAGE ?= no

# Extra compiler and linker flags (e.g. from command line)
#
EXTRA_CFLAGS  ?=
EXTRA_LDFLAGS ?=

#----------------------------------------------------------------------
#
# Build configurations
#
# Can be one of "release", "debug" or "nosym"
# The default configuration (for testing) is "debug".
#
CONFIGURATION ?= debug

ifeq ("$(CONFIGURATION)","release")
 # Release build, define NDEBUG, omit -g flag
 DEFINES += NDEBUG

else ifeq ("$(CONFIGURATION)","release+sym")
 # Release build with symbolic debug information,
 # define NDEBUG, enable the -g flag
 #
 # This configuration is most appropriate for coverage testing,
 # where "assert"-based assertions would just generate impossible
 # branches (for the false paths)
 DEFINES += NDEBUG
 EXTRA_CFLAGS  += -g
 EXTRA_LDFLAGS += -g

else ifeq ("$(CONFIGURATION)","debug")
 # Debug configuration, do not define NDEBUG, add -g flag
 EXTRA_CFLAGS  += -g
 EXTRA_LDFLAGS += -g

else ifeq ("$(CONFIGURATION)","nosym")
 # Debug cobnfiguration without symbols, do not define NDEBUG, omit -g flag

else
 $(error Please set CONFIGURATION to one of "release", "release+sym", "debug", or "nosym")
endif

#----------------------------------------------------------------------
# Frama-C formal verification
#
# The "PROOF" make variable lists the names of C functions, which
# should be considered by frama-c for formal verification. The functions
# listed in the "PROOF" variable will be passed to the WP plugin
# via "-wp-fct".
#

# Names of C functions with a formal proof
# (Note defining PROOF as empty values tries to verify everything)
#
# PROOF := Func1 Func2 ...

# Frama-C binary to use
FRAMAC := frama-c

# Frama-C binary (GUI version) to use
FRAMAC_GUI := frama-c-gui

# Preprocessing options for Frama-C
FRAMAC_PP_OPTIONS ?= -pp-annot -cpp-extra-args="$(DEFINES:%=-D%) $(EXTRA_DEFINES:%=-D%) -DNDEBUG"

# Options for invoking the Frama-C Weakest Precondition (WP) plugin
FRAMAC_WP_OPTIONS ?= -wp -wp-rte -wp-model Typed+ref+cast

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
 CC = $(CROSS_COMPILE)clang
 LD = $(CROSS_COMPILE)clang
 AR = $(CROSS_COMPILE)ar rcs

 EXESUFFIX :=
 OBJSUFFIX := .o
 DEPSUFFIX := .d
 LIBNAMESUFFIX :=
 LIBSUFFIX := .a

else ifeq ("$(COMPILER)","bitcode")

 # LLVM bitcode (clang) toolchain profile
 #
 # Experimental. Requires gold and the LLVMgold.so plugin (llvm-dev).
 #
 # In theory everything should work nicely if we call LLVM with
 # the "-flto" optimization option to enable link-time optimizations.
 # (Alternatively we could also specify "-O4")
 #
 # This will cause the compile steps to generate LLVM bitcode instead
 # of native code. During the link step we use the GNU gold linker with
 # the LLVM plugin, to generate a native binary _AND_ a bitcode version
 # of the executable.
 #
 # Unfortunately the Ubuntu llvm-3.0-dev package (containing the linker
 # plugin) is slightly broken ...
 #
 # .. for now this leaves us with either compiling LLVM und clang by
 # ourselves (which is tedious), or by using the workarounds below
 # (which may be considered evil but less tedious) ...
 $(warning Using EXPERIMENTAL LLVM/Clang bitcode configuration)

 # Path to the LLVM gold plugin
 # NOTE: Usually the plugin should reside in /usr/lib/LLVMgold.so
 # Debian/Ubuntu seem to use a different packaging strategy ... :)
 LLVM_GOLD_PLUGIN ?= /usr/lib/llvm-3.0/lib/LLVMgold.so

 # Workaround for LLVMgold.so dependency bug on Ubuntu.
 # See https://bugs.launchpad.net/ubuntu/+source/llvm-3.0/+bug/1026359
 LLVM_BUGFIX_PRELOAD ?= env LD_PRELOAD=/usr/lib/llvm-3.0/lib/libLTO.so

 CC := $(CROSS_COMPILE)clang
 LD := $(LLVM_BUGFIX_PRELOAD) $(CROSS_COMPILE)clang
 AR := $(CROSS_COMPILE)llvm-link-3.0 -o


 CFLAGS  += -O4 -emit-llvm
 LDFLAGS += -O4 -flto \
	-Wl,-plugin,$(LLVM_GOLD_PLUGIN) \
	-Wl,-plugin-opt,also-emit-llvm

 EXESUFFIX     :=
 OBJSUFFIX     := .bc
 DEPSUFFIX     := .d
 LIBNAMESUFFIX := .bc
 LIBSUFFIX     := .a

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

CFLAGS  += $(EXTRA_CFLAGS) $(INCLUDES:%=-I%) $(DEFINES:%=-D%) $(EXTRA_DEFINES:%=-D%)
LDFLAGS += $(EXTRA_LDFLAGS) $(LIBDIRS:%=-L%)

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

#----------------------------------------------------------------------
# Compile rules
#

#----------------------------------------------------------------------
# Default make target ("make help")
#
help: help-common help-local

help-common:
	@echo "Supported standard build targets:"
	@echo " make all          - Build all targets"
	@echo " make help         - Show this help file (default target)"
	@echo " make describe     - Describe the build steps taken by 'make all'."
	@echo " make clean        - Clean normal build outputs"
	@echo " make distclean    - Clean generated source files and build outputs"
	@echo ""
	@echo "Use 'make all' to build all targets in the current directory and its"
	@echo "subdirectories. A human readable description of the build steps is shown"
	@echo "by 'make describe'."
	@echo ""

# Additional local help (may be redefined in specific Makefile)
help-local: | help-common

#----------------------------------------------------------------------
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

#----------------------------------------------------------------------
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

#----------------------------------------------------------------------
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

#----------------------------------------------------------------------
# Link a standalone application if PROGRAM is given.
#
ifdef PROGRAM
 PROGRAM_FILE  := $(BINDIR)/$(PROGRAM:%.c=%$(EXESUFFIX))
 MAIN_OBJECTS  := $(PROGRAM:%.c=%$(OBJSUFFIX))
 CLEAN_FILES   += $(PROGRAM_FILE) $(PROGRAM_FILE).bc $(MAIN_OBJECTS)
 DEPENDS       += $(PROGRAM:%.c=%$(DEPSUFFIX))
 BUILD_TARGETS += program

 # Note that we use GNU make's "Directory Search for Link Libraries" feature
 # to resolve our static libraries below.
 #
 # This allows us to uniformly handle bitcode libraries (when building
 # bitcode for KLEE) and native libraries (during normal compilation).
 .LIBPATTERNS: lib%.a
 vpath lib%.a $(BINDIR)

 # And here comes our program
 program: $(PROGRAM_FILE)

 ifdef LIBRARY
  # We are building the program and a static library,
  # link against the static library
  $(PROGRAM_FILE): $(MAIN_OBJECTS) -l$(LIBRARY) $(LIBS:%=-l%)
	$(LD) -o $@ $(LDFLAGS) -L. $^
 else
  # We are only building the program, link directly against
  # the object files.
  $(PROGRAM_FILE): $(MAIN_OBJECTS) $(OBJECTS) $(LIBS:%=-l%)
	$(LD) -o $@ $(LDFLAGS) $^
 endif
endif

#----------------------------------------------------------------------
# Formally proof program properties with Frama-C if PROOF is given.
#
ifdef PROOF
 # Source files to be considered by Frama-C
 FRAMAC_SOURCES := $(SOURCES) $(PROGRAM)
 ifeq ($(FRAMAC_SOURCES),)
  $(error No source files containing functions to prove with Frama-C)
 endif

 ifeq ($(PROOF),)
  $(info No functions given for Frama-C proof. Trying to proof everything)
 endif

 # Functions to be verified by Frama-C (leaving PROOF empty tries to
 # verify everything)
 FRAMAC_WP_FCTS := $(PROOF:%=-wp-fct %)

 # Frama-C options (common to gui and cli)
 FRAMAC_OPTIONS := 				\
	$(FRAMAC_PP_OPTIONS)			\
	$(FRAMAC_WP_OPTIONS) $(FRAMAC_WP_FCTS)	\
	$(FRAMAC_SOURCES)

 # Automated proof
 proof: $(FRAMAC_SOURCES)
	$(FRAMAC) $(FRAMAC_OPTIONS) -then -report

 # Proof via Frama-C GUI tool
 proof-gui: $(FRAMC_SOURCES)
	$(FRAMAC_GUI)  $(FRAMAC_OPTIONS)

 .PHONY: proof proof-gui
endif

#----------------------------------------------------------------------
# Static analysis with cppcheck
#

ifneq ("$(SOURCES)","")
local-cppcheck:
	cppcheck $(INCLUDES:%=-I%) $(DEFINES:%=-D%) $(EXTRA_DEFINES:%=-D%) \
	 $(CPPCHECK_FLAGS) $(SOURCES)
else
local-cppcheck:
	@echo "No source files to check in this directory"
endif

#----------------------------------------------------------------------
#
# Build all default targets
#
all: $(BUILD_TARGETS)

#----------------------------------------------------------------------
#
# Clean build products (keep generated files)
#
clean: $(CLEAN_TARGETS)
	rm -f *.gcda *.gcno *.gcov *.o *.bc *.a *.d
	rm -f vgcore.* core.* core $(CLEAN_FILES)

#----------------------------------------------------------------------
#
# Clean build products and generated files
#
distclean: $(DISTCLEAN_TARGETS)
	rm -f *.gcda *.gcno *.gcov *.o *.bc *.a *.d
	rm -f vgcore.* core.* core $(CLEAN_FILES) $(DISTCLEAN_FILES) $(GEN_FILES)

#----------------------------------------------------------------------
#
# Generate all auto-generated source files (gperf, lemon)
#
generate: $(GEN_FILES)

#----------------------------------------------------------------------
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
    ifneq ("$(strip $(LIBS))", "")
	@echo "    Libraries: $(LIBS)"
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

#----------------------------------------------------------------------
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

#----------------------------------------------------------------------
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

#----------------------------------------------------------------------
# Include header dependency tracking data
#
-include $(DEPENDS)

