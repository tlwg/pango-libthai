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


#include <glib.h>
#include <pango/pango-engine.h>
#include <pango/pango-break.h>
#include <thai/thctype.h>
#include <thai/thbrk.h>


#define SCRIPT_ENGINE_NAME "LibThaiScriptEngineLang"

/* We handle the range U+0e01 to U+0e5b exactly
 */
static PangoEngineRange thai_ranges[] = {
  { 0x0e01, 0x0e5b, "*" },  /* Thai */
};

static PangoEngineInfo script_engines[] = {
  {
    SCRIPT_ENGINE_NAME,
    PANGO_ENGINE_TYPE_LANG,
    PANGO_RENDER_TYPE_NONE,
    thai_ranges, G_N_ELEMENTS(thai_ranges)
  }
};

static void
thai_engine_break (const char    *text,
                   int            len,
                   PangoAnalysis *analysis,
                   PangoLogAttr  *attrs,
                   int            attrs_len)
{
  thchar_t *tis_text;

  tis_text = g_convert (text, len, "TIS-620", "UTF-8",
                        NULL, NULL, NULL);
  if (tis_text)
    {
      int tis_len = strlen (tis_text);
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

static PangoEngine *
thai_engine_lang_new ()
{
  PangoEngineLang *result;
  
  result = g_new (PangoEngineLang, 1);

  result->engine.id = SCRIPT_ENGINE_NAME;
  result->engine.type = PANGO_ENGINE_TYPE_LANG;
  result->engine.length = sizeof (result);
  result->script_break = thai_engine_break;

  return (PangoEngine *)result;
}

/* The following three functions provide the public module API for
 * Pango. If we are compiling it is a module, then we name the
 * entry points script_engine_list, etc. But if we are compiling
 * it for inclusion directly in Pango, then we need them to
 * to have distinct names for this module, so we prepend
 * _pango_thai_lang_
 */
#ifdef LANG_MODULE_PREFIX
#define MODULE_ENTRY(func) _pango_thai_lang_##func
#else
#define MODULE_ENTRY(func) func
#endif

/* List the engines contained within this module
 */
void 
MODULE_ENTRY(script_engine_list) (PangoEngineInfo **engines, gint *n_engines)
{
  *engines = script_engines;
  *n_engines = G_N_ELEMENTS (script_engines);
}

/* Load a particular engine given the ID for the engine
 */
PangoEngine *
MODULE_ENTRY(script_engine_load) (const char *id)
{
  if (!strcmp (id, SCRIPT_ENGINE_NAME))
    return thai_engine_lang_new ();
  else
    return NULL;
}

void 
MODULE_ENTRY(script_engine_unload) (PangoEngine *engine)
{
}

