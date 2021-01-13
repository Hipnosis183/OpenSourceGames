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
 * Particle Engine
 *****************************************************************************/

#include "particles/bullet.h"
#include "particles/particle.h"
#include "game/game_time.h"
#include "graphic/sprite.h"

const int BULLET_PARTICLE_FADE_TIME = 2000;

BulletParticle::BulletParticle() :
  Particle("bullet_particle")
{
  SetCollisionModel(true, false, false);
  m_rebound_sound = "weapon/m16_cartridge";
  m_time_left_to_live = 1;
  start_to_fade = 0;

  image = ParticleEngine::GetSprite(BULLET_spr);
  image->Scale(1.0,1.0);
  SetSize(Point2i(1, 1));
}

void BulletParticle::Refresh()
{
  if(IsOutsideWorldXY(GetPosition())) {
    m_time_left_to_live = 0;
    return;
  }
  int current_time = GameTime::GetInstance()->Read();
  UpdatePosition();
  image->Update();
  if(start_to_fade > 0) {
    m_time_left_to_live = start_to_fade + BULLET_PARTICLE_FADE_TIME - current_time;
    m_time_left_to_live = (m_time_left_to_live > 0 ? m_time_left_to_live : 0);
    image->SetAlpha(ONE - ((Double)(current_time - start_to_fade)) / BULLET_PARTICLE_FADE_TIME);
  } else {
    // FIXME this is still a ugly hack
    image->SetRotation_rad((GameTime::GetInstance()->Read()/4) % 3 /* 3 is arbitrary */ );
  }
}

void BulletParticle::SignalRebound()
{
  PhysicalObj::SignalRebound();
  //SetCollisionModel(false, false, false);
  StopMoving();
  start_to_fade = GameTime::GetInstance()->Read();
}
