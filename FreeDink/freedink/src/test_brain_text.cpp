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
bool suite_TestBrainText_init = false;
#include "../../src/test_brain_text.cxx"

static TestBrainText suite_TestBrainText;

static CxxTest::List Tests_TestBrainText = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestBrainText( "../../src/test_brain_text.cxx", 31, "TestBrainText", suite_TestBrainText, Tests_TestBrainText );

static class TestDescription_suite_TestBrainText_test_brain_text : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestBrainText_test_brain_text() : CxxTest::RealTestDescription( Tests_TestBrainText, suiteDescription_TestBrainText, 39, "test_brain_text" ) {}
 void runTest() { suite_TestBrainText.test_brain_text(); }
} testDescription_suite_TestBrainText_test_brain_text;

static class TestDescription_suite_TestBrainText_test_damage_experience : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestBrainText_test_damage_experience() : CxxTest::RealTestDescription( Tests_TestBrainText, suiteDescription_TestBrainText, 101, "test_damage_experience" ) {}
 void runTest() { suite_TestBrainText.test_damage_experience(); }
} testDescription_suite_TestBrainText_test_damage_experience;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
