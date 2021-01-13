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
bool suite_TestIntegration_init = false;
#include "../../src/test_integration.cxx"

static TestIntegration suite_TestIntegration;

static CxxTest::List Tests_TestIntegration = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestIntegration( "../../src/test_integration.cxx", 50, "TestIntegration", suite_TestIntegration, Tests_TestIntegration );

static class TestDescription_suite_TestIntegration_test_integration_player_position_is_updated_after_screen_is_loaded : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIntegration_test_integration_player_position_is_updated_after_screen_is_loaded() : CxxTest::RealTestDescription( Tests_TestIntegration, suiteDescription_TestIntegration, 53, "test_integration_player_position_is_updated_after_screen_is_loaded" ) {}
 void runTest() { suite_TestIntegration.test_integration_player_position_is_updated_after_screen_is_loaded(); }
} testDescription_suite_TestIntegration_test_integration_player_position_is_updated_after_screen_is_loaded;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
