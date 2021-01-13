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
 * Generic Box that falls from they sky.
 *****************************************************************************/

#include <sstream>
#include <iostream>

#include <WARMUX_debug.h>

#include "object/objbox.h"
#include "character/character.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "include/app.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "weapon/explosion.h"

#ifdef DEBUG
#include "graphic/video.h"
#include "include/app.h"
#include "map/camera.h"
#endif

const uint SPEED = 5; // meter / seconde

ObjBox::ObjBox(const std::string &name)
  : PhysicalObj(name)
{
  m_allow_negative_y = true;

  parachute = true;

  m_energy = start_life_points;

  SetSpeed(SPEED, HALF_PI);
  SetCollisionModel(true, false, true);
  JukeBox::GetInstance()->Play("default","box/falling");
}

ObjBox::~ObjBox()
{
  delete anim;
  Game::GetInstance()->SetCurrentBox(NULL);
}

void ObjBox::CloseParachute()
{
  SetAirResistFactor(1.0);
  Game::GetInstance()->SetCurrentBox(NULL);
  MSG_DEBUG("box", "End of the fall: parachute=%d", parachute);
  hit.Play("default", "box/hitting_ground");
  if (!parachute) return;

  MSG_DEBUG("box", "Start of the animation 'fold of the parachute'.");
  parachute = false;
  anim->SetCurrentFrame(0);
  anim->Start();
}

void ObjBox::SignalGroundCollision(const Point2d& /*my_speed_before*/, const Double& /*contactAngle*/)
{
  CloseParachute();
}

void ObjBox::SignalObjectCollision(const Point2d& my_speed_before,
                                   PhysicalObj * obj,
                                   const Point2d& /*obj_speed_before*/)
{
  // The box has (probably) landed on an object (a barrel for instance)
  if (my_speed_before.Norm().IsNotZero())
    CloseParachute();

  if (obj->IsCharacter())
    ApplyBonus((Character *)obj);
}

void ObjBox::SignalDrowning()
{
  CloseParachute();
}

void ObjBox::DropBox()
{
  if (parachute) {
    SetAirResistFactor(1.0);
    parachute = false;
    anim->SetCurrentFrame(anim->GetFrameCount() - 1);
  } else {
    m_ignore_movements = true;
  }
}

void ObjBox::Draw()
{
  anim->Draw(GetPosition());

#ifdef DEBUG
  if (IsLOGGING("test_rectangle")) {
    Rectanglei test_rect(GetTestRect());
    test_rect.SetPosition(test_rect.GetPosition() - Camera::GetInstance()->GetPosition());
    GetMainWindow().RectangleColor(test_rect, primary_red_color, 1);

    Rectanglei rect(GetPosition() - Camera::GetInstance()->GetPosition(), anim->GetSize());
    GetMainWindow().RectangleColor(rect, primary_blue_color, 1);
  }
#endif
}

void ObjBox::Refresh()
{
  // If we touch a character, we remove the medkit
  FOR_ALL_LIVING_CHARACTERS(team, character) {
    if(Overlapse(*character)) {
      ApplyBonus(&(*character));
      Ghost();
      return;
    }
  }
  // Refresh animation
  if (!anim->IsFinished() && !parachute) anim->Update();
}

//Boxes can explode...
void ObjBox::Explode()
{
  ParticleEngine::AddNow(GetCenter() , 10, particle_FIRE, true);
  ApplyExplosion(GetCenter(), GameMode::GetInstance()->bonus_box_explosion_cfg); //reuse the bonus_box explosion
}

void ObjBox::SignalGhostState(bool /*was_already_dead*/)
{
  if (m_energy > 0) return;
  Explode();
}

Sprite* ObjBox::CreateIcon()
{
  ASSERT(anim);
  Sprite *icon = new Sprite(*anim);
  icon->Scale(0.4, 0.4);
  icon->FixParameters();
  return icon;
}

// Static methods
int ObjBox::start_life_points = 41;
