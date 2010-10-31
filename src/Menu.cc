/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * Menu.cc
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

#include <Menu.hpp>

void copy(const char* dst, const char* src) throw(FileNotExist, CannotOpenFile) {
	std::ifstream fileSrc(src);

	if ( fileSrc.is_open() ) {
		std::ofstream fileDst(dst);
		if ( fileDst.is_open() ) {
			std::string str;
			while ( fileSrc.good() ) {
				getline(fileSrc, str);
				if ( fileSrc.good() )
					fileDst << str << std::endl;
			}
		}
		else {
			throw CannotOpenFile();
		}
	}
	else {
		throw FileNotExist();
	}
}

std::ostream& operator<<(std::ostream& s, const MenuLine& l) {
	std::ostringstream oStr;

	oStr << "[" << l.type << "]";
	if ( !(l.label).empty() )
		oStr << " (" << l.label << ")";
	if ( !(l.cmd).empty() )
		oStr << " {" << l.cmd << "}";
	if ( !(l.icon).empty() )
		oStr << " <" << l.icon << ">";

	try {
		if ( l.encoding.empty() || l.encoding=="ISO8859-1" )
			return s << Glib::convert(oStr.str(), "ISO-8859-1", "UTF-8");
		else {
			return s << oStr.str();
		}
	} catch ( Glib::ConvertError ) {
		std::clog << " !! ConvertError" << std::endl;
		return s << oStr.str();
	}
}

bool operator==(const MenuLine& l1, const MenuLine& l2) {
	if ( l1.type!=l2.type ) return false;
	if ( l1.label!=l2.label ) return false;
	if ( l1.cmd!=l2.cmd ) return false;
	if ( l1.icon!=l2.icon ) return false;

	return true;
}

bool operator!=(const MenuLine& l1, const MenuLine& l2) {
	if ( l1.type==l2.type )
		if ( l1.label==l2.label )
			if ( l1.cmd==l2.cmd )
				if ( l1.icon==l2.icon ) return false;

	return true;
}

Menu::Menu(void) throw ( EnvPathNotDefined, FluxboxNotFound ) {
	std::clog << " * Menu constructor" << std::endl;
	std::ostringstream strOut;

	_eof = false;

	strOut << Glib::get_home_dir() << "/.fluxbox";

	std::clog << tab << "Fluxbox dir : " << strOut.str() << std::endl;

	if ( strOut.str()=="/.fluxbox" ) throw EnvPathNotDefined();

	std::ifstream fileinit((strOut.str() + "/init").c_str());

	if ( !fileinit.is_open() ) throw FluxboxNotFound();

	std::string str;
#ifdef GLIBMM_REGEX
	Glib::RefPtr<Glib::Regex> regex = Glib::Regex::create("session\\.menuFile:[[:space:]]*(.*)[[:space:]]*", Glib::REGEX_CASELESS);

	while ( getline(fileinit, str) && !regex->match(str) );
	if ( fileinit.good() ) {
		Glib::StringArrayHandle menufile = regex->split(str);
		Glib::Container_Helpers::ArrayHandleIterator<Glib::Container_Helpers::TypeTraits<Glib::ustring> > iter=menufile.begin();
		std::clog << tab << "Menu file found in init : " << *(++iter) << std::endl;
		strOut.str("");
		int index;
		std::string filename = *iter;
#else
	boost::cmatch what;
	boost::regex expr("session\\.menuFile:[[:space:]]*(.*)[[:space:]]*", boost::regex_constants::icase);

	while ( getline(fileinit, str) && !regex_search(str.c_str(), what, expr) );
	if ( fileinit.good() ) {
		std::clog << tab << "Menu file found in init : " << what[1] << std::endl;
		strOut.str("");
		int index;
		std::string filename = what[1];
#endif
		if ( (index = filename.find_first_of("~") )!=std::string::npos && index==0 ) {
			// if filename contains ~ and is the first character
			filename.replace(index, index+1, Glib::get_home_dir());
			std::clog << tab << " => : " << filename << std::endl;
		}
		strOut << filename;
	}
	else {
		strOut << "/menu";
		std::clog << tab << "Menu file not found in init : " << strOut.str() << std::endl;
	}

	fileinit.close();

	_filename = strOut.str();
}

const bool Menu::is_utf8(void) const {
	return ( _encoding=="UTF-8" );
}

void Menu::setEncoding(const Glib::ustring encoding) {
	if ( _encoding!=encoding ) {
		std::clog << " !! encoding changed to \"" << encoding << "\" from \"" << _encoding << "\"" << std::endl;
		_encoding = encoding;
	}
}

void Menu::open(const Glib::ustring filename) throw ( CannotOpenFile ) {
	if ( filename.empty() ) {
		_menu.open(getFilename_c());
	} else {
		_menu.open(filename.c_str());
	}

	if ( !_menu.is_open() ) {
		throw CannotOpenFile();
	}
}

MenuLine Menu::getLine() {
	MenuLine line;

#ifdef GLIBMM_REGEX
	Glib::RefPtr<Glib::Regex> regex = Glib::Regex::create(
#else
	boost::regex expr(
#endif /* GLIBMM_REGEX */
		"\\[(.*?)\\]"
		"[[:space:]]*(?:\\((.*?)(?<!\\\\)\\))?"
		"[[:space:]]*(?:\\{(.*?)(?<!\\\\)\\})?"
		"[[:space:]]*(?:<((.*?)\\.(?:xpm|png))?>)?"
#ifdef GLIBMM_REGEX
		, Glib::REGEX_CASELESS);
#else
		, boost::regex_constants::icase);
#endif /* GLIBMM_REGEX */

	while ( !(_menu.eof()) && line.type=="" ) {
		std::string str;
#ifndef GLIBMM_REGEX
		boost::cmatch part;
#endif /* GLIBMM_REGEX */
		while ( std::getline(_menu, str) && str.empty() );

		if ( !str.empty() ) {
			Glib::ustring ustr;
			try {
				if ( _encoding.empty() || _encoding=="ISO-8859-15" )
					ustr = Glib::convert(str, "UTF-8", "ISO-8859-15");
				else {
					ustr = str;
				}
			} catch ( Glib::ConvertError ) {
				std::clog << " !! ConvertError" << std::endl;
				ustr = str;
			}

#ifdef GLIBMM_REGEX
			Glib::StringArrayHandle part = regex->split(ustr);
			Glib::Container_Helpers::ArrayHandleIterator<Glib::Container_Helpers::TypeTraits<Glib::ustring> > iter=part.begin();
			if ( iter!=part.end() ) line.type = *(++iter);
			if ( iter!=part.end() ) line.label = *(++iter);
			if ( iter!=part.end() ) line.cmd = *(++iter);
			if ( iter!=part.end() ) line.icon = *(++iter);
#else
			if ( boost::regex_search(ustr.c_str(), part, expr) ) {
				line.type = part[1];
				line.label = part[2];
				line.cmd = part[3];
				line.icon = part[4];
			}
			else {
				line.type = "";
				line.label = "";
				line.cmd = "";
				line.icon = "";
			}
#endif /* GLIBMM_REGEX */
		}
	}
	if ( _menu.eof() ) {
		setEOF();
	}

	return line;
}

void Menu::close() throw ( CannotCloseFile ) {
	_menu.close();

	if ( _menu.is_open() ) {
		throw CannotCloseFile();
	}
}
