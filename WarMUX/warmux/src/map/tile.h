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
 *****************************************************************************/

#ifndef _TILE_H
#define _TILE_H

#include <vector>
#include <WARMUX_point.h>
#include <WARMUX_rectangle.h>
#include <assert.h>

// Forward declarations
class Surface;
class Sprite;
class TileItem;
class TileItem_NonEmpty;

const uint EXPLOSION_BORDER_SIZE = 10;

class Tile : public Rectanglei
{
public:
  typedef struct
  {
    uint16_t index, new_crc;
  } SynchTileInfo;
  typedef std::vector<SynchTileInfo> SynchTileList;

private:
  uint8_t m_alpha_threshold;
  bool    m_use_alpha;

public:
  Tile ();
  ~Tile ();

  // Dig a hole
  void Dig(const Point2i &position, const Surface& provider);
  // Dig a circle hole
  void Dig(const Point2i &center, const uint radius);

  // Insert a sprite into the ground
  void PutSprite(const Point2i& pos, Sprite* spr);
  // Merge a sprite into map (using alpha information)
  void MergeSprite(const Point2i &position, Surface & provider);

  // Load an image
  bool LoadImage(const std::string& filename,
                 uint8_t alpha_threshold,
                 const Point2i & upper_left_offset,
                 const Point2i & lower_right_offset);


  // Is point (x,y) in vacuum ?
  bool IsEmpty(const Point2i &pos) const;

  // Draw it (on the entire visible part)
  void DrawTile();

  // Draw a part that is inside the given clipping rectangle
  // Clipping rectangle is in World coordinate not screen coordinates
  // usefull to redraw only a part that is under a sprite that has moved,...
  void DrawTile_Clipped(const Rectanglei & clip_rectangle) const;

  // Return a surface of the ground inside the rect
  Surface GetPart(const Rectanglei& rec);

  // Return the preview
  Surface* GetPreview() const { return m_preview; };
  void  CheckPreview(bool force = false);
  const Point2i& GetPreviewSize() const { return m_preview_size; };
  const Rectanglei& GetPreviewRect() const { return m_preview_rect; };
  uint GetLastPreviewRedrawTime() const { return m_last_preview_redraw; };
  bool IsPreviewHQ() const { return m_use_alpha; }
  void SetPreviewSizeDelta(int delta);

  // Translate world coordinates into a preview ones and vice versa
  Point2i PreviewCoordinates(const Point2i& pos) { return (pos-m_upper_left_offset)>>m_shift; };
  Point2i FromPreviewCoordinates(const Point2i& pos) { return (pos<<m_shift) + m_upper_left_offset; };

  // Refresh the list of tiles to resynch
  SynchTileList GetTilesToSynch();

  uint32_t GetCRC() const { assert(crc); return crc; }

protected:
  void InitTile(const Point2i &pSize, const Point2i & upper_left_offset, const Point2i & lower_right_offset);
  TileItem_NonEmpty* GetNonEmpty(uint x, uint y);
  TileItem_NonEmpty* CreateNonEmpty(uint8_t *ptr, int stride);

  void FreeMem();
  Point2i Clamp(const Point2i &v) const { return v.clamp(Point2i(0, 0), nbCells - 1); };

  // Ground dimensions
  Point2i nbCells, startCell, endCell;

  void InitPreview();
  Surface*   m_preview;
  uint       m_last_preview_redraw;
  uint       m_shift;
  Point2i    m_last_video_size;
  Point2i    m_preview_size;
  Rectanglei m_preview_rect;

  Point2i m_upper_left_offset;
  Point2i m_lower_right_offset;

  // Pseudo-CRC to validate map data
  uint32_t crc;

  // Canvas giving access to tiles
  std::vector<TileItem *> item;
};

#endif // _TILE_H
