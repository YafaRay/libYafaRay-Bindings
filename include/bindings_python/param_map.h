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

#ifndef LIBYAFARAY_BINDINGS_PARAM_MAP_H
#define LIBYAFARAY_BINDINGS_PARAM_MAP_H

#include "common.h"

typedef struct yafaray_ParamMap yafaray_ParamMap;

namespace yafaray_bindings::python
{
class ParamMap final // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	public:
		[[nodiscard]] yafaray_ParamMap *get() { return param_map_; }
		[[nodiscard]] const yafaray_ParamMap *get() const { return param_map_; }
		[[nodiscard]] static PyTypeObject *getType() { return &py_type_; }

	private:
		[[nodiscard]] static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds);
		[[nodiscard]] static PyObject *destroy(ParamMap *self);
		[[nodiscard]] static PyObject *setVector(ParamMap *self, PyObject *args);
		[[nodiscard]] static PyObject *setString(ParamMap *self, PyObject *args);
		[[nodiscard]] static PyObject *setBool(ParamMap *self, PyObject *args);
		[[nodiscard]] static PyObject *setInt(ParamMap *self, PyObject *args);
		[[nodiscard]] static PyObject *setFloat(ParamMap *self, PyObject *args);
		[[nodiscard]] static PyObject *setColor(ParamMap *self, PyObject *args);
		[[nodiscard]] static PyObject *setMatrix(ParamMap *self, PyObject *args);
		[[nodiscard]] static PyObject *clear(ParamMap *self, PyObject *);
		[[nodiscard]] static PyObject *setInputColorSpace(ParamMap *self, PyObject *args);

		PyObject_HEAD
		yafaray_ParamMap *param_map_ = nullptr;
		static PyMethodDef py_methods_[];
		static PyTypeObject py_type_;
};


} // namespace yafaray_bindings::python

#endif //LIBYAFARAY_BINDINGS_PARAM_MAP_H
