/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * TreeModel_DnD.cc
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

#include <TreeModel_DnD.hpp>

TreeModel_DnD::TreeModel_DnD() {
	/*
	* We can't just call Gtk::TreeModel(Columns) in the initializer list
	* because Columns does not exist when the base class constructor runs.
	* And we can't have a static Columns instance, because that would be
	* instantiated before the gtkmm type system.
	* So, we use this method, which should only be used just after creation:
	*/
	set_column_types(Columns);
}

Glib::RefPtr<TreeModel_DnD> TreeModel_DnD::create() {
	return Glib::RefPtr<TreeModel_DnD>(new TreeModel_DnD());
}

bool TreeModel_DnD::row_drop_possible_vfunc(const Gtk::TreeModel::Path& dest, const Gtk::SelectionData& selection_data) const {
	/*
	 * dest is the path that the row would have after it has been dropped:
	 * But in this case we are more interested in the parent row:
	 */
	Gtk::TreeModel::Path dest_parent = dest;
	bool dest_is_not_top_level = dest_parent.up();
	if( !dest_is_not_top_level || dest_parent.empty() ) {
		// root
		return true;
	}
	else {
		TreeModel_DnD* unconstThis = const_cast<TreeModel_DnD*>(this);
		const_iterator iter_dest_parent = unconstThis->get_iter(dest_parent);

		if( iter_dest_parent ) {
			Row row = *iter_dest_parent;
			if ( row[Columns.type]=="submenu" ) {
				return true;
			}
			else {
				return false;
			}
		}
	}

	return Gtk::TreeStore::row_drop_possible_vfunc(dest, selection_data);
}
