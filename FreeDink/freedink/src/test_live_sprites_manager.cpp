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
bool suite_TestLiveSpritesManager_init = false;
#include "../../src/test_live_sprites_manager.cxx"

static TestLiveSpritesManager suite_TestLiveSpritesManager;

static CxxTest::List Tests_TestLiveSpritesManager = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestLiveSpritesManager( "../../src/test_live_sprites_manager.cxx", 29, "TestLiveSpritesManager", suite_TestLiveSpritesManager, Tests_TestLiveSpritesManager );

static class TestDescription_suite_TestLiveSpritesManager_test_add_sprite : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_add_sprite() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 37, "test_add_sprite" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_add_sprite(); }
} testDescription_suite_TestLiveSpritesManager_test_add_sprite;

static class TestDescription_suite_TestLiveSpritesManager_test_add_sprite_dump : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_add_sprite_dump() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 43, "test_add_sprite_dump" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_add_sprite_dump(); }
} testDescription_suite_TestLiveSpritesManager_test_add_sprite_dump;

static class TestDescription_suite_TestLiveSpritesManager_test_sprite_1_not_killed : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_sprite_1_not_killed() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 49, "test_sprite_1_not_killed" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_sprite_1_not_killed(); }
} testDescription_suite_TestLiveSpritesManager_test_sprite_1_not_killed;

static class TestDescription_suite_TestLiveSpritesManager_test_sprite_gt_1_killed : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_sprite_gt_1_killed() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 56, "test_sprite_gt_1_killed" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_sprite_gt_1_killed(); }
} testDescription_suite_TestLiveSpritesManager_test_sprite_gt_1_killed;

static class TestDescription_suite_TestLiveSpritesManager_test_say_stop_xy_survive_a_screen_change : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_say_stop_xy_survive_a_screen_change() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 68, "test_say_stop_xy_survive_a_screen_change" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_say_stop_xy_survive_a_screen_change(); }
} testDescription_suite_TestLiveSpritesManager_test_say_stop_xy_survive_a_screen_change;

static class TestDescription_suite_TestLiveSpritesManager_test_sprite_live_can_make_last_sprite_created_inconsistent : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_sprite_live_can_make_last_sprite_created_inconsistent() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 81, "test_sprite_live_can_make_last_sprite_created_inconsistent" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_sprite_live_can_make_last_sprite_created_inconsistent(); }
} testDescription_suite_TestLiveSpritesManager_test_sprite_live_can_make_last_sprite_created_inconsistent;

static class TestDescription_suite_TestLiveSpritesManager_test_add_sprite_keeps_sprite_created_inconsistent : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_add_sprite_keeps_sprite_created_inconsistent() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 93, "test_add_sprite_keeps_sprite_created_inconsistent" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_add_sprite_keeps_sprite_created_inconsistent(); }
} testDescription_suite_TestLiveSpritesManager_test_add_sprite_keeps_sprite_created_inconsistent;

static class TestDescription_suite_TestLiveSpritesManager_test_sprite_live_can_make_hole : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_sprite_live_can_make_hole() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 113, "test_sprite_live_can_make_hole" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_sprite_live_can_make_hole(); }
} testDescription_suite_TestLiveSpritesManager_test_sprite_live_can_make_hole;

static class TestDescription_suite_TestLiveSpritesManager_test_get_last_sprite : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_get_last_sprite() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 126, "test_get_last_sprite" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_get_last_sprite(); }
} testDescription_suite_TestLiveSpritesManager_test_get_last_sprite;

static class TestDescription_suite_TestLiveSpritesManager_test_get_last_sprite_makes_last_sprite_created_consistent_if_gt_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_get_last_sprite_makes_last_sprite_created_consistent_if_gt_2() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 146, "test_get_last_sprite_makes_last_sprite_created_consistent_if_gt_2" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_get_last_sprite_makes_last_sprite_created_consistent_if_gt_2(); }
} testDescription_suite_TestLiveSpritesManager_test_get_last_sprite_makes_last_sprite_created_consistent_if_gt_2;

static class TestDescription_suite_TestLiveSpritesManager_test_lsm_isValidSprite : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TestLiveSpritesManager_test_lsm_isValidSprite() : CxxTest::RealTestDescription( Tests_TestLiveSpritesManager, suiteDescription_TestLiveSpritesManager, 168, "test_lsm_isValidSprite" ) {}
 void runTest() { suite_TestLiveSpritesManager.test_lsm_isValidSprite(); }
} testDescription_suite_TestLiveSpritesManager_test_lsm_isValidSprite;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
