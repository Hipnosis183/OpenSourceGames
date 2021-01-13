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

#ifndef PARTICLE_H
#define PARTICLE_H

#include <list>
#include "object/physical_obj.h"
#include <WARMUX_base.h>

// Forward declarations
class Sprite;
class Surface;

typedef enum {
  particle_DARK_SMOKE,
  particle_SMOKE,
  particle_ILL_BUBBLE,
  particle_FIRE,
  particle_STAR,
  particle_BULLET,
  particle_GROUND,
  particle_AIR_HAMMER,
  particle_MAGIC_STAR,
  particle_BODY_MEMBER,
  particle_TELEPORT_MEMBER,
  particle_POLECAT_FART,
  particle_WATER,
  particle_CLEARWATER,
  particle_LAVA,
  particle_RADIOACTIVE,
  particle_DIRTYWATER,
  particle_CHOCOLATEWATER,
  particle_EXPLOSION
} particle_t;

typedef enum
{
  SMOKE_spr,
  EXPLOSION_SMOKE_spr,
  EXPLOSION_BIG_SMOKE_spr,
  ILL_BUBBLE_spr,
  FIRE_spr,
  STAR_spr,
  BULLET_spr,
  MAGIC_STAR_R_spr,
  MAGIC_STAR_Y_spr,
  MAGIC_STAR_B_spr,
  DARK_SMOKE_spr,
  POLECAT_FART_spr,
  CLEARWATER_spr,
  LAVA_spr,
  RADIOACTIVE_spr,
  DIRTYWATER_spr,
  CHOCOLATEWATER_spr,
  EXPLOSION_spr
} particle_spr;

const int particle_spr_nbr = EXPLOSION_spr + 1;

class Particle : public PhysicalObj
{
protected:
  bool on_top; // if true displayed on top of characters and weapons
  uint m_initial_time_to_live;
  uint m_time_left_to_live;
  bool m_check_move_on_end_turn;
  uint m_time_between_scale;
  uint m_last_refresh;

  Sprite *image;

public:
  Particle(const std::string &name);
  ~Particle();
  virtual void Draw();
  virtual void Refresh();
  void SetOnTop(bool b) { on_top = b; }
  bool IsOnTop() const { return on_top; }
  bool StillUseful() const { return m_time_left_to_live > 0; }
  bool CheckOnEndTurn() const { return m_check_move_on_end_turn; }
};

class ParticleEngine
{
  uint m_last_refresh;
  uint m_time_between_add;

  static bool sprites_loaded;
  static Sprite* particle_sprite[particle_spr_nbr];
  static std::list<Particle *> lst_particles;

  static void AddLittleESmoke(const Point2i &pos, const uint &radius);
  static void AddBigESmoke(const Point2i &pos, const uint &radius);

public:
  ParticleEngine(uint time=100);
  void AddPeriodic(const Point2i &position,
                   particle_t type,
                   bool upper,
                   Double angle=-1, Double norme=-1);

  static void Load();
  static void FreeMem();
  static Sprite* GetSprite(particle_spr type);

  static void AddNow(const Point2i &position,
                     uint nb_particles, particle_t type,
                     bool upper,
                     Double angle=-1, Double norme=-1);
  static void AddNow(Particle* particle);

  enum ESmokeStyle { NoESmoke, LittleESmoke, BigESmoke }; // Style of smoke explosion (quantitie of smoke)
  static void AddExplosionSmoke(const Point2i &pos, const uint &radius, const ESmokeStyle &style);

  static void Refresh();
  static void Draw(bool upper);
  static void Stop();
  static PhysicalObj * IsSomethingMoving();
};

#endif /* PARTICLE_H */
