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

#include "bindings_python/param_map.h"
#include <yafaray_c_api.h>

namespace yafaray_bindings::python
{

PyMethodDef ParamMap::py_methods_[]{
		{"create",                reinterpret_cast<PyCFunction>(create),             METH_NOARGS,  ""},
		{"set_vector",            reinterpret_cast<PyCFunction>(setVector),          METH_VARARGS, ""},
		{"set_string",            reinterpret_cast<PyCFunction>(setString),          METH_VARARGS, ""},
		{"set_bool",              reinterpret_cast<PyCFunction>(setBool),            METH_VARARGS, ""},
		{"set_int",               reinterpret_cast<PyCFunction>(setInt),             METH_VARARGS, ""},
		{"set_float",             reinterpret_cast<PyCFunction>(setFloat),           METH_VARARGS, ""},
		{"set_color",             reinterpret_cast<PyCFunction>(setColor),           METH_VARARGS, ""},
		{"set_matrix",            reinterpret_cast<PyCFunction>(setMatrix),          METH_VARARGS, ""},
		{"clear",                 reinterpret_cast<PyCFunction>(clear),              METH_NOARGS,  ""},
		{"set_input_color_space", reinterpret_cast<PyCFunction>(setInputColorSpace), METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject ParamMap::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.ParamMap", /* tp_name */
		sizeof(ParamMap), /* tp_basicsize */
		0, /* tp_itemsize */
		reinterpret_cast<destructor>(destroy), /* tp_dealloc */
		0, /* tp_print / tp_vectorcall_offset */
		nullptr, /* tp_getattr */
		nullptr, /* tp_setattr */
		nullptr, /* tp_as_async */
		nullptr, /* tp_repr */
		nullptr, /* tp_as_number */
		nullptr, /* tp_as_sequence */
		nullptr, /* tp_as_mapping */
		nullptr, /* tp_hash */
		nullptr, /* tp_call */
		nullptr, /* tp_str */
		nullptr, /* tp_getattro */
		nullptr, /* tp_setattro */
		nullptr, /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT, /* tp_flags */
		"libYafaRay ParamMap object", /* tp_doc */
		nullptr, /* tp_traverse */
		nullptr, /* tp_clear */
		nullptr, /* tp_richcompare */
		0, /* tp_weaklistoffset */
		nullptr, /* tp_iter */
		nullptr, /* tp_iternext */
		py_methods_, /* tp_methods */
		nullptr, /* tp_members */
		nullptr, /* tp_getset */
		nullptr, /* tp_base */
		nullptr, /* tp_dict */
		nullptr, /* tp_descr_get */
		nullptr, /* tp_descr_set */
		0, /* tp_dictoffset */
		nullptr, /* tp_init */
		nullptr, /* tp_alloc */
		create, /* tp_new */
};

PyObject *ParamMap::create(PyTypeObject *type, PyObject *, PyObject *)
{
	auto *self = reinterpret_cast<ParamMap *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->param_map_ = yafaray_createParamMap();
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *ParamMap::destroy(ParamMap *self)
{
	yafaray_destroyParamMap(self->param_map_);
	self->param_map_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

PyObject *ParamMap::setVector(ParamMap *self, PyObject *args)
{
	const char *str;
	double x, y, z;
	if(!PyArg_ParseTuple(args, "sddd", &str, &x, &y, &z)) Py_RETURN_FALSE;
	yafaray_setParamMapVector(self->param_map_, str, x, y, z);
	Py_RETURN_NONE;
}

PyObject *ParamMap::setString(ParamMap *self, PyObject *args)
{
	const char *str, *str_val;
	if(!PyArg_ParseTuple(args, "ss", &str, &str_val)) Py_RETURN_FALSE;
	yafaray_setParamMapString(self->param_map_, str, str_val);
	Py_RETURN_NONE;
}

PyObject *ParamMap::setBool(ParamMap *self, PyObject *args)
{
	const char *str;
	int val;
	if(!PyArg_ParseTuple(args, "sp", &str, &val)) Py_RETURN_FALSE;
	yafaray_setParamMapBool(self->param_map_, str, static_cast<yafaray_Bool>(val));
	Py_RETURN_NONE;
}

PyObject *ParamMap::setInt(ParamMap *self, PyObject *args)
{
	const char *str;
	int val;
	if(!PyArg_ParseTuple(args, "si", &str, &val)) Py_RETURN_FALSE;
	yafaray_setParamMapInt(self->param_map_, str, val);
	Py_RETURN_NONE;
}

PyObject *ParamMap::setFloat(ParamMap *self, PyObject *args)
{
	const char *str;
	double val;
	if(!PyArg_ParseTuple(args, "sd", &str, &val)) Py_RETURN_FALSE;
	yafaray_setParamMapFloat(self->param_map_, str, val);
	Py_RETURN_NONE;
}

PyObject *ParamMap::setColor(ParamMap *self, PyObject *args)
{
	const char *str;
	float r, g, b, a = 1.f;
	if(!PyArg_ParseTuple(args, "sfff|f", &str, &r, &g, &b, &a)) Py_RETURN_FALSE;
	yafaray_setParamMapColor(self->param_map_, str, r, g, b, a);
	Py_RETURN_NONE;
}

PyObject *ParamMap::setMatrix(ParamMap *self, PyObject *args)
{
	const char *str;
	double matrix[4][4];
	int transpose = YAFARAY_BOOL_FALSE;
	if(!PyArg_ParseTuple(args, "sdddddddddddddddd|p", &str,
			&matrix[0][0], &matrix[0][1], &matrix[0][2], &matrix[0][3],
			&matrix[1][0], &matrix[1][1], &matrix[1][2], &matrix[1][3],
			&matrix[2][0], &matrix[2][1], &matrix[2][2], &matrix[2][3],
			&matrix[3][0], &matrix[3][1], &matrix[3][2], &matrix[3][3],
			&transpose
	)) Py_RETURN_FALSE;
	yafaray_setParamMapMatrixArray(self->param_map_, str, reinterpret_cast<const double *>(matrix), static_cast<yafaray_Bool>(transpose));
	Py_RETURN_NONE;
}

PyObject *ParamMap::clear(ParamMap *self, PyObject *)
{
	yafaray_clearParamMap(self->param_map_);
	Py_RETURN_NONE;
}

PyObject *ParamMap::setInputColorSpace(ParamMap *self, PyObject *args)
{
	const char *color_space;
	float gamma;
	if(!PyArg_ParseTuple(args, "sf", &color_space, &gamma)) Py_RETURN_FALSE;
	yafaray_setInputColorSpace(self->param_map_, color_space, gamma);
	Py_RETURN_NONE;
}

} // namespace yafaray_bindings::python

