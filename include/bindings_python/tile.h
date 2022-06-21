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

#include "common/yafaray_bindings_common.h"
#include "common/image.h"
#define PY_SSIZE_T_CLEAN
#ifdef _WIN64
#define _hypot hypot
#include <cmath>
#endif
#include <Python.h>

BEGIN_YAFARAY_BINDINGS

struct Tile final
{
	PyObject_HEAD //Important! Python automatically reads/writes this head by position within the struct (not by name!), the class *must never* have anything located before the head or it will be overwritten in a very messy way!
	//The Python Object header needs to know the size of the object, better to use simple raw pointers to contained objects to get a reliable size. Do not use smart pointers or classes here or weird random crashes will happen!
	//Important: the PyObject_New function does *not* initialize the structure with its default values, an explicit init function needed to initialize and allocate any pointer(s)
	int area_x_0_ = 0, area_x_1_ = 0;
	int area_y_0_ = 0, area_y_1_ = 0;
	Image *image_ = nullptr;
	int exported_channels_ = 0;
	void init() { area_x_0_ = 0; area_x_1_ = 0; area_y_0_ = 0; area_y_1_ = 0; image_ = nullptr; }
	enum class CornerPosition : int { TopLeft, TopRight, BottomLeft, BottomRight };
	void drawCorner(int x, int y, int border_length, CornerPosition corner_position);
};

Py_ssize_t pythonTileSize_global(const Tile *tile);
PyObject *pythonTileSubscriptInt_global(const Tile *tile, int py_index);
void pythonTileDelete_global(Tile *tile);

static PySequenceMethods sequence_methods =
{
	( lenfunc ) pythonTileSize_global,
	nullptr,
	nullptr,
	( ssizeargfunc ) pythonTileSubscriptInt_global
};

static PyTypeObject python_tile_type_global = {
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yaf_tile", /* tp_name */
		sizeof(Tile), /* tp_basicsize */
		0, /* tp_itemsize */
		( destructor ) pythonTileDelete_global, /* tp_dealloc */
		0, /* tp_print / tp_vectorcall_offset */
		nullptr, /* getattrfunc tp_getattr; */
		nullptr, /* setattrfunc tp_setattr; */
		nullptr, /* tp_as_async */
		nullptr, /* tp_repr */
		nullptr, /* PyNumberMethods *tp_as_number; */
		&sequence_methods, /* PySequenceMethods *tp_as_sequence; */
		nullptr, /* PyMappingMethods *tp_as_mapping; */
		nullptr, /* hashfunc tp_hash; */
		nullptr, /* ternaryfunc tp_call; */
		nullptr, /* reprfunc tp_str; */
		nullptr, /* getattrofunc tp_getattro; */
		nullptr, /* setattrofunc tp_setattro; */
		nullptr, /* PyBufferProcs *tp_as_buffer; */
		Py_TPFLAGS_DEFAULT, /* long tp_flags; */
};

END_YAFARAY_BINDINGS

#endif //LIBYAFARAY_BINDINGS_TILE_H
