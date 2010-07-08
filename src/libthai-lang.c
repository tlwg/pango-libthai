/* Pango
 * libthai-lang.c:
 *
 * Copyright (C) 2003-2010 Theppitak Karoonboonyanan <thep@linux.thai.net>
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
#include <glib.h>
#include <pango/pango-engine.h>
#include <pango/pango-break.h>
#include <thai/thwchar.h>
#include <thai/thbrk.h>

/* No extra fields needed */
typedef PangoEngineLang      LibThaiEngineLang;
typedef PangoEngineLangClass LibThaiEngineLangClass;

#define SCRIPT_ENGINE_NAME "LibThaiScriptEngineLang"

static PangoEngineScriptInfo thai_scripts[] = {
  { PANGO_SCRIPT_THAI, "*" }
};

static PangoEngineInfo script_engines[] = {
  {
    SCRIPT_ENGINE_NAME,
    PANGO_ENGINE_TYPE_LANG,
    PANGO_RENDER_TYPE_NONE,
    thai_scripts, G_N_ELEMENTS(thai_scripts)
  }
};

static thchar_t *
utf8_to_tis (const char *text, int len)
{
  thchar_t   *tis_text;
  thchar_t   *tis_p;
  const char *text_p;

  if (len < 0)
    len = strlen (text);

  tis_text = g_new (thchar_t, g_utf8_strlen (text, len) + 1);
  if (!tis_text)
    return NULL;

  tis_p = tis_text;
  for (text_p = text; text_p < text + len; text_p = g_utf8_next_char (text_p))
    *tis_p++ = th_uni2tis (g_utf8_get_char (text_p));
  *tis_p = '\0';

  return tis_text;
}

static void
libthai_engine_break (PangoEngineLang *engine,
                      const char      *text,
                      int              len,
                      PangoAnalysis   *analysis,
                      PangoLogAttr    *attrs,
                      int              attrs_len)
{
  thchar_t *tis_text;

  tis_text = utf8_to_tis (text, len);
  if (tis_text)
    {
      int brk_len = strlen ((const char*)tis_text) + 1;
      int *brk_pnts = g_new (int, brk_len);
      int brk_n;
      int i;

      /* find line break positions */
      brk_n = th_brk (tis_text, brk_pnts, brk_len);
      for (i = 0; i < brk_n; i++)
        {
          attrs[brk_pnts[i]].is_line_break = TRUE;
          attrs[brk_pnts[i]].is_word_start = TRUE;
          attrs[brk_pnts[i]].is_word_end = TRUE;
        }

      g_free (brk_pnts);
      g_free (tis_text);
    }
}

static void
libthai_engine_lang_class_init (PangoEngineLangClass *class)
{
  class->script_break = libthai_engine_break;
}

PANGO_ENGINE_LANG_DEFINE_TYPE (LibThaiEngineLang, libthai_engine_lang,
                               libthai_engine_lang_class_init, NULL);

void
PANGO_MODULE_ENTRY(init) (GTypeModule *module)
{
  libthai_engine_lang_register_type (module);
}

void
PANGO_MODULE_ENTRY(exit) (void)
{
}

void 
PANGO_MODULE_ENTRY(list) (PangoEngineInfo **engines, gint *n_engines)
{
  *engines = script_engines;
  *n_engines = G_N_ELEMENTS (script_engines);
}

/* Load a particular engine given the ID for the engine
 */
PangoEngine *
PANGO_MODULE_ENTRY(create) (const char *id)
{
  if (!strcmp (id, SCRIPT_ENGINE_NAME))
    return g_object_new (libthai_engine_lang_type, NULL);
  else
    return NULL;
}

