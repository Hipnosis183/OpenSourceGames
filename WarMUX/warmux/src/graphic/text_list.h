/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2007 Jon de Andres
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
 * List of Text Clases.
 * TextBox is not valid while playing game.
 * Text does not allow \n and similar chars
 *****************************************************************************/

#ifndef TEXT_LIST_H
#define TEXT_LIST_H

#include <list>
#include <WARMUX_base.h>

class Text;
class Color;

class TextList
{
public:
  std::list<Text*> list;
  typedef std::list<Text*>::iterator iterator;

  // Constructors
  TextList() { };
  ~TextList();

  // Manage items in list
  void AddText(const std::string &txt, const Color& color, uint maxlines);
  void DeleteLine() { if (!list.empty()) list.pop_front(); }
  void Clear() { list.clear(); }
  int Size() const { return list.size(); }
  void Draw(int x, int y, int height);
  void DrawLine(const Text* newline, int x, int y, int height) const;
};

#endif
