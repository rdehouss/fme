/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * iconview.hpp
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

#ifndef _ICONVIEW_HPP_
#define _ICONVIEW_HPP_

#include <gtkmm.h>

class ModelColumnsIcon : public Gtk::TreeModel::ColumnRecord {
	public:
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > pixbuf;
		Gtk::TreeModelColumn<Glib::ustring> path;
		Gtk::TreeModelColumn<Glib::ustring> filename;

		ModelColumnsIcon(void) { add(pixbuf), add(path), add(filename); }
};

static Glib::ustring icon_selected;

#endif /* _ICONVIEW_HPP_ */
