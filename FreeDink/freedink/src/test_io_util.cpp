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
bool suite_TestIOUtil_init = false;
#include "../../src/test_io_util.cxx"

static TestIOUtil suite_TestIOUtil;

static CxxTest::List Tests_TestIOUtil = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestIOUtil( "../../src/test_io_util.cxx", 41, "TestIOUtil", suite_TestIOUtil, Tests_TestIOUtil );

static class TestDescription_suite_TestIOUtil_test_ioutil_ciconvert : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOUtil_test_ioutil_ciconvert() : CxxTest::RealTestDescription( Tests_TestIOUtil, suiteDescription_TestIOUtil, 80, "test_ioutil_ciconvert" ) {}
 void runTest() { suite_TestIOUtil.test_ioutil_ciconvert(); }
} testDescription_suite_TestIOUtil_test_ioutil_ciconvert;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
