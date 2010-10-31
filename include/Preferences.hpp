/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * Preferences.hpp
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

#ifndef _PREFERENCES_HPP_
#define _PREFERENCES_HPP_

#ifdef HAVE_CONFIG_H
#include <config.hpp>
#endif /* HAVE_CONFIG_H */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <glibmm.h>
#ifdef HAVE_LIBBOOST_REGEX
#include <boost/regex.hpp>
#endif /* HAVE_LIBBOOST_REGEX */

#include <Manip.hpp>

class Preferences {
	private:
		bool confirmForDelete;
		std::vector<Glib::ustring> paths_icons;
	public:
		Preferences(void);
		~Preferences(void);

		bool getConfirmForDelete(void) const { return confirmForDelete; }
		std::vector<Glib::ustring>& getPathsIcons(void) { return paths_icons; }

		void setConfirmForDelete(const bool _confirmForDelete) { confirmForDelete = _confirmForDelete; }
		void setPathsIcons(const std::vector<Glib::ustring> _paths_icons) { paths_icons = _paths_icons; }
};

#endif /* _PREFERENCES_HPP_ */
