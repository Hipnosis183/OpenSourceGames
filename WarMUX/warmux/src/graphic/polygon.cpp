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
 * Polygon class. Store point of a polygon and handle affine transformation
 *****************************************************************************/

#include <WARMUX_types.h>
#include "graphic/polygon.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "tool/resource_manager.h"
#include "tool/affine_transform.h"
#include "tool/math_tools.h"


//=========== POLYGON BUFFER ============ //
// Use this structure to store transformed point
// In affine transformation, never transform directly the original point !
// If you do it, your point will become dented.

PolygonBuffer::PolygonBuffer()
{
  // Start with at least 32 points buffer
  array_size = 32;
  vx = new int16_t[array_size];
  vy = new int16_t[array_size];
  buffer_size = 0;
}

void PolygonBuffer::SetSize(const int size)
{
  if (array_size > size) {
    buffer_size = size;
  } else {
    int16_t * tmp_vx = vx;
    int16_t * tmp_vy = vy;
    // Double the buffer size (64, 128, 256, 512)
    // to avoid call of delete/new at each new point
    array_size = (array_size * 2 > size ? array_size * 2 : size);
    vx = new int16_t[array_size];
    vy = new int16_t[array_size];
    for (int i = 0; i < buffer_size; i++) {
      vx[i] = tmp_vx[i];
      vy[i] = tmp_vy[i];
    }
    buffer_size = size;
    delete[] tmp_vx;
    delete[] tmp_vy;
  }
}

//=========== POLYGON ITEM ============ //
// Use this structure to draw item

PolygonItem::PolygonItem()
{
  transformed_position = position = Point2d(0, 0);
  SetSprite(NULL);
  SetAlignment(H_CENTERED, V_CENTERED);
}

PolygonItem::PolygonItem(PolygonItem * other)
{
  transformed_position = other->transformed_position;
  position = other->position;
  SetPosition(other->GetPosition());
  SetSprite(new Sprite(*(other->GetSprite())));
  SetAlignment(other->h_align, other->v_align);
}

PolygonItem::PolygonItem(const Sprite * sprite, const Point2d & pos, H_align h_a, V_align v_a)
{
  SetPosition(pos);
  SetSprite(new Sprite(*sprite));
  SetAlignment(h_a, v_a);
}

PolygonItem::~PolygonItem()
{
  delete item;
}

void PolygonItem::ApplyTransformation(const AffineTransform2D & trans)
{
  transformed_position = trans * position;
}

void PolygonItem::Draw(Surface * dest)
{
  if (!item)
    return;
  item->Blit(*dest, GetOffsetAlignment());
}

bool PolygonItem::Contains(const Point2d & p) const
{
  return Rectanglei(GetOffsetAlignment(), item->GetSize()).Contains(Point2i(p.x, p.y));
}

Point2i PolygonItem::GetOffsetAlignment() const
{
  Point2i offset;
  switch(v_align) {
    case TOP: offset.y = (int)transformed_position.y; break;
    case BOTTOM: offset.y = (int)transformed_position.y - item->GetHeight(); break;
    default : offset.y = (int)transformed_position.y - (item->GetHeight() / 2); break;
  }
  switch(h_align) {
    case LEFT: offset.x = (int)transformed_position.x; break;
    case RIGHT: offset.x = (int)transformed_position.x - item->GetWidth(); break;
    default : offset.x = (int)transformed_position.x - (item->GetWidth() / 2); break;
  }
  return offset;
}

//=========== POLYGON ============ //
// Store a vector of points and handle affine transformation,
// Bezier interpolation handling etc.
Polygon::Polygon(const std::vector<Point2d>& shape)
{
  Init();
  transformed_shape = original_shape = shape;
  shape_buffer->SetSize(original_shape.size());
}

Polygon::Polygon(Polygon & poly)
{
  Init();
  texture = poly.texture;
  if (poly.IsPlaneColor()) {
    plane_color = new Color(poly.GetPlaneColor());
  }
  if (poly.IsBordered()) {
    border_color = new Color(poly.GetBorderColor());
  }
  transformed_shape = original_shape = poly.original_shape;
  shape_buffer->SetSize(original_shape.size());
  for (std::vector<PolygonItem *>::iterator elt = poly.items.begin(); elt != poly.items.end(); elt++) {
    AddItem((*elt)->GetSprite(), (*elt)->GetPosition(), (*elt)->GetHAlign(), (*elt)->GetVAlign());
  }
}

