#
# Makefile for the KLEE task
#

# Prefix for the KLEE installations
# (assumes KLEE install in $(HOME)/tools as shown in the install.txt instructions)
KLEE_PREFIX := $(HOME)/tools

# Include directory of KLEE headers
# (assume KLEE install in $(HOME)/tools as by the install.txt instructions)
KLEE_INCLUDE_DIR := $(KLEE_PREFIX)/include

# KLEE binary
# (assume KLEE install in $(HOME)/tools as by the install.txt instructions)
KLEE := $(KLEE_PREFIX)/bin/klee

# KLEE flags
KLEE_FLAGS ?= -optimize

# Path to the LLVM gold linker plugin
# NOTE: Usually the plugin should reside in /usr/lib/LLVMgold.so
# Debian/Ubuntu seem to use a different packaging strategy ... :)
LLVM_GOLD_PLUGIN := /usr/lib/llvm-3.0/lib/LLVMgold.so

# Workaround for LLVMgold.so dependency bug on Ubuntu.
# See https://bugs.launchpad.net/ubuntu/+source/llvm-3.0/+bug/1026359
LLVM_BUGFIX_PRELOAD := env LD_PRELOAD=/usr/lib/llvm-3.0/lib/libLTO.so

# Extra compiler flags
EXTRA_CFLAGS ?=

# Compile to bitcode (with clang
COMPILE := $(LLVM_BUGFIX_PRELOAD) clang -emit-llvm -g \
	    -Wl,-plugin,$(LLVM_GOLD_PLUGIN) -Wl,-plugin-opt,emit-llvm \
	    -Wall -std=c99 -pedantic -I$(KLEE_INCLUDE_DIR) $(EXTRA_CFLAGS)

# Quiet by default
Q := @

# Hacklets to be compiler
HACKLETS := 		\
	urldecode.c	\
	offbyone.c

# LLVM bitcode output from the hacklets
BITCODE := $(HACKLETS:%.c=%.bc)

#----------------------------------------------------------------------
#
# Help for the build
#
help:
	@echo "Available make targets:"
	@echo " make all         - Build all bitcode programs"
	@echo " make clean       - Clean all build products"
	@echo " make distclean   - Clean all build products and KLEE results"
	@echo " make klee        - Compile and run all bitcode programs with KLEE"
	@echo " make klee-<name> - Compile and run bitcode program <name>.bc with KLEE"
	@echo ""
	@echo "Additional make targets:"
	@echo " make setup-check - Check your KLEE installation"
	@echo " make <name>.c    - Compile bitcode program <name<.bc from source file <name>.c"


# Build all bitcode programs
all: $(BITCODE)


# Compile and run all
klee: $(BITCODE:%.bc=klee-%)

# Cleanup build proudcts
clean:
	rm -f vgcore vgcore.* core $(BITCODE)

# Cleanup build products and results from previous KLEE runs
distclean: clean
	rm -rf klee-last klee-out-*

# Run a bitcode program with KLEE
klee-%: %.bc setup-check
	$(Q)echo "=== Running bitcode program $< with KLEE ==="
	$(KLEE) $(KLEE_FLAGS) $<

# Compile (and link) bitcode programs
%.bc: %.c setup-check
	$(Q)echo "[COMPILE $<]"
	$(Q)$(COMPILE) -o$@ -Wall -pedantic -std=c99 $<

# Check that the KLEE binary and "klee.h" header exists.
setup-check:
	$(Q)[ -x $(KLEE) ] || \
		(echo "KLEE executable does not exist at $(KLEE). " ; \
		echo "See $(CURDIR)/install.txt for setup instructions." ; exit 1)
	$(Q)[ -f $(KLEE_INCLUDE_DIR)/klee/klee.h ] || \
		(echo "klee.h does not exist at $(KLEE_INCLUDE_DIR)/klee/klee.h. " ; \
		echo "See $(CURDIR)/install.txt for setup instructions."; exit 1)
