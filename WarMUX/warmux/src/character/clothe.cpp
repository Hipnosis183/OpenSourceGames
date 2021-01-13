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
 *****************************************************************************/

#include "tool/xml_document.h"
#include <WARMUX_debug.h>
#include "character/clothe.h"
#include "character/member.h"
#include <map>
#include <iostream>

Clothe::Clothe(const xmlNode *                  xml,
               std::map<std::string, Member*> & members_lst)
{
  XmlReader::ReadStringAttr(xml, "name", name);

  xmlNodeArray nodes = XmlReader::GetNamedChildren(xml, "c_member");
  xmlNodeArray::const_iterator it;
  MSG_DEBUG("body.clothe", "   Found %i clothe members in %s", nodes.size(), name.c_str());

  ASSERT(nodes.size());

  std::string att;

  for (it = nodes.begin(); it != nodes.end(); ++it) {

    if (!XmlReader::ReadStringAttr(*it, "name", att)) {
      std::cerr << "Malformed attached clothe member definition" << std::endl;
      continue;
    }

    std::map<std::string, Member *>::iterator itMember = members_lst.find(att);

    if (itMember != members_lst.end()) {
      Member *member = itMember->second;
      layers.push_back(member);
      // Weapon member doesn't have a sprite, don't check it
      if (member->GetType()!="weapon") {
        non_weapon_layers.push_back(member);
        if (member->MustRefresh())
          must_refresh.push_back(itMember->second);
      }
    } else {
      std::cerr << "Undefined clothe member \"" << att << "\"" << std::endl;
    }
  }
}

Clothe::Clothe(Clothe *                         c,
               std::map<std::string, Member*> & members_lst):
  name(c->name)
{
  for (std::vector<Member*>::iterator it = c->layers.begin();
      it != c->layers.end();
      ++it) {
    Member *member = members_lst.find((*it)->GetName())->second;
    layers.push_back(member);
    // Weapon member doesn't have a sprite, don't check it
    if (member->GetType()!="weapon") {
      non_weapon_layers.push_back(member);
      if (member->MustRefresh())
        must_refresh.push_back(member);
    }
  }
}