Polygon::~Polygon()
{
  ClearItem();
  if (texture)
    delete texture;
  if (border_color)
    delete border_color;
  if (plane_color)
    delete plane_color;
  delete shape_buffer;

  shape_buffer = NULL;
  border_color = NULL;
  plane_color = NULL;
  texture = NULL;
}

// Only called by constructor, so setting pointer values is valid
void Polygon::Init()
{
  is_closed = true;
  texture = NULL;
  plane_color = NULL;
  border_color = NULL;
  original_shape.clear();
  transformed_shape.clear();
  items.clear();
  shape_buffer = new PolygonBuffer();
  min = max = Point2d(0.0, 0.0);
}

void Polygon::ApplyTransformation(const AffineTransform2D & trans, bool save_transformation)
{
  int i = 0;
  for (std::vector<Point2d>::iterator point = original_shape.begin();
      point != original_shape.end(); point++, i++) {
    transformed_shape[i] = trans * (*point);
    if (save_transformation)
      (*point) = transformed_shape[i];
    shape_buffer->vx[i] = (int)transformed_shape[i].x;
    shape_buffer->vy[i] = (int)transformed_shape[i].y;
    if (i == 0) {
      max = min = transformed_shape[i];
    } else {
      max = max.max(transformed_shape[i]);
      min = min.min(transformed_shape[i]);
    }
  }
  for (std::vector<PolygonItem *>::iterator item = items.begin();
      item != items.end(); item++) {
    (*item)->ApplyTransformation(trans);
  }
}

// Reset the point
void Polygon::ResetTransformation()
{
  for (int i = 0; i < (int)original_shape.size(); i++) {
    transformed_shape[i] = original_shape[i];
    shape_buffer->vx[i] = (int)original_shape[i].x;
    shape_buffer->vy[i] = (int)original_shape[i].y;
    if (i == 0) {
      max = min = transformed_shape[i];
    } else {
      max = max.max(transformed_shape[i]);
      min = min.min(transformed_shape[i]);
    }
  }
}

// Check if a point is inside the polygon using Jordan curve theorem (amen).
// For better explanation : http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
// Note : Compile but not tested yet !
bool Polygon::IsInsidePolygon(const Point2d & p) const
{
  int i, j;
  bool c = false;
  for (i = 0, j = GetNbOfPoint() - 1; i < GetNbOfPoint(); j = i++) {
    if ((((transformed_shape[i].y <= p.y) && (p.y < transformed_shape[j].y)) ||
         ((transformed_shape[j].y <= p.y) && (p.y < transformed_shape[i].y))) &&
        (p.x < (transformed_shape[j].x - transformed_shape[i].x) * (p.y - transformed_shape[i].y) / (transformed_shape[j].y - transformed_shape[i].y) + transformed_shape[i].x))
          c = !c;
  }
  return c;
}

// we process the area size. If < 0 => clockwise else anticlokwise
// Warning ! The polygon must be concave
bool Polygon::IsClockWise() const
{
  Point2d a = original_shape[0];
  Point2d b = original_shape[original_shape.size() / 3];
  Point2d c = original_shape[(original_shape.size() * 2) / 3];
  return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y) < 0;
}

// Not 100% accurate but sufficient for now
bool Polygon::IsOverlapping(const Polygon & poly) const
{
  int nbPoint = GetNbOfPoint();
  for (int i = 0; i < nbPoint; i++) {
    if (poly.IsInsidePolygon(transformed_shape[i])) {
      return true;
    }
  }
  return false;
}

void Polygon::AddPoint(const Point2d & p)
{
  original_shape.push_back(p);
  transformed_shape.push_back(p);
  shape_buffer->SetSize(original_shape.size());
  shape_buffer->vx[original_shape.size() - 1] = (int)p.x;
  shape_buffer->vy[original_shape.size() - 1] = (int)p.y;
  max = p.max(max);
  min = p.min(min);
}

