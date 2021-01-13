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
bool suite_TestDinkC_init = false;
#include "../../src/test_dinkc.cxx"

static TestDinkC suite_TestDinkC;

static CxxTest::List Tests_TestDinkC = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestDinkC( "../../src/test_dinkc.cxx", 47, "TestDinkC", suite_TestDinkC, Tests_TestDinkC );

static class TestDescription_suite_TestDinkC_test_dinkc_getparms_bounds : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkC_test_dinkc_getparms_bounds() : CxxTest::RealTestDescription( Tests_TestDinkC, suiteDescription_TestDinkC, 59, "test_dinkc_getparms_bounds" ) {}
 void runTest() { suite_TestDinkC.test_dinkc_getparms_bounds(); }
} testDescription_suite_TestDinkC_test_dinkc_getparms_bounds;

static class TestDescription_suite_TestDinkC_test_dinkc_getparms_int : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkC_test_dinkc_getparms_int() : CxxTest::RealTestDescription( Tests_TestDinkC, suiteDescription_TestDinkC, 66, "test_dinkc_getparms_int" ) {}
 void runTest() { suite_TestDinkC.test_dinkc_getparms_int(); }
} testDescription_suite_TestDinkC_test_dinkc_getparms_int;

static class TestDescription_suite_TestDinkC_test_dinkc_getparms_emptyint : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkC_test_dinkc_getparms_emptyint() : CxxTest::RealTestDescription( Tests_TestDinkC, suiteDescription_TestDinkC, 72, "test_dinkc_getparms_emptyint" ) {}
 void runTest() { suite_TestDinkC.test_dinkc_getparms_emptyint(); }
} testDescription_suite_TestDinkC_test_dinkc_getparms_emptyint;

static class TestDescription_suite_TestDinkC_test_dinkc_getparms_parens : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkC_test_dinkc_getparms_parens() : CxxTest::RealTestDescription( Tests_TestDinkC, suiteDescription_TestDinkC, 105, "test_dinkc_getparms_parens" ) {}
 void runTest() { suite_TestDinkC.test_dinkc_getparms_parens(); }
} testDescription_suite_TestDinkC_test_dinkc_getparms_parens;

static class TestDescription_suite_TestDinkC_test_dinkc_lookup_var_107 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkC_test_dinkc_lookup_var_107() : CxxTest::RealTestDescription( Tests_TestDinkC, suiteDescription_TestDinkC, 144, "test_dinkc_lookup_var_107" ) {}
 void runTest() { suite_TestDinkC.test_dinkc_lookup_var_107(); }
} testDescription_suite_TestDinkC_test_dinkc_lookup_var_107;

static class TestDescription_suite_TestDinkC_test_dinkc_lookup_var_108 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkC_test_dinkc_lookup_var_108() : CxxTest::RealTestDescription( Tests_TestDinkC, suiteDescription_TestDinkC, 160, "test_dinkc_lookup_var_108" ) {}
 void runTest() { suite_TestDinkC.test_dinkc_lookup_var_108(); }
} testDescription_suite_TestDinkC_test_dinkc_lookup_var_108;

static class TestDescription_suite_TestDinkC_test_dinkc_dont_return_same_script_id_twice : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkC_test_dinkc_dont_return_same_script_id_twice() : CxxTest::RealTestDescription( Tests_TestDinkC, suiteDescription_TestDinkC, 177, "test_dinkc_dont_return_same_script_id_twice" ) {}
 void runTest() { suite_TestDinkC.test_dinkc_dont_return_same_script_id_twice(); }
} testDescription_suite_TestDinkC_test_dinkc_dont_return_same_script_id_twice;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
