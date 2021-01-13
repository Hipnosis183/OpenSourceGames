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
bool suite_TestDinkCConsole_init = false;
#include "../../src/test_dinkc_console.cxx"

static TestDinkCConsole suite_TestDinkCConsole;

static CxxTest::List Tests_TestDinkCConsole = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestDinkCConsole( "../../src/test_dinkc_console.cxx", 42, "TestDinkCConsole", suite_TestDinkCConsole, Tests_TestDinkCConsole );

static class TestDescription_suite_TestDinkCConsole_test_console : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkCConsole_test_console() : CxxTest::RealTestDescription( Tests_TestDinkCConsole, suiteDescription_TestDinkCConsole, 51, "test_console" ) {}
 void runTest() { suite_TestDinkCConsole.test_console(); }
} testDescription_suite_TestDinkCConsole_test_console;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
