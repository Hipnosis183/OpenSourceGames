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
 * A beautiful button with picture and text
 *****************************************************************************/

#ifndef BUTTON_PIC_H
#define BUTTON_PIC_H

#include "gui/button.h"
#include "graphic/surface.h"
#include "tool/resource_manager.h"

class Text;

class ButtonPic : public Widget
{
  Text * txt_label;
  Surface m_img_normal;

public:
  ButtonPic(const std::string & label,
            const std::string & resource_id,
            const Point2i & size);
  ButtonPic(Profile * profile,
            const xmlNode * buttonPicNode);
  virtual ~ButtonPic();

  virtual bool LoadXMLConfiguration(void);

  virtual void Draw(const Point2i& mousePosition);
  virtual void Pack();
};

#endif

