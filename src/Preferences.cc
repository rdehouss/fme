/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * Preferences.cc
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

#include <Preferences.hpp>

Preferences::Preferences(void) {
	std::clog << " * Preferences constructor" << std::endl;

	std::ifstream fmeconfig((Glib::get_home_dir() + "/.fluxbox/fmeconfig").c_str());

	if ( fmeconfig.is_open() ) {
		/* Read it */
		std::clog << tab << "Read fmeconfig" << std::endl;
#ifdef GLIBMM_REGEX
		Glib::RefPtr<Glib::Regex> regex = Glib::Regex::create(
#else
		boost::regex expr(
#endif /* GLIBMM_REGEX */
			"\\[(.*?)\\](.*?)\\[/\\1\\]"
#ifdef GLIBMM_REGEX
			, Glib::REGEX_CASELESS);
#else
			, boost::regex_constants::icase);
#endif /* GLIBMM_REGEX */
		std::string str;
		while ( fmeconfig.good() ) {
			std::getline(fmeconfig, str);
#ifdef GLIBMM_REGEX
			Glib::StringArrayHandle what = regex->split(str);
			Glib::Container_Helpers::ArrayHandleIterator<Glib::Container_Helpers::TypeTraits<Glib::ustring> > iter=what.begin();
			if ( iter!=what.end() ) {
				++iter;
				if ( !(*iter).compare("confirm_delete") ) {
					std::string boolean = *(++iter);
					confirmForDelete = atoi(boolean.c_str());
				} else if ( !(*iter).compare("path_icons") ) {
					Glib::ustring path_icons = *(++iter);
					paths_icons.push_back(path_icons);
				}
			}
#else
			boost::cmatch what;
			if ( boost::regex_search(str.c_str(), what, expr) ) {
				if ( what[1]=="confirm_delete" ) {
					std::string boolean = what[2];
					confirmForDelete = atoi(boolean.c_str());
				} else if ( what[1]=="path_icons" ) {
					Glib::ustring path_icons = what[2];
					paths_icons.push_back(path_icons);
				}
			}
#endif /* GLIBMM_REGEX */
		}
	}
	else {
		/* File don't exist, create it */
		std::clog << tab << "fmeconfig don't exists, creating..." << std::endl;
		std::ofstream fmeconfig((Glib::get_home_dir() + "/.fluxbox/fmeconfig").c_str());
		
		confirmForDelete = true;

		fmeconfig << "# fmeconfig" << std::endl << std::endl;

		fmeconfig << "[fmeconfig]" << std::endl;
		fmeconfig << tab << "[confirm_delete]" << "1" << "[/confirm_delete]" << std::endl;

		paths_icons.push_back(Glib::get_home_dir() + "/.icons");
		paths_icons.push_back("/usr/share/pixmaps");
		paths_icons.push_back(GLADE_DIR);

		for ( std::vector<Glib::ustring>::iterator iterPath = paths_icons.begin(); iterPath!=paths_icons.end(); iterPath++ ) {
			fmeconfig << tab << "[path_icons]" << *iterPath << "[/path_icons]" << std::endl;
		}

		fmeconfig << "[/fmeconfig]" << std::endl;

		fmeconfig.close();
	}
}

Preferences::~Preferences(void) {
	std::clog << " * Preferences destructor" << std::endl;
	std::ofstream fmeconfig((Glib::get_home_dir() + "/.fluxbox/fmeconfig").c_str());

	fmeconfig << "# fmeconfig" << std::endl << std::endl;
		
	fmeconfig << "[fmeconfig]" << std::endl;

	fmeconfig << tab << "[confirm_delete]" << confirmForDelete << "[/confirm_delete]" << std::endl;

	for ( std::vector<Glib::ustring>::iterator iterPath = paths_icons.begin(); iterPath!=paths_icons.end(); iterPath++ ) {
		fmeconfig << tab << "[path_icons]" << *iterPath << "[/path_icons]" << std::endl;
	}

	fmeconfig << "[/fmeconfig]" << std::endl;

	fmeconfig.close();
}
