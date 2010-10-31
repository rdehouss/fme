/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * ListModelPathsIcons.hpp
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

#ifndef _LISTMODELMODELSPATHSICONS_HPP_
#define _LISTMODELMODELSPATHSICONS_HPP_

#include <gtkmm.h>

class ListModelPathsIcons : public Gtk::ListStore {
	protected:
		ListModelPathsIcons();

	public:
		class MyModelColumns : public Gtk::TreeModel::ColumnRecord {
			public:
				Gtk::TreeModelColumn<Glib::ustring> path;

				MyModelColumns(void) { add(path); }
		};

		MyModelColumns Columns;

		static Glib::RefPtr<ListModelPathsIcons> create();
};

#endif /* _LISTMODELMODELSPATHSICONS_HPP_ */
