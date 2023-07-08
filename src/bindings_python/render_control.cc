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

#include "bindings_python/render_control.h"
#include <yafaray_c_api.h>

namespace yafaray_bindings::python
{

PyMethodDef RenderControl::py_methods_[]{
		{"create",                    reinterpret_cast<PyCFunction>(create),                 METH_NOARGS, ""},
		{"cancel",                    reinterpret_cast<PyCFunction>(cancel),                 METH_NOARGS, ""},
		{"set_for_normal_start",      reinterpret_cast<PyCFunction>(setForNormalStart),      METH_NOARGS, ""},
		{"set_for_progressive_start", reinterpret_cast<PyCFunction>(setForProgressiveStart), METH_NOARGS, ""},
		{"set_for_resuming",          reinterpret_cast<PyCFunction>(setForResuming),         METH_NOARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject RenderControl::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.RenderControl", /* tp_name */
		sizeof(RenderControl), /* tp_basicsize */
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
		"libYafaRay RenderControl object", /* tp_doc */
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

PyObject *RenderControl::create(PyTypeObject *type, PyObject *args, PyObject *)
{
	auto *self = reinterpret_cast<RenderControl *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->render_control_ = yafaray_createRenderControl();
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *RenderControl::destroy(RenderControl *self)
{
	yafaray_destroyRenderControl(self->render_control_);
	self->render_control_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

PyObject *RenderControl::cancel(RenderControl *self, PyObject *args)
{
	yafaray_cancelRendering(self->render_control_);
	Py_RETURN_NONE;
}

PyObject *RenderControl::setForNormalStart(RenderControl *self, PyObject *args)
{
	yafaray_setRenderControlForNormalStart(self->render_control_);
	Py_RETURN_NONE;
}


PyObject *RenderControl::setForProgressiveStart(RenderControl *self, PyObject *args)
{
	yafaray_setRenderControlForProgressiveStart(self->render_control_);
	Py_RETURN_NONE;
}


PyObject *RenderControl::setForResuming(RenderControl *self, PyObject *args)
{
	yafaray_setRenderControlForResuming(self->render_control_);
	Py_RETURN_NONE;
}


} // namespace yafaray_bindings::python

