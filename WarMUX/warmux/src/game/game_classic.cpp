/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Specialization of Game methods for the classic mode.
 *****************************************************************************/

#include "character/character.h"
#include "game/game_classic.h"
#include "game/game_mode.h"
#include "game/game_time.h"
#include "include/action_handler.h"
#include "interface/cursor.h"
#include "interface/interface.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "network/network.h"
#include "object/objbox.h"
#include "object/bonus_box.h"
#include "object/medkit.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include <WARMUX_debug.h>
#include <WARMUX_random.h>

GameClassic::GameClassic()
  : Game()
  , duration(0)
{ }

void GameClassic::EndOfGame()
{
  SetState(END_TURN);
  duration = GameMode::GetInstance()->duration_exchange_player + 2;
  GameMessages::GetInstance()->Add(_("And the winner is..."), white_color);

  while (duration >= 1) {
    MainLoop();
  }
}

void GameClassic::RefreshClock()
{
  GameTime * global_time = GameTime::GetInstance();

  if (1000 < global_time->Read() - last_clock_update) {
    last_clock_update = global_time->Read();

    switch (state) {

    case PLAYING:
      if (duration <= 1) {

        /* let the user release the key to shoot */
        if (ActiveTeam().GetWeapon().IsLoading())
          break;

        JukeBox::GetInstance()->Play("default", "end_turn");
        SetState(END_TURN);
      } else {
        duration--;
        if (duration == 12) {
          countdown_sample.Play("default", "countdown-end_turn");
        }
        if (duration > 10) {
          Interface::GetInstance()->UpdateTimer(duration, false, false);
        } else {
          Interface::GetInstance()->UpdateTimer(duration, true, false);
        }
      }
      break;

    case HAS_PLAYED:
      if (duration <= 1) {
        SetState(END_TURN);
      } else {
        duration--;
        Interface::GetInstance()->UpdateTimer(duration, false, false);
      }
      break;

    case END_TURN:
      if (duration <= 1) {

        if (IsAnythingMoving()) {
          duration = 1;
          // Hack to be sure that nothing is moving since long enough
          // it avoids giving hand to another team during the end of an explosion for example
          break;
        }

        if (IsGameFinished()) {
          duration--;
          break;
        }

        if (give_objbox && GetWorld().IsOpen()) {
          NewBox();
          give_objbox = false;
          break;
        }
        else {
          SetState(PLAYING);
          break;
        }
      } else {
        duration--;
      }
      break;
    } // switch
  }// if
}

uint GameClassic::GetRemainingTime() const
{
  return duration;
}

// Beginning of a new turn
void GameClassic::__SetState_PLAYING()
{
  MSG_DEBUG("game.statechange", "Playing");

  // initialize counter
  duration = GameMode::GetInstance()->duration_turn;
  Interface::GetInstance()->UpdateTimer(duration, false, true);
  Interface::GetInstance()->EnableDisplayTimer(true);
  last_clock_update = GameTime::GetInstance()->Read();

  Wind::GetRef().ChooseRandomVal();

  SetCharacterChosen(false);

  // Prepare each character for a new turn
  FOR_ALL_LIVING_CHARACTERS(team,character)
    character->PrepareTurn();

  // Select the next team
  ASSERT (!IsGameFinished());
  GetTeamsList().NextTeam();

  give_objbox = true; //hack: make it so that there is no more than one objbox per turn
}

void GameClassic::__SetState_HAS_PLAYED()
{
  MSG_DEBUG("game.statechange", "Has played, now can move");
  duration = GameMode::GetInstance()->duration_move_player;
  last_clock_update = GameTime::GetInstance()->Read();
  Interface::GetInstance()->UpdateTimer(duration, false, true);
  CharacterCursor::GetInstance()->Hide();
}

void GameClassic::__SetState_END_TURN()
{
  MSG_DEBUG("game.statechange", "End of turn");
  countdown_sample.Stop();
  ActiveTeam().AccessWeapon().SignalTurnEnd();
  ActiveTeam().AccessWeapon().Deselect();
  CharacterCursor::GetInstance()->Hide();
  duration = GameMode::GetInstance()->duration_exchange_player;
  Interface::GetInstance()->UpdateTimer(duration, false, true);
  Interface::GetInstance()->EnableDisplayTimer(false);
  last_clock_update = GameTime::GetInstance()->Read();

  // Applying Disease damage and Death mode.
  ApplyDiseaseDamage();
  ApplyDeathMode();
}

// Reduce energy of each character if we are in death mode
void GameClassic::ApplyDeathMode () const
{
  if (IsGameFinished()) return;

  if (GameTime::GetInstance()->Read() > GameMode::GetInstance()->duration_before_death_mode * 1000) {
    GameMessages::GetInstance()->Add(_("Hurry up, you are too slow !!"), white_color);
    FOR_ALL_LIVING_CHARACTERS(team, character) {
      // If the character energy is lower than damage
      // per turn we reduce the character's health to 1
      if (static_cast<uint>(character->GetEnergy()) >
          GameMode::GetInstance()->damage_per_turn_during_death_mode)
        // No damage dealer, thus pass NULL
        character->SetEnergyDelta(-(int)GameMode::GetInstance()->damage_per_turn_during_death_mode, NULL);
      else
        character->SetEnergy(1, NULL);
    }
  }
}

bool GameClassic::IsGameFinished() const
{
  return (NbrRemainingTeams() <= 1);
}