void Polygon::InsertPoint(int index, const Point2d & p)
{
  if (index == GetNbOfPoint()) {
    AddPoint(p);
    return;
  }
  std::vector<Point2d> vector_tmp;
  std::vector<Point2d>::iterator point;
  Point2d tmp;
  int i = 0;
  shape_buffer->SetSize(original_shape.size() + 1);
  // Inserting first part of the point
  for (point = original_shape.begin();
      point != original_shape.end() && i < index; point++, i++) {
    tmp = *point;
    vector_tmp.push_back(tmp);
    shape_buffer->vx[i] = (int)tmp.x;
    shape_buffer->vy[i] = (int)tmp.y;
  }
  // Inserting a new point
  vector_tmp.push_back(p);
  shape_buffer->vx[i] = (int)p.x;
  shape_buffer->vy[i++] = (int)p.y;
  // And inserting remaining points of previous shape
  for (; point != original_shape.end(); point++, i++) {
    tmp = *point;
    vector_tmp.push_back(tmp);
    shape_buffer->vx[i] = (int)tmp.x;
    shape_buffer->vy[i] = (int)tmp.y;
  }
  transformed_shape = original_shape = vector_tmp;
}

void Polygon::DeletePoint(int index)
{
  std::vector<Point2d> vector_tmp;
  Point2d tmp;
  int i = 0;
  shape_buffer->SetSize(shape_buffer->GetSize() - 1);
  for (std::vector<Point2d>::iterator point = original_shape.begin();
      point != original_shape.end(); point++, i++) {
    if (i == index) continue; // Skip point to remove
    tmp = (*point);
    vector_tmp.push_back(*point);
    shape_buffer->vx[i] = (int)tmp.x;
    shape_buffer->vy[i] = (int)tmp.y;
    max = max.max(tmp);
    min = min.min(tmp);
  }
  transformed_shape = original_shape = vector_tmp;
}

void Polygon::DelItem(int index)
{
  std::vector<PolygonItem *> vector_tmp;
  Point2d tmp;
  int i = 0;
  for (std::vector<PolygonItem *>::iterator item = items.begin();
       item != items.end(); item++, i++) {
    if (i == index) // Skip point to remove
      continue;
    vector_tmp.push_back(*item);
  }
  items = vector_tmp;
}

void Polygon::ClearItem(bool free_mem)
{
  for (std::vector<PolygonItem *>::iterator item = items.begin();
       item != items.end(); item++) {
    if (free_mem)
      delete (*item);
  }
  items.clear();
}

Point2d Polygon::GetRandomUpperPoint()
{
  std::vector<Point2d>::iterator point = transformed_shape.begin();
  Point2d tmp, previous;
  tmp = *point;
  MSG_DEBUG("random.get", "Polygon::GetRandomUpperPoint()");
  int start = RandomSync().GetInt(0, GetNbOfPoint());
  int i;
  for (i = 0; i < start; i++)
    point++;
  Double max_tmp = 0.4;
  while(point != transformed_shape.end()) {
    previous = *point++;
    i++;
    tmp = *point - previous;
    tmp = tmp / tmp.Norm();
    if (tmp.y > max_tmp)
      return tmp;
  }
  return Point2d();
}

// And the famous Bezier curve. And this algorithm is that simple ? I'm so disappointed !
// But now you can tell the world warmux is using Bezier curves.
void Polygon::AddBezierCurve(const Point2d& anchor1, const Point2d& control1,
                             const Point2d& control2, const Point2d& anchor2,
                             const int num_steps, const bool add_first_point,
                             const bool add_last_point)
{
  Point2d tmp1 = anchor1 + control1;
  Point2d tmp2 = anchor2 + control2;
  Double a, b;
  if (add_first_point)
    AddPoint(anchor1);
  for (int step = 1; step < num_steps - 1; step++) {
    a = step / (Double)num_steps;
    b = 1 - a;
    AddPoint(anchor1 * b * b * b + tmp1 * THREE * b * b * a + tmp2 * THREE * b * a * a + anchor2 * a * a * a);
  }
  if (add_last_point)
    AddPoint(anchor2);
}

