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
#include "bindings_python/libyafaray_python.h"
#include "bindings_python/tile.h"
#endif //YAFARAY_BINDINGS_WITH_PYTHON

#include <cstring>

int yafaray_bindings_getVersionMajor() { return yafaray_bindings::buildinfo::getVersionMajor(); }
int yafaray_bindings_getVersionMinor() { return yafaray_bindings::buildinfo::getVersionMinor(); }
int yafaray_bindings_getVersionPatch() { return yafaray_bindings::buildinfo::getVersionPatch(); }

char *createCString(const std::string &std_string)
{
	const size_t string_size = std_string.size();
	char *c_string = new char[string_size + 1];
	std::strcpy(c_string, std_string.c_str());
	return c_string;
}

char *yafaray_bindings_getVersionString()
{
	return createCString(yafaray_bindings::buildinfo::getVersionString());
}

void yafaray_bindings_deallocateCharPointer(char *string_pointer_to_deallocate)
{
	delete[] string_pointer_to_deallocate;
}

#ifdef YAFARAY_BINDINGS_WITH_PYTHON
PyObject *PyInit_libyafaray4_bindings()
{
	//Preparing python multi-thread API for proper GIL management. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
	Py_Initialize();
#if PY_VERSION_HEX < 0x03090000
	PyEval_InitThreads(); //Creates GIL if it does not exist. Deprecated and does nothing starting from Python 3.9. Will be removed in Python 3.11.
#endif

	PyObject *py_module_object;
	if(PyType_Ready(&yafaray_bindings::python::YafaRayInterface_Type) < 0) return nullptr;
	if(PyType_Ready(&yafaray_bindings::python_tile_type_global) < 0) return nullptr;
	py_module_object = PyModule_Create(&yafaray_bindings::python::yafaray_module);
	Py_INCREF(&yafaray_bindings::python::YafaRayInterface_Type);
	Py_INCREF(&yafaray_bindings::python_tile_type_global);
	if(PyModule_AddObject(py_module_object, "Interface", reinterpret_cast<PyObject *>(&yafaray_bindings::python::YafaRayInterface_Type)) < 0)
	{
		Py_DECREF(&yafaray_bindings::python_tile_type_global);
		Py_DECREF(&yafaray_bindings::python::YafaRayInterface_Type);
		Py_DECREF(py_module_object);
		return nullptr;
	}
	return py_module_object;
}
#endif //YAFARAY_BINDINGS_WITH_PYTHON
