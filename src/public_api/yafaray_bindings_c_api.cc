/****************************************************************************
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
 */

#include "public_api/yafaray_bindings_c_api.h"
#include "common/version_build_info.h"
#ifdef YAFARAY_BINDINGS_WITH_PYTHON
#include "bindings_python/common.h"
#include "bindings_python/logger.h"
#include "bindings_python/param_map.h"
#include "bindings_python/param_map_list.h"
#include "bindings_python/scene.h"
#include "bindings_python/surface_integrator.h"
#include "bindings_python/film.h"
#include "bindings_python/render_control.h"
#include "bindings_python/render_monitor.h"
#include "bindings_python/tile.h"
#endif //YAFARAY_BINDINGS_WITH_PYTHON

#include <cstring>

int yafaray_bindings_getVersionMajor() { return yafaray_bindings::build_info::getVersionMajor(); }
int yafaray_bindings_getVersionMinor() { return yafaray_bindings::build_info::getVersionMinor(); }
int yafaray_bindings_getVersionPatch() { return yafaray_bindings::build_info::getVersionPatch(); }

char *createCString(const std::string &std_string)
{
	const size_t string_size = std_string.size();
	char *c_string = new char[string_size + 1];
	std::strcpy(c_string, std_string.c_str());
	return c_string;
}

char *yafaray_bindings_getVersionString()
{
	return createCString(yafaray_bindings::build_info::getVersionString());
}

void yafaray_bindings_destroyCharString(char *string)
{
	delete[] string;
}

#ifdef YAFARAY_BINDINGS_WITH_PYTHON
PyObject *PyInit_libyafaray4_bindings()
{
	//Preparing python multi-thread API for proper GIL management. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
	Py_Initialize();
#if PY_VERSION_HEX < 0x03090000
	PyEval_InitThreads(); //Creates GIL if it does not exist. Deprecated and does nothing starting from Python 3.9. Will be removed in Python 3.11.
#endif

	if(PyType_Ready(yafaray_bindings::python::Logger::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::ParamMap::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::ParamMapList::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::Scene::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::SurfaceIntegrator::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::Film::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::RenderControl::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::RenderMonitor::getType()) < 0) return nullptr;
	if(PyType_Ready(yafaray_bindings::python::Tile::getType()) < 0) return nullptr;

	Py_INCREF(yafaray_bindings::python::Logger::getType());
	Py_INCREF(yafaray_bindings::python::ParamMap::getType());
	Py_INCREF(yafaray_bindings::python::ParamMapList::getType());
	Py_INCREF(yafaray_bindings::python::Scene::getType());
	Py_INCREF(yafaray_bindings::python::SurfaceIntegrator::getType());
	Py_INCREF(yafaray_bindings::python::Film::getType());
	Py_INCREF(yafaray_bindings::python::RenderControl::getType());
	Py_INCREF(yafaray_bindings::python::RenderMonitor::getType());
	Py_INCREF(yafaray_bindings::python::Tile::getType());

	PyObject *py_module_object = PyModule_Create(&yafaray_bindings::python::yafaray_module);
	if(PyModule_AddObject(py_module_object, "Logger", reinterpret_cast<PyObject *>(yafaray_bindings::python::Logger::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::Logger::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	if(PyModule_AddObject(py_module_object, "ParamMap", reinterpret_cast<PyObject *>(yafaray_bindings::python::ParamMap::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::ParamMap::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	if(PyModule_AddObject(py_module_object, "ParamMapList", reinterpret_cast<PyObject *>(yafaray_bindings::python::ParamMapList::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::ParamMapList::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	if(PyModule_AddObject(py_module_object, "Scene", reinterpret_cast<PyObject *>(yafaray_bindings::python::Scene::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::Scene::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	if(PyModule_AddObject(py_module_object, "SurfaceIntegrator", reinterpret_cast<PyObject *>(yafaray_bindings::python::SurfaceIntegrator::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::SurfaceIntegrator::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	if(PyModule_AddObject(py_module_object, "Film", reinterpret_cast<PyObject *>(yafaray_bindings::python::Film::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::Tile::getType());
		Py_DECREF(yafaray_bindings::python::Film::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	if(PyModule_AddObject(py_module_object, "RenderControl", reinterpret_cast<PyObject *>(yafaray_bindings::python::RenderControl::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::RenderControl::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	if(PyModule_AddObject(py_module_object, "RenderMonitor", reinterpret_cast<PyObject *>(yafaray_bindings::python::RenderMonitor::getType())) < 0)
	{
		Py_DECREF(yafaray_bindings::python::RenderMonitor::getType());
		Py_DECREF(py_module_object);
		return nullptr;
	}
	return py_module_object;
}
#endif //YAFARAY_BINDINGS_WITH_PYTHON
