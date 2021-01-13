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
bool suite_TestIOGfxDisplay_init = false;
#include "../../src/TestIOGfxDisplay.cxx"

static TestIOGfxDisplay suite_TestIOGfxDisplay;

static CxxTest::List Tests_TestIOGfxDisplay = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestIOGfxDisplay( "../../src/TestIOGfxDisplay.cxx", 48, "TestIOGfxDisplay", suite_TestIOGfxDisplay, Tests_TestIOGfxDisplay );

static class TestDescription_suite_TestIOGfxDisplay_test01SplashGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test01SplashGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 682, "test01SplashGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test01SplashGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test01SplashGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 687, "test_Display_screenshotGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Display_screenshotGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_Display_screenshotGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 692, "test_Surface_screenshotGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Surface_screenshotGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_Surface_screenshotGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_allocGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_allocGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 697, "test_allocGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_allocGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_allocGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_surfToDisplayCoordsGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_surfToDisplayCoordsGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 702, "test_surfToDisplayCoordsGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_surfToDisplayCoordsGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_surfToDisplayCoordsGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_blitGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_blitGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 707, "test_blitGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_blitGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_blitGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_fill_screenGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fill_screenGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 712, "test_fill_screenGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fill_screenGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_fill_screenGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_fillRectGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fillRectGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 717, "test_fillRectGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fillRectGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_fillRectGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_testBrightnessGL2Truecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_testBrightnessGL2Truecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 722, "testBrightnessGL2Truecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.testBrightnessGL2Truecolor(); }
} testDescription_suite_TestIOGfxDisplay_testBrightnessGL2Truecolor;

static class TestDescription_suite_TestIOGfxDisplay_testSplashGL2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_testSplashGL2() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 729, "testSplashGL2" ) {}
 void runTest() { suite_TestIOGfxDisplay.testSplashGL2(); }
} testDescription_suite_TestIOGfxDisplay_testSplashGL2;

static class TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotGL2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotGL2() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 734, "test_Display_screenshotGL2" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Display_screenshotGL2(); }
} testDescription_suite_TestIOGfxDisplay_test_Display_screenshotGL2;

static class TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotGL2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotGL2() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 739, "test_Surface_screenshotGL2" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Surface_screenshotGL2(); }
} testDescription_suite_TestIOGfxDisplay_test_Surface_screenshotGL2;

static class TestDescription_suite_TestIOGfxDisplay_test_blitGL2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_blitGL2() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 744, "test_blitGL2" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_blitGL2(); }
} testDescription_suite_TestIOGfxDisplay_test_blitGL2;

static class TestDescription_suite_TestIOGfxDisplay_test_fillRectGL2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fillRectGL2() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 749, "test_fillRectGL2" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fillRectGL2(); }
} testDescription_suite_TestIOGfxDisplay_test_fillRectGL2;

static class TestDescription_suite_TestIOGfxDisplay_test_fill_screenGL2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fill_screenGL2() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 754, "test_fill_screenGL2" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fill_screenGL2(); }
} testDescription_suite_TestIOGfxDisplay_test_fill_screenGL2;

static class TestDescription_suite_TestIOGfxDisplay_testSplashSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_testSplashSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 767, "testSplashSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.testSplashSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_testSplashSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 772, "test_Display_screenshotSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Display_screenshotSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_Display_screenshotSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 777, "test_Surface_screenshotSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Surface_screenshotSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_Surface_screenshotSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_allocSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_allocSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 782, "test_allocSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_allocSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_allocSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_blitSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_blitSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 787, "test_blitSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_blitSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_blitSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_fillRectSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fillRectSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 792, "test_fillRectSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fillRectSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_fillRectSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_test_fill_screenSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fill_screenSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 797, "test_fill_screenSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fill_screenSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_test_fill_screenSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_testBrightnessSWTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_testBrightnessSWTruecolor() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 802, "testBrightnessSWTruecolor" ) {}
 void runTest() { suite_TestIOGfxDisplay.testBrightnessSWTruecolor(); }
} testDescription_suite_TestIOGfxDisplay_testBrightnessSWTruecolor;

static class TestDescription_suite_TestIOGfxDisplay_testSplashSW : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_testSplashSW() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 809, "testSplashSW" ) {}
 void runTest() { suite_TestIOGfxDisplay.testSplashSW(); }
} testDescription_suite_TestIOGfxDisplay_testSplashSW;

static class TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotSW : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Display_screenshotSW() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 814, "test_Display_screenshotSW" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Display_screenshotSW(); }
} testDescription_suite_TestIOGfxDisplay_test_Display_screenshotSW;

static class TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotSW : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_Surface_screenshotSW() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 819, "test_Surface_screenshotSW" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_Surface_screenshotSW(); }
} testDescription_suite_TestIOGfxDisplay_test_Surface_screenshotSW;

static class TestDescription_suite_TestIOGfxDisplay_test_blitSW : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_blitSW() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 824, "test_blitSW" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_blitSW(); }
} testDescription_suite_TestIOGfxDisplay_test_blitSW;

static class TestDescription_suite_TestIOGfxDisplay_test_fillRectSW : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fillRectSW() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 829, "test_fillRectSW" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fillRectSW(); }
} testDescription_suite_TestIOGfxDisplay_test_fillRectSW;

static class TestDescription_suite_TestIOGfxDisplay_test_fill_screenSW : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestIOGfxDisplay_test_fill_screenSW() : CxxTest::RealTestDescription( Tests_TestIOGfxDisplay, suiteDescription_TestIOGfxDisplay, 834, "test_fill_screenSW" ) {}
 void runTest() { suite_TestIOGfxDisplay.test_fill_screenSW(); }
} testDescription_suite_TestIOGfxDisplay_test_fill_screenSW;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
