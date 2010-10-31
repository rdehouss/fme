/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * ListModelPathsIcons.cc
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

#include <ListModelPathsIcons.hpp>

ListModelPathsIcons::ListModelPathsIcons() {
	/*
	* We can't just call Gtk::TreeModel(Columns) in the initializer list
	* because Columns does not exist when the base class constructor runs.
	* And we can't have a static Columns instance, because that would be
	* instantiated before the gtkmm type system.
	* So, we use this method, which should only be used just after creation:
	*/
	set_column_types(Columns);
}

Glib::RefPtr<ListModelPathsIcons> ListModelPathsIcons::create() {
	return Glib::RefPtr<ListModelPathsIcons>(new ListModelPathsIcons());
}
