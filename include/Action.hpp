/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * Action.hpp
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

#ifndef _ACTION_HPP_
#define _ACTION_HPP_

#include <gtkmm.h>

#include <Menu.hpp>

typedef enum {
	NO_TYPE = 0,
	TITLE_CHANGED,
	ROW_MOVED,
	ROW_INSERTED,
	ROW_EDITED,
	ROW_DELETED,
	NEW_MENU
} TYPE_ACTION;

class Action {
	private:
		TYPE_ACTION type;
		Glib::ustring titleOld;
		Glib::ustring titleNew;
		Gtk::TreeModel::Path pathSrc;
		Gtk::TreeModel::Path pathDst;
		int numberChildren;
		MenuLine lineOld;
		MenuLine lineNew;
		std::string fileOld;
		std::string fileNew;

	public:
		Action(void) { clean(); }
		~Action(void) {}

		const TYPE_ACTION getType(void) const { return type; }
		const Glib::ustring& getTitleOld(void) const { return titleOld; }
		const Glib::ustring& getTitleNew(void) const { return titleNew; }
		const Gtk::TreeModel::Path& getPathSrc(void) const { return pathSrc; }
		const Gtk::TreeModel::Path& getPathDst(void) const { return pathDst; }
		const int getNumberChildren(void) const { return numberChildren; }
		const MenuLine& getMenuLineOld(void) const { return lineOld; }
		const MenuLine& getMenuLineNew(void) const { return lineNew; }
		const std::string& getFileOld(void) const { return fileOld; }
		const std::string& getFileNew(void) const { return fileNew; }

		void setType(const TYPE_ACTION _type) { type = _type; }
		void setTitleOld(const Glib::ustring& _titleOld) { titleOld = _titleOld; }
		void setTitleNew(const Glib::ustring& _titleNew) { titleNew = _titleNew; }
		void setPathSrc(const Gtk::TreeModel::Path& _pathSrc) { pathSrc = _pathSrc; }
		void setPathDst(const Gtk::TreeModel::Path& _pathDst) { pathDst = _pathDst; }
		void setNumberChildren(const int _numberChildren) { numberChildren = _numberChildren; }
		void setMenuLineOld(const MenuLine& _lineOld);
		void setMenuLineNew(const MenuLine& _lineNew);
		void setFileOld(const std::string& _fileOld) { fileOld = _fileOld; }
		void setFileNew(const std::string& _fileNew) { fileNew = _fileNew; }

		void clean(void);
};

#endif /* _ACTION_HPP_ */
