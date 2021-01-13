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
bool suite_TestDinkIni_init = false;
#include "../../src/test_dinkini.cxx"

static TestDinkIni suite_TestDinkIni;

static CxxTest::List Tests_TestDinkIni = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestDinkIni( "../../src/test_dinkini.cxx", 43, "TestDinkIni", suite_TestDinkIni, Tests_TestDinkIni );

static class TestDescription_suite_TestDinkIni_test_dinkini_init : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestDinkIni_test_dinkini_init() : CxxTest::RealTestDescription( Tests_TestDinkIni, suiteDescription_TestDinkIni, 50, "test_dinkini_init" ) {}
 void runTest() { suite_TestDinkIni.test_dinkini_init(); }
} testDescription_suite_TestDinkIni_test_dinkini_init;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
