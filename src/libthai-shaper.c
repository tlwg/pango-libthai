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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
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

  if (combining)
    {
      glyphs->glyphs[index].geometry.x_offset =
	glyphs->glyphs[index - 1].geometry.width;
      glyphs->glyphs[index].geometry.width =
	logical_rect.width + glyphs->glyphs[index - 1].geometry.width;
      glyphs->glyphs[index - 1].geometry.width = 0;
    }
  else
    {
      if (logical_rect.width > 0)
        {
	 glyphs->glyphs[index].geometry.x_offset = 0;
	 glyphs->glyphs[index].geometry.width = logical_rect.width;
	}
      else
        {
	 glyphs->glyphs[index].geometry.x_offset = ink_rect.width;
	 glyphs->glyphs[index].geometry.width = ink_rect.width;
        }
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
            (*font_info->make_unknown_glyph) (font_info, log_glyphs[i]) :
            (*font_info->make_glyph) (font_info, log_glyphs[i]);
    }

  return i;
}

/* Map from code point to group used for rendering with XTIS fonts
 * (0 == base character)
 */
static const char xtis_groups[32] = {
  0, 1, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 2,
  2, 2, 2, 2, 2, 2, 1, 0
};

/* Map from code point to index within group 1
 * (0 == no combining mark from group 1)
 */   
static const char xtis_group1_map[32] = {
  0, 1, 0, 0, 2, 3, 4, 5,
  6, 7, 8, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

/* Map from code point to index within group 2
 * (0 == no combining mark from group 2)
 */   
static const char xtis_group2_map[32] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1,
  2, 3, 4, 5, 6, 7, 1, 0
};

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
  
      case THAI_FONT_XTIS:
        /* If we are rendering with an XTIS font, we try to find a precomposed
         * glyph for the cluster.
         */
	if (tis_cell.base < 0x80)
          {
            log_glyphs[0] = tis_cell.base;
            return 1;
          }
	else
          {
            int        xtis_index;
            int        i;
            PangoGlyph glyph;

            xtis_index = 0x100 * (tis_cell.base - 0x80) + 0x30;
            if (tis_cell.hilo)
              xtis_index += 8 * xtis_group1_map[tis_cell.hilo - 0xd0];
            if (tis_cell.top)
              xtis_index += xtis_group2_map[tis_cell.top - 0xd0];
            glyph = (*font_info->make_glyph) (font_info, xtis_index);
            if (pango_x_has_glyph (font_info->font, glyph))
              {
                log_glyphs[0] = xtis_index;
                return 1;
              }
            i = 0;
            log_glyphs[i++] = 0x100 * (tis_cell.base - 0x80) + 0x30;
            if (tis_cell.hilo)
              log_glyphs[i++] = 0x100 * (tis_cell.hilo - 0x80) + 0x30;
            if (tis_cell.top)
              log_glyphs[i++] = 0x100 * (tis_cell.top - 0x80) + 0x30;
            return i;
          }
  
      default:
        return 0;
    }
}

void 
thai_engine_shape (ThaiFontInfo     *font_info,
		   const char       *text,
		   gint              length,
		   PangoAnalysis    *analysis,
		   PangoGlyphString *glyphs)
{
  thchar_t *tis_text;
  int      tis_length;
  int      i, j, n;

  tis_text = g_convert (text, length, "TIS-620", "UTF-8",
                        NULL, NULL, NULL);
  tis_length = strlen (tis_text);

  pango_glyph_string_set_size (glyphs, 0);

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

