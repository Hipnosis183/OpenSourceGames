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
 * A beautiful checkbox with picture and text
 *****************************************************************************/

#include "gui/picture_text_cbox.h"
#include "include/app.h"
#include "graphic/text.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

PictureTextCBox::PictureTextCBox(const std::string & label,
                                 const std::string & resource_id,
                                 const Point2i & _size,
                                 bool value):
  CheckBox(label, _size.x, value),
  m_image(),
  m_enabled(),
  m_disabled_front(),
  m_disabled_back()
{
  SetFont(dark_gray_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  m_image = LOAD_RES_IMAGE(resource_id);
  m_enabled = LOAD_RES_IMAGE("menu/enabled");
  m_disabled_front = LOAD_RES_IMAGE("menu/disabled_front");
  m_disabled_back = LOAD_RES_IMAGE("menu/disabled_back");
  m_value = value;

  Text::SetMaxWidth(size.x);
  SetSize(_size);
}

PictureTextCBox::PictureTextCBox(Profile * profile,
                                 const xmlNode * widgetNode) :
  CheckBox(profile, widgetNode),
  m_image(),
  m_enabled(),
  m_disabled_front(),
  m_disabled_back()
{
}

bool PictureTextCBox::LoadXMLConfiguration()
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }

  XmlReader * xmlFile = profile->GetXMLDocument();

  ParseXMLGeometry();
  ParseXMLBorder();
  ParseXMLBackground();

  Text::LoadXMLConfiguration(xmlFile, widgetNode);

  std::string file;
  Surface tmp;

  xmlFile->ReadStringAttr(widgetNode, "pictureEnabled", file);
  file = profile->relative_path + file;
  if (!tmp.ImgLoad(file)) {
    file = profile->relative_path + "menu/enabled.png";
    if (!tmp.ImgLoad(file)) {
      Error("XML Loading -> PictureTextCBox: can't load " + file);
    }
  }
  m_enabled = tmp.DisplayFormatAlpha();

  xmlFile->ReadStringAttr(widgetNode, "picture", file);
  file = profile->relative_path + file;
  if (!tmp.ImgLoad(file)) {
    Error("XML Loading -> PictureTextCBox: Node 'picture': can't load the file : " + file);
  }
  m_image = tmp.DisplayFormatAlpha();

  xmlFile->ReadStringAttr(widgetNode, "pictureDisabledFront", file);
  file = profile->relative_path + file;
  if (!tmp.ImgLoad(file)) {
    file = profile->relative_path + "menu/disabled_front.png";
    if (!tmp.ImgLoad(file)) {
      Error("XML Loading -> PictureTextCBox: can't load " + file);
    }
  }
  m_disabled_front = tmp.DisplayFormatAlpha();

  xmlFile->ReadStringAttr(widgetNode, "pictureDisabledBack", file);
  file = profile->relative_path + file;
  if (!tmp.ImgLoad(file)) {
    file = profile->relative_path + "menu/disabled_back.png";
    if (!tmp.ImgLoad(file)) {
      Error("XML Loading -> PictureTextCBox: can't load " + file);
    }
  }
  m_disabled_back = tmp.DisplayFormatAlpha();

  return true;
}

void PictureTextCBox::Draw(const Point2i &/*mousePosition*/)
{
  Surface & video_window = GetMainWindow();

  if (m_value) {
    uint enabled_x = GetPositionX() + (GetSizeX() - m_enabled.GetWidth())/2 ;
    uint enabled_y = GetPositionY();
    uint outside_x = std::max(uint(0), GetPositionX() - enabled_x);
    uint outside_y = std::max(uint(0), GetPositionY() - enabled_y);

    enabled_x += outside_x;
    enabled_y += outside_y;
    Rectanglei srcRect(outside_x, outside_y, m_enabled.GetWidth() - outside_x,
                       m_enabled.GetHeight() - outside_y);
    video_window.Blit(m_enabled, srcRect, Point2i(enabled_x, enabled_y));
  } else {
    uint disabled_x = GetPositionX() + (GetSizeX() - m_disabled_back.GetWidth())/2 ;
    uint disabled_y = GetPositionY();
    uint outside_x = std::max(uint(0), GetPositionX() - disabled_x);
    uint outside_y = std::max(uint(0), GetPositionY() - disabled_y);

    disabled_x += outside_x;
    disabled_y += outside_y;
    Rectanglei srcRect(outside_x, outside_y, m_disabled_back.GetWidth() - outside_x,
                       m_disabled_back.GetHeight() - outside_y);
    video_window.Blit(m_disabled_back, srcRect, Point2i(disabled_x, disabled_y));
  }

  // center the image
  uint tmp_x = GetPositionX() + (GetSizeX() - m_image.GetWidth())/2 ;
  uint tmp_y = GetPositionY() + (m_enabled.GetHeight() - m_image.GetHeight())/2;

  video_window.Blit(m_image, Point2i(tmp_x, tmp_y));

  Text::DrawCenterTop(GetPosition() + Point2i(GetSizeX()/2,
                      GetSizeY() - Text::GetHeight()));

  if (!m_value) {
    uint disabled_x = GetPositionX() + (GetSizeX() - m_disabled_front.GetWidth())/2 ;
    uint disabled_y = GetPositionY() + (m_enabled.GetHeight() - m_disabled_front.GetHeight())/2;
    video_window.Blit(m_disabled_front, Point2i(disabled_x, disabled_y));
  }
}

void PictureTextCBox::Pack()
{
  Text::SetMaxWidth(size.x);
}
