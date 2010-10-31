# -*- coding: utf-8 -*-

bin_PROGRAMS = fme
fme_SOURCES = main.cc ui.cc TreeModel_DnD.cc Menu.cc Manip.cc Action.cc Preferences.cc ListModelPathsIcons.cc
INCLUDES = -I$(top_srcdir)/include @GTKMM_CFLAGS@ -DGLADE_DIR=\"$(pkgdatadir)"/glade"\"
LDADD = @GTKMM_LIBS@
CLEANFILES = *~
DISTCLEANFILES = .deps/*.P
