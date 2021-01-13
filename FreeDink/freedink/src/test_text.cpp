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
bool suite_TestText_init = false;
#include "../../src/test_text.cxx"

static TestText suite_TestText;

static CxxTest::List Tests_TestText = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestText( "../../src/test_text.cxx", 68, "TestText", suite_TestText, Tests_TestText );

static class TestDescription_suite_TestText_test_text : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestText_test_text() : CxxTest::RealTestDescription( Tests_TestText, suiteDescription_TestText, 82, "test_text" ) {}
 void runTest() { suite_TestText.test_text(); }
} testDescription_suite_TestText_test_text;

static class TestDescription_suite_TestText_test_fade : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestText_test_fade() : CxxTest::RealTestDescription( Tests_TestText, suiteDescription_TestText, 110, "test_fade" ) {}
 void runTest() { suite_TestText.test_fade(); }
} testDescription_suite_TestText_test_fade;

static class TestDescription_suite_TestText_test_damage_experience : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestText_test_damage_experience() : CxxTest::RealTestDescription( Tests_TestText, suiteDescription_TestText, 125, "test_damage_experience" ) {}
 void runTest() { suite_TestText.test_damage_experience(); }
} testDescription_suite_TestText_test_damage_experience;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
