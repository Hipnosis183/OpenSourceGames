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
 * List of objects that are displayed.
 *****************************************************************************/

#include "object/objects_list.h"
//-----------------------------------------------------------------------------
#include "object/barrel.h"
#include "include/app.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/camera.h"
#include <WARMUX_debug.h>
#include <WARMUX_rectangle.h>
#include "game/game_time.h"
#include "weapon/mine.h"
#include <vector>
#include <iostream>

ObjectsList::ObjectsList()
{}

ObjectsList::~ObjectsList()
{
  FreeMem();
}

void ObjectsList::PlaceMines()
{
  MSG_DEBUG("lst_objects","Placing mines");
  for (uint i=0; i<ActiveMap()->LoadedData()->GetNbMine(); ++i)
  {
    ObjMine *obj = new ObjMine(*MineConfig::GetInstance());
    Double detection_range_factor = 1.5;
    if (obj->PutRandomly(false, MineConfig::GetInstance()->detection_range * PIXEL_PER_METER * detection_range_factor))
      // detection range is in meter
      push_back(obj);
    else
      delete obj;
  }
}

void ObjectsList::PlaceBarrels()
{
  MSG_DEBUG("lst_objects","Placing barrels");
  for (uint i= 0; i<ActiveMap()->LoadedData()->GetNbBarrel(); ++i)
  {
    PetrolBarrel *obj = new PetrolBarrel();

    if (obj->PutRandomly(false, 20.0))
      push_back(obj);
    else
      delete obj;
  }
}


//-----------------------------------------------------------------------------
void ObjectsList::Refresh()
{
  ObjectsList::iterator object=begin();

  while(object != end())
  {
    (*object)->UpdatePosition();

    if (!(*object)->IsGhost()) {
      // Update position may lead to a Ghost object, we
      // must not to refresh in that case
      (*object)->Refresh();
    }

    if ((*object)->IsGhost()) {
      // Stop following this object, remove from overlapse reference then delete it.
      Camera::GetInstance()->StopFollowingObj(*object);
      RemoveOverlappedObjectReference(*object);
      delete (*object);
      object = erase(object);
    } else {
      object++;
    }
  }
}

//-----------------------------------------------------------------------------
void ObjectsList::Draw()
{
  for (ObjectsList::iterator it = begin();
       it != end();
       ++it)
  {
    ASSERT((*it) != NULL);

    if (!(*it)->IsGhost())
      (*it)->Draw();
  }
}

//-----------------------------------------------------------------------------
bool ObjectsList::AllReady() const
{
  FOR_EACH_OBJECT(object)
  {
    if (!(*object)->IsImmobile())
    {
      MSG_DEBUG("lst_objects", "\"%s\" is not ready ( IsImmobile()==false )",
                (*object)->GetName().c_str());
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------

void ObjectsList::FreeMem()
{
  ObjectsList::iterator object;
  for (object = begin(); object != end(); ++object) {
    if ((*object))
      delete(*object);
  }
  clear();
}

//-----------------------------------------------------------------------------

void ObjectsList::RemoveOverlappedObjectReference(const PhysicalObj * obj)
{
  for (iterator it = overlapped_objects.begin(); it != overlapped_objects.end();) {

    if ((*it)->GetOverlappingObject() == obj) {
      MSG_DEBUG("lst_objects", "removing overlapse reference of \"%s\" (%p) in \"%s\"",
                obj->GetName().c_str(), obj, (*it)->GetName().c_str());
      (*it)->SetOverlappingObject(NULL);
      it = overlapped_objects.erase(it);

    } else if ((*it) == obj) {
      MSG_DEBUG("lst_objects", "removing overlapse object of \"%s\" (%p)",
                obj->GetName().c_str(), obj);
      it = overlapped_objects.erase(it);
    } else {
      ++it;
    }
  }
}

void ObjectsList::AddOverlappedObject(PhysicalObj * obj)
{
  MSG_DEBUG("lst_objects", "adding overlapsed object \"%s\" %p",
            obj->GetName().c_str(), obj);

  overlapped_objects.push_back(obj);
}

void ObjectsList::RemoveOverlappedObject(PhysicalObj * obj)
{
  MSG_DEBUG("lst_objects", "removing overlapsed object \"%s\" %p",
            obj->GetName().c_str(), obj);

  overlapped_objects.remove(obj);
}
