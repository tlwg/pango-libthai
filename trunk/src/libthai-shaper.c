/* Pango
 * thai-shaper.c:
 *
 * Copyright (C) 1999 Red Hat Software
 * Author: Owen Taylor <otaylor@redhat.com>
 *
 * Software and Language Engineering Laboratory, NECTEC
 * Author: Theppitak Karoonboonyanan <thep@links.nectec.or.th>
 *
 * Copyright (c) 1996-2000 by Sun Microsystems, Inc.
 * Author: Chookij Vanatham <Chookij.Vanatham@Eng.Sun.COM>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.         See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include <string.h>

#include <glib.h>
#include <pango/pango-engine.h>
#include <thai/thrend.h>
#include "libthai-shaper.h"


static void
add_glyph (ThaiFontInfo     *font_info, 
           PangoGlyphString *glyphs, 
           gint              cluster_start, 
           PangoGlyph        glyph,
           gboolean          combining)
{
  PangoRectangle ink_rect, logical_rect;
  gint index = glyphs->num_glyphs;

  pango_glyph_string_set_size (glyphs, index + 1);
  
  glyphs->glyphs[index].glyph = glyph;
  glyphs->glyphs[index].attr.is_cluster_start = combining ? 0 : 1;
  
  glyphs->log_clusters[index] = cluster_start;

  pango_font_get_glyph_extents (font_info->font,
                                glyphs->glyphs[index].glyph, &ink_rect, &logical_rect);

  if (combining || logical_rect.width > 0)
    {
      glyphs->glyphs[index].geometry.x_offset = 0;
      glyphs->glyphs[index].geometry.width = logical_rect.width;
    }
  else
    {
      glyphs->glyphs[index].geometry.x_offset = ink_rect.width;
      glyphs->glyphs[index].geometry.width = ink_rect.width;
    }
  glyphs->glyphs[index].geometry.y_offset = 0;
}

static int
make_font_glyphs (ThaiFontInfo *font_info,
                  thglyph_t *log_glyphs, int n_log_glyphs,
                  PangoGlyph *pango_glyphs, int n_pango_glyphs)
{
  int i;

  for (i = 0; i < n_log_glyphs && i < n_pango_glyphs; i++)
    {
      pango_glyphs[i]
        = (font_info->font_set == THAI_FONT_NONE) ?
            libthai_make_unknown_glyph (font_info, log_glyphs[i]) :
            libthai_make_glyph (font_info, log_glyphs[i]);
    }

  return i;
}

static int
make_logical_glyphs (ThaiFontInfo *font_info, struct thcell_t tis_cell,
                     thglyph_t log_glyphs[], size_t n_log_glyphs)
{
  int i;

  switch (font_info->font_set)
    {
      case THAI_FONT_NONE:
      case THAI_FONT_TIS:
      case THAI_FONT_ISO10646:
        return th_render_cell_tis (tis_cell, log_glyphs, n_log_glyphs, TRUE);
  
      case THAI_FONT_TIS_MAC:
        return th_render_cell_mac (tis_cell, log_glyphs, n_log_glyphs, TRUE);
  
      case THAI_FONT_TIS_WIN:
        return th_render_cell_win (tis_cell, log_glyphs, n_log_glyphs, TRUE);
  
      default:
        return 0;
    }
}

void 
libthai_engine_shape (PangoEngineShape *engine,
                      PangoFont        *font,
                      const char       *text,
                      gint              length,
                      PangoAnalysis    *analysis,
                      PangoGlyphString *glyphs)
{
  ThaiFontInfo *font_info;
  thchar_t *tis_text;
  int      tis_length;
  int      i, j, n;

  tis_text = g_convert (text, length, "TIS-620", "UTF-8",
                        NULL, NULL, NULL);
  tis_length = strlen (tis_text);

  pango_glyph_string_set_size (glyphs, 0);

  font_info = libthai_get_font_info (font);

  for (i = 0; i < tis_length; /* nop */)
    {
      int             cell_length;
      thglyph_t       log_glyphs[4];
      struct thcell_t tis_cell;
      PangoGlyph      pango_glyphs[4];
  
      cell_length = th_next_cell (tis_text + i, tis_length - i, &tis_cell, TRUE);
      n = make_logical_glyphs (font_info, tis_cell,
                               log_glyphs, G_N_ELEMENTS (log_glyphs));
      n = make_font_glyphs (font_info, log_glyphs, n,
                            pango_glyphs, G_N_ELEMENTS (pango_glyphs));
      for (j = 0; j < n; j++)
        add_glyph (font_info, glyphs, g_utf8_offset_to_pointer (text, i) - text,
                   pango_glyphs[j], (j != 0));
      i += cell_length;
    }
}

