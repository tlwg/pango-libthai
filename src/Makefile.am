## Process this file with automake to create Makefile.in.

common_sources = libthai-shaper.c libthai-shaper.h
thai_x_sources = $(common_sources) libthai-x.c 
thai_xft_sources = $(common_sources) libthai-xft.c 
thai_ft2_sources = $(common_sources) libthai-ft2.c 
thai_lang_sources = libthai-lang.c 

pangolibs = $(PANGO_LIBS) $(LIBTHAI_LIBS)
pangoxlibs = $(PANGOX_LIBS) $(X_LIBS) $(LIBTHAI_LIBS)
pangoxftlibs = $(PANGOXFT_LIBS) $(XFT_LIBS) $(LIBTHAI_LIBS)
pangoft2libs = $(PANGOFT2_LIBS) $(FREETYPE_LIBS) $(LIBTHAI_LIBS)

if HAVE_XFT
XFT_MODULES=pango-libthai-xft.la
XFT_INCLUDED=
XFT_PREFIX=
thai_xft_libadd=$(pangoxftlibs)
else
XFT_MODULES=
XFT_INCLUDED=
XFT_PREFIX=
endif

if HAVE_X
X_MODULES=pango-libthai-x.la
X_INCLUDED=
X_PREFIX=
thai_x_libadd=$(pangoxlibs)
else
X_MODULES=
X_INCLUDED=
X_PREFIX=
endif

if HAVE_FREETYPE
FT2_MODULES=pango-libthai-ft2.la
FT2_INCLUDED=
FT2_PREFIX=
thai_ft2_libadd=$(pangoft2libs)
else
FT2_MODULES=
FT2_INCLUDED=
FT2_PREFIX=
endif

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

noinst_LTLIBRARIES = $(X_INCLUDED) $(XFT_INCLUDED) $(FT2_INCLUDED)
module_LTLIBRARIES = $(X_MODULES) $(XFT_MODULES) $(FT2_MODULES) pango-libthai-lang.la
moddefine = $(X_PREFIX) $(XFT_PREFIX) $(FT2_PREFIX)
moduledir = $(libdir)/pango/$(PANGO_MODULE_VERSION)/modules

INCLUDES =			\
	-DPANGO_ENABLE_ENGINE	\
	-DG_DISABLE_DEPRECATED	\
	$(PANGO_CFLAGS)		\
	$(LIBTHAI_CFLAGS)	\
	$(moddefine)		\
	$(GLIB_CFLAGS)		\
	$(XFT_CFLAGS)		\
	$(FREETYPE_CFLAGS)	\
	$(X_CFLAGS)

pango_libthai_x_la_LDFLAGS = -export-dynamic -avoid-version -module
pango_libthai_x_la_LIBADD = $(thai_x_libadd)
pango_libthai_x_la_SOURCES = $(thai_x_sources)

pango_libthai_xft_la_LDFLAGS = -export-dynamic -avoid-version -module
pango_libthai_xft_la_LIBADD = $(thai_xft_libadd)
pango_libthai_xft_la_SOURCES = $(thai_xft_sources)

pango_libthai_ft2_la_LDFLAGS = -export-dynamic -avoid-version -module $(no_undefined)
pango_libthai_ft2_la_LIBADD = $(thai_ft2_libadd)
pango_libthai_ft2_la_SOURCES = $(thai_ft2_sources)

pango_libthai_lang_la_LDFLAGS = -export-dynamic -avoid-version -module $(no_undefined)
pango_libthai_lang_la_LIBADD = $(pangolibs)
pango_libthai_lang_la_SOURCES = $(thai_lang_sources)

install-data-hook:
	@if test -z "$(DESTDIR)" ; then 				\
	  echo $(mkinstalldirs) $(DESTDIR)$(sysconfdir)/pango \&\& 	\
	  pango-querymodules 			\
		\> $(DESTDIR)$(sysconfdir)/pango/pango.modules ;	\
	  $(mkinstalldirs) $(DESTDIR)$(sysconfdir)/pango && 		\
	  pango-querymodules 			\
		> $(DESTDIR)$(sysconfdir)/pango/pango.modules ;		\
	else								\
	  echo "***" ; 							\
	  echo "*** Warning: pango.modules not created" ; 		\
	  echo "***" ; 							\
	  echo "*** Generate this file on the target system" ; 		\
	  echo "*** system using pango-querymodules" ; 			\
	  echo "***" ; 							\
	fi

included-modules: $(noinst_LTLIBRARIES)

.PHONY: included-modules
