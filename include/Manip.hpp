/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * Manip.hpp
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

#ifndef _MANIP_HPP_
#define _MANIP_HPP_

#include <iostream>

template <class T>
class objetManip {
	private:
		T _val;
		std::ostream& (*_manipFunc)(std::ostream&, T);

	public:
		objetManip(std::ostream& (*pFunc)(std::ostream &, T), T val) : _manipFunc(pFunc), _val(val) {}

		void operator()(std::ostream &s) const { _manipFunc(s, _val); }
};

template <class T>
std::ostream &operator<<(std::ostream &s, const objetManip<T> &manip) {
	manip(s);
	return s;
}

objetManip<int> tab(int n);

std::ostream& tab(std::ostream& s);

#endif /* _MANIP_HPP_ */
