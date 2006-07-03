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

