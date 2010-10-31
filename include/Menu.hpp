/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * Menu.hpp
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

#ifndef _MENU_HPP_
#define _MENU_HPP_

#ifdef HAVE_CONFIG_H
#include <config.hpp>
#endif /* HAVE_CONFIG_H */ 

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <glibmm.h>
#ifdef HAVE_LIBBOOST_REGEX
#include <boost/regex.hpp>
#endif /* HAVE_LIBBOOST_REGEX */

#include <Manip.hpp>

typedef struct {
	Glib::ustring encoding;
	Glib::ustring type;
	Glib::ustring label;
	Glib::ustring cmd;
	Glib::ustring icon;
	void clear(void) { type = label = cmd = icon = ""; }
} MenuLine;

std::ostream& operator<<(std::ostream& s, const MenuLine& l);
bool operator==(const MenuLine& line1, const MenuLine& line2);
bool operator!=(const MenuLine& line1, const MenuLine& line2);

class FileNotExist {};
class CannotOpenFile {};
class CannotCloseFile {};

void copy(const char* dst, const char* src) throw(FileNotExist, CannotOpenFile);

class EnvPathNotDefined {};
class FluxboxNotFound {};

class Menu {
	private:
		Glib::ustring _filename;
		Glib::ustring _encoding;
		std::ifstream _menu;
		bool _eof;

		void setEOF() { _eof = true; }

	public:
		Menu(void) throw ( EnvPathNotDefined, FluxboxNotFound );
		~Menu(void) {}

		const char *getFilename_c(void) const { return _filename.c_str(); }
		const Glib::ustring getFilename(void) const { return _filename; }
		const bool is_utf8(void) const;
		const Glib::ustring getEncoding(void) const { return _encoding; }
		const bool eof(void) const { return _eof; }

		void setEncoding(const Glib::ustring encoding);

		void open(const Glib::ustring filename="") throw ( CannotOpenFile );
		MenuLine getLine();
		void close() throw ( CannotCloseFile );
};

#endif /* _MENU_HPP_ */