// Generate random point between 2 points
void Polygon::AddRandomCurve(const Point2d& start, const Point2d& end,
                             const Double x_random_offset, const Double y_random_offset,
                             const int num_steps, const bool add_first_point,
                             const bool add_last_point)
{
  Point2d step = (end - start) / num_steps;
  Point2d tmp;
  if (add_first_point)
    AddPoint(start);
  for (int i = 1; i < num_steps - 1; i++) {
    MSG_DEBUG("random.get", "Polygon::AddRandomCurve(...)");
    Double x = RandomSync().GetDouble(-x_random_offset, x_random_offset);
    MSG_DEBUG("random.get", "Polygon::AddRandomCurve(...)");
    Double y = RandomSync().GetDouble(-y_random_offset, y_random_offset);
    AddPoint(start + (step * i) + Point2d(x,y));
  }
  if (add_last_point)
    AddPoint(end);
}

// Generate a new polygon with Bezier interpolation
Polygon * Polygon::GetBezierInterpolation(Double smooth_value, int num_steps, Double rand)
{
  Point2d p0, p1, p2, p3, c0, c1, c2, v1, v2;
  Polygon * shape = new Polygon();
  Double l1, l2, l3;
  AffineTransform2D trans = AffineTransform2D();
  for (int index_p1 = 0; index_p1 < (int)original_shape.size(); index_p1++) {
    p0 = original_shape[(index_p1 == 0 ? original_shape.size() : index_p1) - 1];
    p1 = original_shape[index_p1];
    p2 = original_shape[(index_p1 + 1) % original_shape.size()];
    p3 = original_shape[(index_p1 + 2) % original_shape.size()];

    // compute center of [p0,p1], [p1,p2] and [p2,p3]
    c0 = p0 + ONE_HALF*(p1 - p0);
    c1 = p1 + ONE_HALF*(p2 - p1);
    c2 = p2 + ONE_HALF*(p3 - p2);

    // Distance
    l1 = p0.Distance(p1);
    l2 = p1.Distance(p2);
    l3 = p2.Distance(p3);

    // Point control
    v1 = (c1 - c0) * (l2 / (l1 + l2)) * smooth_value;
    v2 = (c1 - c2) * (l2 / (l2 + l3)) * smooth_value;

    // Randomization
    if (rand != ZERO) {
      MSG_DEBUG("random.get", "Polygon::GetBezierInterpolation(...)");
      trans.SetRotation(RandomSync().GetDouble(-rand, rand));
      v1 = trans * v1;
      MSG_DEBUG("random.get", "Polygon::GetBezierInterpolation(...)");
      trans.SetRotation(RandomSync().GetDouble(-rand, rand));
      v2 = trans * v2;
    }

    shape->AddBezierCurve(p1, v1, v2, p2, num_steps, false);
  }
  for (std::vector<PolygonItem *>::iterator elt = items.begin(); elt != items.end(); elt++) {
    shape->AddItem((*elt)->GetSprite(), (*elt)->GetPosition(), (*elt)->GetHAlign(), (*elt)->GetVAlign());
  }
  return shape;
}

// expand the polygon (to draw a little border for example)
void Polygon::Expand(Double expand_value)
{
  if (original_shape.size() < 2)
    return;
  if (!IsClockWise())
    expand_value = -expand_value;
  std::vector<Point2d> tmp_shape;
  AffineTransform2D trans = AffineTransform2D::Rotate(HALF_PI);
  Point2d current, next, vect, expand;
  int i, j, k;
  for (i = 0; i < (int)original_shape.size(); i++) {
    j = (i + 1) % original_shape.size();
    current = original_shape[i];
    next    = original_shape[j];
    // If the next point is to close to current point skip next point
    // Avoid visual artefact
    k = 0;
    Double min_distance = 0.1;
    while(k < 10 && next.Distance(current) < min_distance) {
      j = (j + 1) % original_shape.size();
      next    = original_shape[j];
      k++;
    }
    vect = trans * (next - current);
    vect = (vect / vect.Norm()) * expand_value; // Normalize and length
    expand = current + vect;
    tmp_shape.push_back(expand);
    shape_buffer->vx[i] = (int)expand.x;
    shape_buffer->vy[i] = (int)expand.y;
  }
  original_shape.clear();
  transformed_shape = original_shape = tmp_shape;
}

// Color handling
void Polygon::SetBorderColor(const Color & color)
{
  if (!border_color)
    border_color = new Color(color);
  else
    *border_color = color;
}

