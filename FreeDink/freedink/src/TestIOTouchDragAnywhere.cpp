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
bool suite_TestTouch_init = false;
#include "../../src/TestIOTouchDragAnywhere.cxx"

static TestTouch suite_TestTouch;

static CxxTest::List Tests_TestTouch = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestTouch( "../../src/TestIOTouchDragAnywhere.cxx", 40, "TestTouch", suite_TestTouch, Tests_TestTouch );

static class TestDescription_suite_TestTouch_testTransform : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestTouch_testTransform() : CxxTest::RealTestDescription( Tests_TestTouch, suiteDescription_TestTouch, 48, "testTransform" ) {}
 void runTest() { suite_TestTouch.testTransform(); }
} testDescription_suite_TestTouch_testTransform;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
