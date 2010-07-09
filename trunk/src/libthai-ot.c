/* Pango
 * libthai-ot.c:
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

#include <string.h>

#include "libthai-ot.h"

static const PangoOTFeatureMap gsub_features[] =
{
  {"ccmp", PANGO_OT_ALL_GLYPHS},
  {"locl", PANGO_OT_ALL_GLYPHS},
  {"liga", PANGO_OT_ALL_GLYPHS},
};

static const PangoOTFeatureMap gpos_features[] =
{
  {"kern", PANGO_OT_ALL_GLYPHS},
  {"mark", PANGO_OT_ALL_GLYPHS},
  {"mkmk", PANGO_OT_ALL_GLYPHS}
};

const PangoOTRuleset *
libthai_ot_get_ruleset (PangoFont     *font,
                        PangoScript    script,
                        PangoLanguage *language)
{
  PangoFcFont    *fc_font;
  FT_Face         face;
  PangoOTInfo    *info;
  const PangoOTRuleset *ruleset = NULL;

  g_return_val_if_fail (font != NULL, NULL);

  fc_font = PANGO_FC_FONT (font);
  face = pango_fc_font_lock_face (fc_font);
  g_assert (face != NULL);

  info = pango_ot_info_get (face);
  if (G_LIKELY (info))
    {
      static GQuark ruleset_quark = 0;

      if (G_UNLIKELY (!ruleset_quark))
        ruleset_quark = g_quark_from_string ("thai-ot-ruleset");

      ruleset = g_object_get_qdata (G_OBJECT (info), ruleset_quark);
      if (G_UNLIKELY (!ruleset))
        {
          PangoOTRulesetDescription desc;

          desc.script = script;
          desc.language = language;

          desc.n_static_gsub_features = G_N_ELEMENTS (gsub_features);
          desc.static_gsub_features = gsub_features;
          desc.n_static_gpos_features = G_N_ELEMENTS (gpos_features);
          desc.static_gpos_features = gpos_features;

          /* TODO populate other_features from analysis->extra_attrs */
          desc.n_other_features = 0;
          desc.other_features = NULL;

          ruleset = pango_ot_ruleset_get_for_description (pango_ot_info_get (face),
                                                          &desc);

          if (ruleset)
            {
              g_object_set_qdata_full (G_OBJECT (info), ruleset_quark, ruleset,
                                       (GDestroyNotify)g_object_unref);
            }
          else
            {
              g_object_unref (ruleset);
              ruleset = NULL;
            }
        }
    }

  pango_fc_font_unlock_face (fc_font);

  return ruleset;
}


void
libthai_ot_shape (PangoFont           *font,
                  const PangoAnalysis *analysis,
                  PangoGlyphString    *glyphs)
{
  const PangoOTRuleset *ot_ruleset;

  g_return_if_fail (font != NULL);
  g_return_if_fail (glyphs != NULL);

  ot_ruleset = libthai_ot_get_ruleset (font, analysis->script,
                                       analysis->language);

  if (ot_ruleset != NULL)
    {
      gint i;
      PangoOTBuffer *buffer;

      /* prepare ot buffer */
      buffer = pango_ot_buffer_new (PANGO_FC_FONT (font));
      pango_ot_buffer_set_rtl (buffer, analysis->level % 2 != 0);
      for (i = 0; i < glyphs->num_glyphs; i++)
        {
          pango_ot_buffer_add_glyph (buffer,
				     glyphs->glyphs[i].glyph,
				     0,
				     glyphs->log_clusters[i]);
        }

      pango_ot_ruleset_substitute (ot_ruleset, buffer);
      pango_ot_ruleset_position (ot_ruleset, buffer);

      pango_ot_buffer_output (buffer, glyphs);
      pango_ot_buffer_destroy (buffer);
    }
}

