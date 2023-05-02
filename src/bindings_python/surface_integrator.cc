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

#include "bindings_python/surface_integrator.h"
#include "bindings_python/logger.h"
#include "bindings_python/param_map.h"
#include "bindings_python/scene.h"
#include "bindings_python/film.h"
#include "bindings_python/render_control.h"
#include "bindings_python/render_monitor.h"

namespace yafaray_bindings::python
{
PyMethodDef SurfaceIntegrator::py_methods_[]{
		{"create",                  reinterpret_cast<PyCFunction>(create),                  METH_VARARGS, ""},
		{"defineVolumeIntegrator",  reinterpret_cast<PyCFunction>(defineVolumeIntegrator),  METH_VARARGS, ""},
		{"preprocess",              reinterpret_cast<PyCFunction>(preprocess),              METH_VARARGS, ""},
		{"render",                  reinterpret_cast<PyCFunction>(render),                  METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject SurfaceIntegrator::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.SurfaceIntegrator", /* tp_name */
		sizeof(SurfaceIntegrator), /* tp_basicsize */
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
		"libYafaRay SurfaceIntegrator object", /* tp_doc */
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

PyObject *SurfaceIntegrator::create(PyTypeObject *type, PyObject *args, PyObject *)
{
	Logger *logger{nullptr};
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "OsO", &logger, &name, &param_map)) Py_RETURN_FALSE;
	auto *self = reinterpret_cast<SurfaceIntegrator *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->surface_integrator_ = yafaray_createSurfaceIntegrator(logger->get(), name, param_map->get());
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *SurfaceIntegrator::destroy(SurfaceIntegrator *self)
{
	yafaray_destroySurfaceIntegrator(self->surface_integrator_);
	self->surface_integrator_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

PyObject *SurfaceIntegrator::defineVolumeIntegrator(SurfaceIntegrator *self, PyObject *args)
{
	const Scene *scene{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "OO", &scene, &param_map)) Py_RETURN_FALSE;
	yafaray_defineVolumeIntegrator(self->surface_integrator_, scene->get(), param_map->get());
	Py_RETURN_NONE;
}

PyObject *SurfaceIntegrator::preprocess(SurfaceIntegrator *self, PyObject *args)
{
	RenderMonitor *render_monitor{nullptr};
	const RenderControl *render_control{nullptr};
	const Scene *scene{nullptr};
	if(!PyArg_ParseTuple(args, "OOO", &render_monitor, &render_control, &scene)) Py_RETURN_FALSE;
	//Save Python main thread state so the Python GIL is released and can be used in the render threads
	PyThreadState *save_thread = PyEval_SaveThread();
	yafaray_preprocessSurfaceIntegrator(render_monitor->get(), self->surface_integrator_, render_control->get(), scene->get());
	//Restore Python main thread state so the main thread retrieves the Python GIL
	PyEval_RestoreThread(save_thread);
	Py_RETURN_NONE;
}

PyObject *SurfaceIntegrator::render(SurfaceIntegrator *self, PyObject *args)
{
	RenderControl *render_control{nullptr};
	RenderMonitor *render_monitor{nullptr};
	Film *film{nullptr};
	if(PyArg_ParseTuple(args, "OOO", &render_control, &render_monitor, &film))
	{
		//Save Python main thread state so the Python GIL is released and can be used in the render threads
		PyThreadState *save_thread = PyEval_SaveThread();
		yafaray_render(render_control->get(), render_monitor->get(), self->surface_integrator_, film->get());
		//Restore Python main thread state so the main thread retrieves the Python GIL
		PyEval_RestoreThread(save_thread);
	}
	Py_RETURN_NONE;
}

} // namespace yafaray_bindings::python

