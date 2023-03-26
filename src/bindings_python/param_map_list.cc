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

#include "bindings_python/param_map_list.h"
#include "bindings_python/param_map.h"
#include <yafaray_c_api.h>

namespace yafaray_bindings::python
{
PyMethodDef ParamMapList::py_methods_[]{
		{"create",      reinterpret_cast<PyCFunction>(create),      METH_VARARGS, ""},
		{"clear",       reinterpret_cast<PyCFunction>(clear),       METH_NOARGS,  ""},
		{"addParamMap", reinterpret_cast<PyCFunction>(addParamMap), METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject ParamMapList::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.ParamMapList", /* tp_name */
		sizeof(ParamMapList), /* tp_basicsize */
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
		"libYafaRay ParamMapList object", /* tp_doc */
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

PyObject *ParamMapList::create(PyTypeObject *type, PyObject *, PyObject *)
{
	auto *self = reinterpret_cast<ParamMapList *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->param_map_list_ = yafaray_createParamMapList();
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *ParamMapList::destroy(ParamMapList *self)
{
	yafaray_destroyParamMapList(self->param_map_list_);
	self->param_map_list_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

PyObject *ParamMapList::clear(ParamMapList *self, PyObject *)
{
	yafaray_clearParamMapList(self->param_map_list_);
	Py_RETURN_NONE;
}

PyObject *ParamMapList::addParamMap(ParamMapList *self, PyObject *args)
{
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "O", &param_map)) Py_RETURN_FALSE;
	yafaray_addParamMapToList(self->param_map_list_, param_map->get());
	Py_RETURN_NONE;
}

} // namespace yafaray_bindings::python

