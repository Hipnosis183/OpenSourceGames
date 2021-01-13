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
 * Generic Box
 *****************************************************************************/

#ifndef OBJBOX_H
#define OBJBOX_H
//-----------------------------------------------------------------------------
#include "object/physical_obj.h"
#include "sound/sound_sample.h"

class Team;
class Character;
class Sprite;
class Surface;
typedef struct _xmlNode xmlNode;
class Action;

class ObjBox : public PhysicalObj //it would be nice to name this "Box", but that was already taken...
{
  SoundSample hit;

  virtual void ApplyBox(Team &/*team*/, Character &/*character*/){}
  void CloseParachute();

public:
  ObjBox(const std::string &name);
  ~ObjBox();

  void DropBox();
  static void LoadXml(const xmlNode*  /*object*/){};

  void Draw();
  virtual void Refresh();
  virtual void Randomize() {};
  virtual void ApplyBonus(Character *) {};

  // You must implement this, ideally using a static Sprite*
  virtual const Surface* GetIcon() const = 0;

protected:
  bool parachute;
  Sprite *anim;
  static int start_life_points;
  void Explode();

  virtual void SignalGroundCollision(const Point2d& my_speed_before, const Double& contactAngle);
  virtual void SignalObjectCollision(const Point2d& my_speed_before,
                                     PhysicalObj *object,
                                     const Point2d& object_speed);
  virtual void SignalDrowning();
  virtual void SignalGhostState(bool was_already_dead);

  // This returns you a scaled version of your anim Sprite*
  Sprite *CreateIcon();
};

//-----------------------------------------------------------------------------
#endif /* OBJBOX_H */
