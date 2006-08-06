/* Pango
 * thai-lang.c:
 *
 * Copyright (C) 2003 Theppitak Karoonboonyanan <thep@linux.thai.net>
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
#include <pango/pango-break.h>
#include <thai/thctype.h>
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

static void
libthai_engine_break (PangoEngineLang *engine,
                      const char      *text,
                      int              len,
                      PangoAnalysis   *analysis,
                      PangoLogAttr    *attrs,
                      int              attrs_len)
{
  thchar_t *tis_text;

  tis_text = (thchar_t *) g_convert (text, len, "TIS-620", "UTF-8",
                                     NULL, NULL, NULL);
  if (tis_text)
    {
      int tis_len = strlen ((const char*)tis_text);
      int *brk_pnts = g_new (int, tis_len);
      int brk_n;
      int i;

      /* set & initialize general attribs */
      for (i = 0; i < attrs_len; i++)
        {
          attrs[i].is_line_break = FALSE;
          attrs[i].is_mandatory_break = FALSE;
          attrs[i].is_cursor_position = th_iscombchar (tis_text[i]) ? FALSE : TRUE;
          attrs[i].is_char_break = attrs[i].is_cursor_position;
          attrs[i].is_white = isspace (tis_text[i]) ? TRUE : FALSE;

          attrs[i].is_word_start = FALSE;
          attrs[i].is_word_end = FALSE;

          attrs[i].is_sentence_boundary = FALSE;
          attrs[i].is_sentence_start = FALSE;
          attrs[i].is_sentence_end = FALSE;

          attrs[i].backspace_deletes_character = attrs[i].is_cursor_position;
        }

      /* find line break positions */
      brk_n = th_brk (tis_text, brk_pnts, tis_len);
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

