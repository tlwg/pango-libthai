/* Pango
 * libthai-shaper.h:
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
 * Copyright (C) 2003-2010 Theppitak Karoonboonyanan
 * Author: Theppitak Karoonboonyanan <thep@linux.thai.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */


#ifndef __THAI_SHAPER_H__
#define __THAI_SHAPER_H__

#define ucs2tis(wc)     (unsigned int)((unsigned int)(wc) - 0x0E00 + 0xA0)
#define tis2uni(c)      ((gunichar)(c) - 0xA0 + 0x0E00)

typedef struct _ThaiFontInfo ThaiFontInfo;

/* Font encodings we will use
 */
typedef enum {
  THAI_FONT_NONE,
  THAI_FONT_TIS,
  THAI_FONT_TIS_MAC,
  THAI_FONT_TIS_WIN
} ThaiFontSet;

typedef enum {
  THAI_FONTINFO_X,
  THAI_FONTINFO_XFT
} ThaiFontInfoType;

struct _ThaiFontInfo
{
  PangoFont       *font;
  ThaiFontSet      font_set;
};

/*
 * Abstract methods (implemented by each shaper module)
 */
ThaiFontInfo *
libthai_get_font_info (PangoFont *font);

PangoGlyph
libthai_get_glyph_tis (ThaiFontInfo *font_info, guchar c);

PangoGlyph
libthai_make_glyph_tis (ThaiFontInfo *font_info, guchar c);

PangoGlyph
libthai_get_glyph_uni (ThaiFontInfo *font_info, gunichar uc);

PangoGlyph
libthai_make_glyph_uni (ThaiFontInfo *font_info, gunichar uc);

PangoGlyph
libthai_make_unknown_glyph (ThaiFontInfo *font_info, gunichar c);

/*
 * Public functions
 */
void
libthai_engine_shape (PangoEngineShape    *engine,
                      PangoFont           *font,
                      const char          *text,
                      int                  length,
                      const PangoAnalysis *analysis,
                      PangoGlyphString    *glyphs);

#endif /* __THAI_SHAPER_H__ */

