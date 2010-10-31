/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * main.cc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Raphael Dehousse
 */

#ifdef HAVE_CONFIG_H
#include <config.hpp>
#endif /* HAVE_CONFIG_H */

#include <iostream>
#include <cstring>
#include <libglademm.h>

#include <ui.hpp>

#ifdef ENABLE_NLS
#include <slice_i18n.hpp>
#endif /* ENABLE_NLS */

int main(int argc, char *argv[]) {
#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif /* ENABLE_NLS */

	bool debug = true;
	std::ofstream fd;
	std::streambuf *savelog;

	if ( argc>1 && ( !strcmp(argv[1], "--version") || !strcmp(argv[1], "-V" ) ) ) {
		std::cout << PACKAGE_STRING << std::endl;
		exit(EXIT_SUCCESS);
	}

	if ( argc==1 || strcmp(argv[1], "--debug") ) {
		fd.open("/dev/null");
		savelog = std::clog.rdbuf();
		std::clog.rdbuf(fd.rdbuf());
		debug = false;
	}
	else {
		if ( !strcmp(argv[1], "--debug") ) {
			fd.open((Glib::get_home_dir() + "/fmedebug.log").c_str());
			savelog = std::clog.rdbuf();
			std::clog.rdbuf(fd.rdbuf());
			debug = true;
		}
	}
	
	std::clog << PACKAGE_STRING << std::endl;
	std::clog << "Debug mode enabled" << std::endl;

	Gtk::Main kit(argc, argv);

	Glib::RefPtr<Gnome::Glade::Xml> xml;

	try {
		xml = Gnome::Glade::Xml::create(GLADE_DIR"/fme.glade");
	} catch ( Gnome::Glade::XmlError& e ) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	ui *MainWindow;
	xml->get_widget_derived("MainWindow", MainWindow);

	if ( !MainWindow ) {
		std::cerr << "Error while loading the main window" << std::endl;
		return EXIT_FAILURE;
	}

	Gtk::Main::run(*MainWindow);

	delete MainWindow;

	fd.close();
	std::clog.rdbuf(savelog);

	return EXIT_SUCCESS;
}
