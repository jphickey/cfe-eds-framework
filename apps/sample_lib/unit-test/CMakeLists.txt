##################################################################
#
# Coverage Unit Test build recipe
#
# This CMake file contains the recipe for building the sample unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

#
# NOTE on the subdirectory structures here:
#
# - "inc" provides local header files shared between the coveragetest,
#    wrappers, and overrides source code units
# - "coveragetest" contains source code for the actual unit test cases
#    The primary objective is to get line/path coverage on the FSW 
#    code units.
# - "wrappers" contains wrappers for the FSW code.  The wrapper adds
#    any UT-specific scaffolding to facilitate the coverage test, and
#    includes the unmodified FSW source file.
# - "overrides" provides implementation of LOCAL stub functions to
#    that replace external calls.  This is for use cases where the
#    normal link-time  replacements is not sufficient/possible, and
#    a different implementation needs to be called at compile-time 
#    instead. 
#
# IMPORTANT: Most UT test cases do not need the "overrides" feature,
# it is primarily for cases where a C library function needs to be
# overridden.  It's use-case is included here as an example.


set(UT_NAME sample_lib)

# Use the UT_Assert public API
# This is also allowed to directly include files in the "fsw/src"
# directory that are normally private to the implementation
include_directories(${osal_MISSION_DIR}/ut_assert/inc)
include_directories(${PROJECT_SOURCE_DIR}/fsw/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# The "overrides" target provides replacements for C library
# calls that cannot be handled at link time.  Most UT test
# cases will NOT need this feature.
add_library(ut_${UT_NAME}_overrides STATIC
    override_src/libc_string_stubs.c
)

# The LIB_SRC_FILES variable should contain the list of source files for the FSW build
# This assumes a 1:1 relationship between FSW source units and coverage tests
# Generate a dedicated "testrunner" executable that executes the tests for each FSW code unit
# Although sample_lib has only one source file, this is done in a loop such that 
# the general pattern should work for several files as well.
foreach(SRCFILE ${LIB_SRC_FILES})
    get_filename_component(UNITNAME "${SRCFILE}" NAME_WE)
    
    set(TESTNAME                "${UT_NAME}-${UNITNAME}")
    set(UNIT_SOURCE_FILE        "${CFE_SAMPLE_LIB_SOURCE_DIR}/fsw/src/${UNITNAME}.c")
    set(TESTCASE_SOURCE_FILE    "coveragetest/coveragetest_${UNITNAME}.c")
    
    # Compile the source unit under test as a OBJECT
    add_library(ut_${TESTNAME}_object OBJECT
        ${UNIT_SOURCE_FILE}
    )    
    
    # Apply the UT_COVERAGE_COMPILE_FLAGS to the units under test
    # This should enable coverage analysis on platforms that support this
    target_compile_options(ut_${TESTNAME}_object PRIVATE ${UT_COVERAGE_COMPILE_FLAGS})
        
    # For this object target only, the "override" includes should be injected
    # into the include path BEFORE any other include path.  This is so the
    # override will take precedence over any system-provided version.
    target_include_directories(ut_${TESTNAME}_object PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/override_inc)
        
    # Compile a test runner application, which contains the
    # actual coverage test code (test cases) and the unit under test
    add_executable(${TESTNAME}-testrunner
        ${TESTCASE_SOURCE_FILE}
        $<TARGET_OBJECTS:ut_${TESTNAME}_object>
    )
    
    # This also needs to be linked with UT_COVERAGE_LINK_FLAGS (for coverage)
    # This is also linked with any other stub libraries needed,
    # as well as the UT assert framework    
    target_link_libraries(${TESTNAME}-testrunner
        ${UT_COVERAGE_LINK_FLAGS}
        ut_${UT_NAME}_stubs
        ut_${UT_NAME}_overrides
        ut_cfe-core_stubs
        ut_assert
    )
    
    # Add it to the set of tests to run as part of "make test"
    add_test(${TESTNAME} ${TESTNAME}-testrunner)
    install(TARGETS ${TESTNAME}-testrunner DESTINATION ${TGTNAME}/${UT_INSTALL_SUBDIR})
    
endforeach()

