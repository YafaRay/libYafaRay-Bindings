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

#include "bindings_python/renderer.h"
#include "bindings_python/logger.h"
#include "bindings_python/param_map.h"
#include "bindings_python/scene.h"
#include "bindings_python/film.h"

namespace yafaray_bindings::python
{
PyMethodDef Renderer::py_methods_[]{
		{"create",                  reinterpret_cast<PyCFunction>(create),                  METH_VARARGS, ""},
		{"defineSurfaceIntegrator", reinterpret_cast<PyCFunction>(defineSurfaceIntegrator), METH_VARARGS, ""},
		{"defineVolumeIntegrator",  reinterpret_cast<PyCFunction>(defineVolumeIntegrator),  METH_VARARGS, ""},
		{"setup",                   reinterpret_cast<PyCFunction>(setup),                   METH_VARARGS, ""},
		{"render",                  reinterpret_cast<PyCFunction>(render),                  METH_VARARGS, ""},
		{"cancel",                  reinterpret_cast<PyCFunction>(cancel),                  METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject Renderer::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.Renderer", /* tp_name */
		sizeof(Renderer), /* tp_basicsize */
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
		"libYafaRay Renderer object", /* tp_doc */
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

PyObject *Renderer::create(PyTypeObject *type, PyObject *args, PyObject *)
{
	Logger *logger{nullptr};
	const Scene *scene{nullptr};
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "OOsO", &logger, &scene, &name, &param_map)) Py_RETURN_FALSE;
	auto *self = reinterpret_cast<Renderer *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->renderer_ = yafaray_createRenderer(logger->get(), scene->get(), name, YAFARAY_DISPLAY_CONSOLE_NORMAL, param_map->get());
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *Renderer::destroy(Renderer *self)
{
	yafaray_destroyRenderer(self->renderer_);
	self->renderer_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

PyObject *Renderer::defineSurfaceIntegrator(Renderer *self, PyObject *args)
{
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "O", &param_map)) Py_RETURN_FALSE;
	yafaray_defineSurfaceIntegrator(self->renderer_, param_map->get());
	Py_RETURN_NONE;
}

PyObject *Renderer::defineVolumeIntegrator(Renderer *self, PyObject *args)
{
	const Scene *scene{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "OO", &scene, &param_map)) Py_RETURN_FALSE;
	yafaray_defineVolumeIntegrator(self->renderer_, scene->get(), param_map->get());
	Py_RETURN_NONE;
}

PyObject *Renderer::setup(Renderer *self, PyObject *args)
{
	Scene *scene{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "OO", &scene, &param_map)) Py_RETURN_FALSE;
	yafaray_setupRender(scene->get(), self->renderer_, param_map->get());
	Py_RETURN_NONE;
}

void Renderer::monitorCallback(int steps_total, int steps_done, const char *tag, void *callback_user_data)
{
	auto renderer = reinterpret_cast<Renderer *>(callback_user_data);
	if(renderer->monitor_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("iis", steps_total, steps_done, tag);
		PyObject_CallObject(renderer->monitor_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

PyObject *Renderer::render(Renderer *self, PyObject *args)
{
	const Scene *scene{nullptr};
	Film *film{nullptr};
	PyObject *callback{nullptr};
//	if(PyArg_ParseTuple(args, "ii|O:monitorCallback", &film->border_start_x_, &film->border_start_y_, &callback))
	if(PyArg_ParseTuple(args, "OOO", &film, &scene, &callback))
	{
		//Save Python main thread state so the Python GIL is released and can be used in the render threads
		PyThreadState *save_thread = PyEval_SaveThread();

		if(!callback)
		{
			yafaray_render(self->renderer_, film->get(), scene->get(), nullptr, nullptr, YAFARAY_DISPLAY_CONSOLE_NORMAL);
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
			yafaray_render(self->renderer_, film->get(), scene->get(), Renderer::monitorCallback, self, YAFARAY_DISPLAY_CONSOLE_HIDDEN);
		}

		//Restore Python main thread state so the main thread retrieves the Python GIL
		PyEval_RestoreThread(save_thread);
	}
	Py_RETURN_NONE;
}

PyObject *Renderer::cancel(Renderer *self, PyObject *args)
{
	Logger *logger{nullptr};
	if(!PyArg_ParseTuple(args, "O", &logger)) Py_RETURN_FALSE;
	yafaray_cancelRendering(logger->get(), self->renderer_);
	Py_RETURN_NONE;
}

} // namespace yafaray_bindings::python

