/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * TreeModel_DnD.hpp
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

#ifndef _TREEMODEL_DND_HPP_
#define _TREEMODEL_DND_HPP_

#include <gtkmm.h>

class TreeModel_DnD : public Gtk::TreeStore {
	protected:
		TreeModel_DnD();

	public:
		class MyModelColumns : public Gtk::TreeModel::ColumnRecord {
			public:
				Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > pixbuf;
				Gtk::TreeModelColumn<Glib::ustring> type;
				Gtk::TreeModelColumn<Glib::ustring> label;
				Gtk::TreeModelColumn<Glib::ustring> cmd;
				Gtk::TreeModelColumn<Glib::ustring> icon;

				MyModelColumns(void) { add(pixbuf), add(type); add(label); add(cmd); add(icon); }
		};

		MyModelColumns Columns;

		static Glib::RefPtr<TreeModel_DnD> create();

	protected:
		virtual bool row_drop_possible_vfunc(const Gtk::TreeModel::Path& dest, const Gtk::SelectionData& selection_data) const;
  
};

#endif /* _TREEMODEL_DND_HPP_ */
