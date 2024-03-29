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

#ifndef YAFARAY_BINDINGS_C_API_H
#define YAFARAY_BINDINGS_C_API_H

#include "yafaray_bindings_c_api_export.h"
#ifdef YAFARAY_BINDINGS_WITH_PYTHON
#define PY_SSIZE_T_CLEAN
#ifdef _WIN64
#define _hypot hypot
#include <cmath>
#endif
#include <Python.h>
#endif //YAFARAY_BINDINGS_WITH_PYTHON

#define YAFARAY_BINDINGS_C_API_VERSION_MAJOR 4

#ifdef __cplusplus
extern "C" {
#endif

	YAFARAY_BINDINGS_C_API_EXPORT int yafaray_bindings_getVersionMajor();
	YAFARAY_BINDINGS_C_API_EXPORT int yafaray_bindings_getVersionMinor();
	YAFARAY_BINDINGS_C_API_EXPORT int yafaray_bindings_getVersionPatch();
	/* The following functions return a text string where memory is allocated by libYafaRay itself. Do not free the char* directly with free, use "yafaray_bindings_deallocateCharPointer" to free them instead to ensure proper deallocation. */
	YAFARAY_BINDINGS_C_API_EXPORT char *yafaray_bindings_getVersionString();
	YAFARAY_BINDINGS_C_API_EXPORT void yafaray_bindings_destroyCharString(char *string);
#ifdef YAFARAY_BINDINGS_WITH_PYTHON
	YAFARAY_BINDINGS_C_API_EXPORT PyObject* PyInit_libyafaray4_bindings(void);
#endif //YAFARAY_BINDINGS_WITH_PYTHON

#ifdef __cplusplus
}
#endif

#endif /* YAFARAY_BINDINGS_C_API_H */