void Polygon::SetPlaneColor(const Color & color)
{
  if (!plane_color)
    plane_color = new Color(color);
  else
    *plane_color = color;
}

void Polygon::Draw(Surface * dest)
{
  // Draw polygon
  if (is_closed) {
    if (IsPlaneColor())
      dest->FilledPolygon(shape_buffer->vx, shape_buffer->vy, shape_buffer->GetSize(), *plane_color);
    if (IsTextured())
      dest->TexturedPolygon(shape_buffer->vx, shape_buffer->vy, shape_buffer->GetSize(), texture, 0, 0);
    if (IsBordered())
      dest->AAPolygonColor(shape_buffer->vx, shape_buffer->vy, shape_buffer->GetSize(), *border_color);
  } else {
    // Draw Line
    Color *tmp;
    if (IsBordered())
      tmp = border_color;
    else
      tmp = plane_color;
    if (tmp) {
      for (int i = 0; i < shape_buffer->GetSize() - 1; i++) {
        dest->AALineColor(shape_buffer->vx[i], shape_buffer->vx[i + 1],
                          shape_buffer->vy[i], shape_buffer->vy[i + 1], *tmp);
      }
    }
  }
  // Draw Item
  for (std::vector<PolygonItem *>::iterator item = items.begin();
       item != items.end(); item++) {
    (*item)->Draw(dest);
  }
}

void Polygon::DrawOnScreen()
{
  Draw(&GetMainWindow());
  GetWorld().ToRedrawOnScreen(GetRectangleToRefresh());
}

////////////////////////////
// DecoratedBox

DecoratedBox::DecoratedBox(Double width, Double height)
 : Polygon()
 , m_border(NULL)
 , m_style(DecoratedBox::STYLE_ROUNDED)
{
  min = Point2d(0.0, 0.0);
  max = Point2d(width, height);
  original_min = min;
  original_max = max;
}

DecoratedBox::~DecoratedBox()
{
  if (m_border)
    delete m_border;
}

void DecoratedBox::Draw(Surface * dest)
{
  if (!m_border) {
    //m_border = new Surface(Point2i(GetSize().x, GetSize().y), SDL_SWSURFACE, true);
    Surface border = Surface(GetSize(), SDL_SWSURFACE, true);
    GenerateStyledBorder(border, m_style);
    m_border = new Surface(border.DisplayFormatAlpha());
  }
  dest->Blit(*m_border, min);


  // Draw Item
  for (std::vector<PolygonItem *>::iterator item = items.begin();
      item != items.end(); item++) {
    (*item)->Draw(dest);
  }
}

void DecoratedBox::SetPosition(Double x, Double y)
{
    for (std::vector<PolygonItem *>::iterator item = items.begin();
      item != items.end(); item++) {
      Point2d old_pos = (*item)->GetPosition();
      old_pos.x += x - min.x;
      old_pos.y += y - min.y;
      (*item)->SetPosition(old_pos);
    }
    max.x = x - min.x +max.x ;
    max.y = y - min.y +max.y;
    min.x = x;
    min.y = y;
    original_min = min;
    original_max = max;
}

