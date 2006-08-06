## Process this file with automake to create Makefile.in.

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

thai_fc_sources = libthai-shaper.c libthai-shaper.h libthai-fc.c \
                  libthai-ot.c libthai-ot.h
thai_lang_sources = libthai-lang.c 

moduledir = $(libdir)/pango/$(PANGO_MODULE_VERSION)/modules

pangolibs = $(PANGO_LIBS) $(LIBTHAI_LIBS)

if ENABLE_SHAPING
PANGO_FC_INCLUDES = $(PANGOFT2_CFLAGS) $(FREETYPE_CFLAGS)
PANGO_FC_MODULES = pango-libthai-fc.la
pangoft2libs = $(PANGOFT2_LIBS) $(FREETYPE_LIBS) $(pangolibs)
else
PANGO_FC_INCLUDES =
PANGO_FC_MODULES =
pangoft2libs =
endif

INCLUDES =			\
	-DPANGO_ENABLE_ENGINE	\
	-DG_DISABLE_DEPRECATED	\
	$(PANGO_CFLAGS)		\
	$(LIBTHAI_CFLAGS)	\
	$(PANGO_FC_INCLUDES)

module_LTLIBRARIES = $(PANGO_FC_MODULES) pango-libthai-lang.la

pango_libthai_fc_la_LDFLAGS = -export-dynamic -avoid-version -module $(no_undefined)
pango_libthai_fc_la_LIBADD = $(pangoft2libs)
pango_libthai_fc_la_SOURCES = $(thai_fc_sources)

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

