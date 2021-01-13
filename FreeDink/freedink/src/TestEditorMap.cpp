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
bool suite_TestEditorMap_init = false;
#include "../../src/TestEditorMap.cxx"

static TestEditorMap suite_TestEditorMap;

static CxxTest::List Tests_TestEditorMap = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestEditorMap( "../../src/TestEditorMap.cxx", 31, "TestEditorMap", suite_TestEditorMap, Tests_TestEditorMap );

static class TestDescription_suite_TestEditorMap_test_new : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestEditorMap_test_new() : CxxTest::RealTestDescription( Tests_TestEditorMap, suiteDescription_TestEditorMap, 36, "test_new" ) {}
 void runTest() { suite_TestEditorMap.test_new(); }
} testDescription_suite_TestEditorMap_test_new;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
