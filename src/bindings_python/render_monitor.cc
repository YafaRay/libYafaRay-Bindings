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

#include "bindings_python/render_monitor.h"
#include <yafaray_c_api.h>

namespace yafaray_bindings::python
{
PyMethodDef RenderMonitor::py_methods_[]{
		{"create",                  reinterpret_cast<PyCFunction>(create),                  METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject RenderMonitor::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.RenderMonitor", /* tp_name */
		sizeof(RenderMonitor), /* tp_basicsize */
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
		"libYafaRay RenderMonitor object", /* tp_doc */
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

PyObject *RenderMonitor::create(PyTypeObject *type, PyObject *args, PyObject *)
{
	PyObject *callback{nullptr};
	PyArg_ParseTuple(args, "|O:monitorCallback", &callback);
	auto *self = reinterpret_cast<RenderMonitor *>(type->tp_alloc(type, 0));
	if(self)
	{
		if(!callback)
		{
			self->render_monitor_ = yafaray_createRenderMonitor(nullptr, nullptr, YAFARAY_DISPLAY_CONSOLE_NORMAL);
		}
		else
		{
			if(!PyCallable_Check(callback))
			{
				PyErr_SetString(PyExc_TypeError, "parameter must be callable");
				Py_RETURN_FALSE;
			}
			Py_XINCREF(callback);         /* Add a reference to new callback */
			Py_XDECREF(self->monitor_callback_);  /* Dispose of previous callback */
			self->monitor_callback_ = callback;       /* Remember new callback */
			self->render_monitor_ = yafaray_createRenderMonitor(RenderMonitor::monitorCallback, self, YAFARAY_DISPLAY_CONSOLE_HIDDEN);
		}
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *RenderMonitor::destroy(RenderMonitor *self)
{
	yafaray_destroyRenderMonitor(self->render_monitor_);
	self->render_monitor_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

void RenderMonitor::monitorCallback(int steps_total, int steps_done, const char *tag, void *callback_user_data)
{
	auto render_monitor = reinterpret_cast<RenderMonitor *>(callback_user_data);
	if(render_monitor->monitor_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("iis", steps_total, steps_done, tag);
		PyObject_CallObject(render_monitor->monitor_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

} // namespace yafaray_bindings::python

