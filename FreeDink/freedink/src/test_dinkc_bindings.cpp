/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include "test_android_workaround.h"
#include "config.h"
#include "SDL.h"
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_Test_dinkc_bindings_init = false;
#include "../../src/test_dinkc_bindings.cxx"

static Test_dinkc_bindings suite_Test_dinkc_bindings;

static CxxTest::List Tests_Test_dinkc_bindings = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Test_dinkc_bindings( "../../src/test_dinkc_bindings.cxx", 53, "Test_dinkc_bindings", suite_Test_dinkc_bindings, Tests_Test_dinkc_bindings );

static class TestDescription_suite_Test_dinkc_bindings_test_dinkc_concurrent_fades : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test_dinkc_bindings_test_dinkc_concurrent_fades() : CxxTest::RealTestDescription( Tests_Test_dinkc_bindings, suiteDescription_Test_dinkc_bindings, 71, "test_dinkc_concurrent_fades" ) {}
 void runTest() { suite_Test_dinkc_bindings.test_dinkc_concurrent_fades(); }
} testDescription_suite_Test_dinkc_bindings_test_dinkc_concurrent_fades;

static class TestDescription_suite_Test_dinkc_bindings_test_dinkc_make_global_function : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test_dinkc_bindings_test_dinkc_make_global_function() : CxxTest::RealTestDescription( Tests_Test_dinkc_bindings, suiteDescription_Test_dinkc_bindings, 108, "test_dinkc_make_global_function" ) {}
 void runTest() { suite_Test_dinkc_bindings.test_dinkc_make_global_function(); }
} testDescription_suite_Test_dinkc_bindings_test_dinkc_make_global_function;

static class TestDescription_suite_Test_dinkc_bindings_test_dinkc_sp_custom : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test_dinkc_bindings_test_dinkc_sp_custom() : CxxTest::RealTestDescription( Tests_Test_dinkc_bindings, suiteDescription_Test_dinkc_bindings, 117, "test_dinkc_sp_custom" ) {}
 void runTest() { suite_Test_dinkc_bindings.test_dinkc_sp_custom(); }
} testDescription_suite_Test_dinkc_bindings_test_dinkc_sp_custom;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
