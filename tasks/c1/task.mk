#
# Task specific build configuration for assignment C1.
#
# WARNING: THIS FILE IS PART OF THE ASSIGNMENT SPECIFICATION AND
# MODELS THE FUNCTIONALITY AND BEHAVIOR OF IAIK'S TEST SYSTEM. ANY
# CHANGES TO THIS FILE WILL BE IGNORED BY THE ACTUAL TEST SYSTEM.
#
TOP = .

include $(TOP)/build.mk

#----------------------------------------------------------------------
# Documentation of extra make-rules
#
help-local:
	@echo "Supported extra targets:"
	@echo " make lcov-runtests     - Recompile and run code coverage analysis (lcov)"
	@echo " make valgrind-runtests - Recompile and run the test-suite with valgrind"
	@echo " make bitcode           - Recompile as LLVM bitcode for use with KLEE"
	@echo " make scan-build        - Static code analysis with clang's static analyzer"
	@echo " make cppcheck          - Static code analysis with cppcheck"
	@echo ""
	@echo "Experimental extra targets:"
	@echo " make proof             - Run Frama-C to prove program properties"
	@echo " make proof-gui         - Run Frama-C (GUI) to prove program properties"
	@echo ""

#----------------------------------------------------------------------
# Extra rules for clang's static analyzer
#

# Additional options for the clang static analyzer
SCAN_BUILD_FLAGS ?=

# Scan with the clang static analyzer
scan-build:
	scan-build $(SCAN_BUILD_FLAGS) -o _scans -k $(MAKE) COMPILER=clang clean all

#----------------------------------------------------------------------
# Extra rules for the cppcheck static analyzer
#

# Additional options for cppcheck
CPPCHECK_FLAGS ?= --std=c99 --enable=all --verbose

cppcheck:
	$(MAKE) -C src/ local-cppcheck

#----------------------------------------------------------------------
# Extra rules for running the tests with valgrind
#

# Additional flags for valgrind
VG_FLAGS ?= --tool=memcheck  --leak-check=full --show-reachable=yes \
	--track-origins=yes --undef-value-errors=yes --vgdb=full

# Additional flags for the test-tool (when running under valgrind)
VGR_RUNTEST_ARGS ?= -b

valgrind-runtests:
	$(MAKE) CONFIGURATION=debug clean all
	@echo "==== Starting testsuite under valgrind ===="
	valgrind $(VG_FLAGS) ./bin/tinybntests $(VG_RUNTEST_ARGS)

#----------------------------------------------------------------------
# Extra rules for LCOV
#

# Additional lcov options for the init step
LCOV_INIT_FLAGS ?=

# Additional lcov options for the update step
LCOV_UPDATE_FLAGS ?=

# Additional arguments to the tinybntests tool during "lcov-runtests"
LCOV_RUNTEST_ARGS ?=

# Build configuration when building for LCOV
#
# See the CONFIGURATION variable in build.mk for more details.
LCOV_CONFIGURATION ?= release+sym

# Recompile with coverage testing data, run all test-cases in batch mode,
# and then update the coverage report.
#
# Note that we ignore errors (non-zero exit status) reported by tinybntest
# to allow use of this target from early on.
lcov-runtests:
	$(MAKE) lcov-compile
	-bin/tinybntests -b $(LCOV_RUNTEST_ARGS)
	$(MAKE) lcov-update


# Recompile for testing with lcov
lcov-compile:
	$(MAKE) COMPILER=gcc COVERAGE=gcov CONFIGURATION="$(LCOV_CONFIGURATION)" clean all
	$(MAKE) lcov-init

# Reset coverage testing data
lcov-init:
	lcov -z -d src/
	lcov -i -c -d src/ $(LCOV_INIT_FLAGS) -o coverage.info

# Update current coverage data (after some tests have been executed) and
# the coverage report.
lcov-update:
	lcov -c -d src/ $(LCOV_UPDATE_FLAGS) -o coverage.info
	lcov -z -d src/
	genhtml -o _cov coverage.info
	@echo "* Open $(PWD)/_cov/index.html in a browser to see coverage results."

#----------------------------------------------------------------------
# Extra rules for the KLEE task
#
bitcode:
	$(MAKE) COMPILER=bitcode clean all

#----------------------------------------------------------------------
# Extra rules for the Frama-C bonus task
#
proof:
	$(MAKE) -C src/ proof

proof-gui:
	$(MAKE) -C src/ proof-gui


