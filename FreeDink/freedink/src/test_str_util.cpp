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
bool suite_Test_str_util_init = false;
#include "../../src/test_str_util.cxx"

static Test_str_util suite_Test_str_util;

static CxxTest::List Tests_Test_str_util = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Test_str_util( "../../src/test_str_util.cxx", 34, "Test_str_util", suite_Test_str_util, Tests_Test_str_util );

static class TestDescription_suite_Test_str_util_test_strtoupper : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test_str_util_test_strtoupper() : CxxTest::RealTestDescription( Tests_Test_str_util, suiteDescription_Test_str_util, 36, "test_strtoupper" ) {}
 void runTest() { suite_Test_str_util.test_strtoupper(); }
} testDescription_suite_Test_str_util_test_strtoupper;

static class TestDescription_suite_Test_str_util_test_reverse : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test_str_util_test_reverse() : CxxTest::RealTestDescription( Tests_Test_str_util, suiteDescription_Test_str_util, 45, "test_reverse" ) {}
 void runTest() { suite_Test_str_util.test_reverse(); }
} testDescription_suite_Test_str_util_test_reverse;

static class TestDescription_suite_Test_str_util_test_separate_string : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test_str_util_test_separate_string() : CxxTest::RealTestDescription( Tests_Test_str_util, suiteDescription_Test_str_util, 51, "test_separate_string" ) {}
 void runTest() { suite_Test_str_util.test_separate_string(); }
} testDescription_suite_Test_str_util_test_separate_string;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
