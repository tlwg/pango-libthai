/* Pango
 * libthai-ot.h:
 *
 * Copyright (C) 2004-2010 Theppitak Karoonboonyanan
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

#ifndef __LIBTHAI_OT_H__
#define __LIBTHAI_OT_H__

#include <pango/pango-ot.h>

const PangoOTRuleset *
libthai_ot_get_ruleset (PangoFont     *font,
                        PangoScript    script,
                        PangoLanguage *language);

void
libthai_ot_shape (PangoFont           *font,
                  const PangoAnalysis *analysis,
                  PangoGlyphString    *glyphs);

#endif /* __LIBTHAI_OT_H__ */

