/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * Action.cc
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

#include <Action.hpp>

void Action::setMenuLineOld(const MenuLine& _lineOld) {
	lineOld.type = _lineOld.type;
	lineOld.label = _lineOld.label;
	lineOld.cmd = _lineOld.cmd;
	lineOld.icon = _lineOld.icon;
}

void Action::setMenuLineNew(const MenuLine& _lineNew) {
	lineNew.type = _lineNew.type;
	lineNew.label = _lineNew.label;
	lineNew.cmd = _lineNew.cmd;
	lineNew.icon = _lineNew.icon;
}

void Action::clean(void) {
	type = NO_TYPE;
	titleOld.clear();
	titleNew.clear();
	pathSrc.clear();
	pathDst.clear();
	numberChildren = 0;
	lineOld.clear();
	lineNew.clear();
}
