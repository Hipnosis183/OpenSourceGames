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
bool suite_TestGfxFonts_init = false;
#include "../../src/test_gfx_fonts.cxx"

static TestGfxFonts suite_TestGfxFonts;

static CxxTest::List Tests_TestGfxFonts = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestGfxFonts( "../../src/test_gfx_fonts.cxx", 43, "TestGfxFonts", suite_TestGfxFonts, Tests_TestGfxFonts );

static class TestDescription_suite_TestGfxFonts_test_print_textTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestGfxFonts_test_print_textTruecolor() : CxxTest::RealTestDescription( Tests_TestGfxFonts, suiteDescription_TestGfxFonts, 59, "test_print_textTruecolor" ) {}
 void runTest() { suite_TestGfxFonts.test_print_textTruecolor(); }
} testDescription_suite_TestGfxFonts_test_print_textTruecolor;

static class TestDescription_suite_TestGfxFonts_test_print_text_wrap_getcmdsTruecolor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestGfxFonts_test_print_text_wrap_getcmdsTruecolor() : CxxTest::RealTestDescription( Tests_TestGfxFonts, suiteDescription_TestGfxFonts, 76, "test_print_text_wrap_getcmdsTruecolor" ) {}
 void runTest() { suite_TestGfxFonts.test_print_text_wrap_getcmdsTruecolor(); }
} testDescription_suite_TestGfxFonts_test_print_text_wrap_getcmdsTruecolor;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
