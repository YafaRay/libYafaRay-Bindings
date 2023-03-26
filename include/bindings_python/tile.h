#pragma once
/****************************************************************************
 *
 *      This is part of the libYafaRay-Bindings package
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2.1 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library; if not, write to the Free Software
 *      Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef LIBYAFARAY_BINDINGS_TILE_H
#define LIBYAFARAY_BINDINGS_TILE_H

#include "common.h"
#include "common/image.h"

namespace yafaray_bindings::python
{
class Tile final // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	public:
		enum class CornerPosition : int { TopLeft, TopRight, BottomLeft, BottomRight };
		[[nodiscard]] static PyTypeObject *getType() { return &py_type_; }
		void setAreaX0(int area_x_0) { area_x_0_ = area_x_0; }
		void setAreaX1(int area_x_1) { area_x_1_ = area_x_1; }
		void setAreaY0(int area_y_0) { area_y_0_ = area_y_0; }
		void setAreaY1(int area_y_1) { area_y_1_ = area_y_1; }
		void setImage(Image *image) { image_ = image; }
		void setExportedChannels(int exported_channels) { exported_channels_ = exported_channels; }
		void drawCorner(int x, int y, int border_length, CornerPosition corner_position);

	private:
		static Py_ssize_t size(const Tile *tile);
		static PyObject *subscriptInt(const Tile *tile, int py_index);
		static void destroy(Tile *tile);

		PyObject_HEAD //Important! Python automatically reads/writes this head by position within the struct (not by name!), the class *must never* have anything located before the head or it will be overwritten in a very messy way!
		//The Python Object header needs to know the size of the object, better to use simple raw pointers to contained objects to get a reliable size. Do not use smart pointers or classes here or weird random crashes will happen!
		//Important: the PyObject_New function does *not* initialize the structure with its default values, an explicit init function needed to initialize and allocate any pointer(s)
		int area_x_0_ = 0;
		int area_x_1_ = 0;
		int area_y_0_ = 0;
		int area_y_1_ = 0;
		Image *image_ = nullptr;
		int exported_channels_ = 0;
		static PySequenceMethods sequence_methods_;
		static PyTypeObject py_type_;
};

} // namespace yafaray_bindings::python

#endif //LIBYAFARAY_BINDINGS_TILE_H
