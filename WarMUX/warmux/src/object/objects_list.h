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
 * Stores most of the object playing in the game
 *****************************************************************************/

#ifndef OBJECTS_LIST_H
#define OBJECTS_LIST_H
//-----------------------------------------------------------------------------
#include <WARMUX_base.h>
#include <WARMUX_singleton.h>
#include "object/physical_obj.h"
#include <list>
//-----------------------------------------------------------------------------

// Loop for all objects
#define FOR_ALL_OBJECTS(object) \
  for (ObjectsList::iterator object=ObjectsList::GetRef().begin(), \
       end=ObjectsList::GetRef().end(); \
       object != end; \
       ++object)

//-----------------------------------------------------------------------------

// Loop for all objects that aren't out of the screen
#define FOR_EACH_OBJECT(object) \
  FOR_ALL_OBJECTS(object) \
    if (!(*object)->IsGhost())

//-----------------------------------------------------------------------------

class ObjectsList : public Singleton<ObjectsList>, public std::list<PhysicalObj*>
{
  ObjectsList();
  ~ObjectsList();
  friend class Singleton<ObjectsList>;

  void RemoveOverlappedObjectReference(const PhysicalObj * obj);

public:
  typedef std::list<PhysicalObj*>::iterator iterator;
  std::list<PhysicalObj*> overlapped_objects;

public:
  // Call the Refresh method of all the objects
  void Refresh();
  // Call the Draw method of all the objects
  void Draw();

  bool AllReady() const;

  // Place mines randomly on the map
  void PlaceMines();
  // Place barrels randomly on the map
  void PlaceBarrels();

  void FreeMem();

  void AddObject(PhysicalObj * obj)
  {
    // bug #16834 and some others probably: set last runtime
    // to a realistic value
    obj->ResetLastRunTime();
    push_back(obj);
  }

  // Overlapse handling
  void RemoveObject(PhysicalObj * obj)
  {
    remove(obj);
    RemoveOverlappedObjectReference(obj);
  }

  void AddOverlappedObject(PhysicalObj * obj);
  void RemoveOverlappedObject(PhysicalObj * obj);
};

//-----------------------------------------------------------------------------
#endif
