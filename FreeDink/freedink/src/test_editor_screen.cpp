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
bool suite_TestScreen_init = false;
#include "../../src/test_editor_screen.cxx"

static TestScreen suite_TestScreen;

static CxxTest::List Tests_TestScreen = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestScreen( "../../src/test_editor_screen.cxx", 31, "TestScreen", suite_TestScreen, Tests_TestScreen );

static class TestDescription_suite_TestScreen_test_new : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestScreen_test_new() : CxxTest::RealTestDescription( Tests_TestScreen, suiteDescription_TestScreen, 39, "test_new" ) {}
 void runTest() { suite_TestScreen.test_new(); }
} testDescription_suite_TestScreen_test_new;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
