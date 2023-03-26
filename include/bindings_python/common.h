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

#ifndef LIBYAFARAY_BINDINGS_COMMON_H
#define LIBYAFARAY_BINDINGS_COMMON_H

#define PY_SSIZE_T_CLEAN
#ifdef _WIN64
#define _hypot hypot
#endif
#include <Python.h>

namespace yafaray_bindings::python
{
static struct PyModuleDef yafaray_module {
		PyModuleDef_HEAD_INIT,
		"yafaray", /* name of module */
		"libYafaRay Python Bindings", /* module documentation, may be NULL */
		-1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
		//python_public_methods
};

PyObject *setCallback(PyObject *&callback, PyObject *python_callback);

} // namespace yafaray_bindings::python

#endif //LIBYAFARAY_BINDINGS_COMMON_H