void GenerateStyledBorder(Surface & source, DecoratedBox::Style style)
{
  Profile     *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  Surface     rounding_style[3][3];
  std::string style_str;
  int         i, j;

  switch (style)
  {
    case DecoratedBox::STYLE_ROUNDED:
    style_str = "rounding";
    break;
    case DecoratedBox::STYLE_SQUARE:
    style_str = "square";
    break;
  }

  // styled box
  rounding_style[1][2] = LOAD_RES_IMAGE("interface/"+style_str+"_bottom");
  rounding_style[0][2] = LOAD_RES_IMAGE("interface/"+style_str+"_bottom_left");
  rounding_style[2][2] = LOAD_RES_IMAGE("interface/"+style_str+"_bottom_right");
  rounding_style[1][0] = LOAD_RES_IMAGE("interface/"+style_str+"_top");
  rounding_style[0][0] = LOAD_RES_IMAGE("interface/"+style_str+"_top_left");
  rounding_style[2][0] = LOAD_RES_IMAGE("interface/"+style_str+"_top_right");
  rounding_style[0][1] = LOAD_RES_IMAGE("interface/"+style_str+"_left");
  rounding_style[2][1] = LOAD_RES_IMAGE("interface/"+style_str+"_right");
  rounding_style[1][1] = LOAD_RES_IMAGE("interface/"+style_str+"_center");
  for (j=0; j<3; j++)
    for (i=0; i<3; i++)
      rounding_style[j][i].SetAlpha(0, 0);

  Surface save_surf(source.GetSize(),SDL_SWSURFACE, true);
  source.SetAlpha(0, 0);
  save_surf.Blit(source);

  Rectanglei temp_rect(Point2i(0,0), source.GetSize());

  Point2i temp_position;

  temp_position = temp_rect.GetPosition();
  source.Blit(rounding_style[0][0], temp_position);

  temp_position = temp_rect.GetPosition();
  temp_position.x += temp_rect.GetSize().x - rounding_style[2][0].GetSize().x;
  source.Blit(rounding_style[2][0], temp_position);

  temp_position = temp_rect.GetPosition();
  temp_position.y += temp_rect.GetSize().y - rounding_style[0][2].GetSize().y;
  source.Blit(rounding_style[0][2], temp_position);

  temp_position = temp_rect.GetPosition();
  temp_position.x += temp_rect.GetSize().x - rounding_style[2][2].GetSize().x;
  temp_position.y += temp_rect.GetSize().y - rounding_style[2][2].GetSize().y;
  source.Blit(rounding_style[2][2], temp_position);


  for (i = rounding_style[0][0].GetSize().x;
       i < temp_rect.GetSize().x - rounding_style[2][0].GetSize().x;
       i++) {
    temp_position = temp_rect.GetPosition();
    temp_position.x += i;
    source.Blit(rounding_style[1][0], temp_position);

    temp_position.y += temp_rect.GetSize().y - rounding_style[1][2].GetSize().y;
    source.Blit(rounding_style[1][2], temp_position);

  }

  for (i = rounding_style[0][0].GetSize().y;
       i < temp_rect.GetSize().y - rounding_style[0][2].GetSize().y;
       i++) {
    temp_position = temp_rect.GetPosition();
    temp_position.y += i;
    source.Blit(rounding_style[0][1],temp_position);

    temp_position.x += temp_rect.GetSize().x - rounding_style[2][1].GetSize().x;
    source.Blit(rounding_style[2][1],temp_position);

  }

  for (i = rounding_style[0][0].GetSize().x;
       i < temp_rect.GetSize().x - rounding_style[2][0].GetSize().x;
       i++) {

    for (j = rounding_style[0][0].GetSize().y;
         j < temp_rect.GetSize().y - rounding_style[0][2].GetSize().y;
         j++) {
      temp_position = temp_rect.GetPosition() + Point2i(i,j);
      source.Blit(rounding_style[1][1],temp_position);
    }
  }

  source.SetAlpha(SDL_SRCALPHA, 0);
  source.Blit(save_surf);
}

void DecoratedBox::ApplyTransformation(const AffineTransform2D & trans, bool save_transformation)
{
  Point2d new_min = trans * original_min;
  Point2d new_max = trans * original_max;

  if ( round(max.x - min.x +ONE_HALF) != round(new_max.x -new_min.x+ONE_HALF) ||
       round(max.y - min.y) != round(new_max.y -new_min.y) ) {
    delete m_border;
    m_border = NULL;
  }

  min = new_min;
  max = new_max;

  if (save_transformation) {
    original_min = min;
    original_max = max;
  }

  for (std::vector<PolygonItem *>::iterator item = items.begin();
       item != items.end(); item++) {
    (*item)->ApplyTransformation(trans);
  }
}

void DecoratedBox::ResetTransformation()
{
  if ( round(max.x - min.x)!=round(original_max.x -original_min.x) ||
       round(max.y - min.y)!=round(original_max.y -original_min.y) ) {
      delete m_border;
      m_border = NULL;
   }

   min = original_min;
   max = original_max;

}

void DecoratedBox::AddItem(PolygonItem * item)
{
  Point2d old_pos = item->GetPosition();
  old_pos.x += min.x;
  old_pos.y += min.y;
  item->SetPosition(old_pos);
  Polygon::AddItem(item);
}
