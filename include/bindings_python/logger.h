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

#ifndef LIBYAFARAY_BINDINGS_LOGGER_H
#define LIBYAFARAY_BINDINGS_LOGGER_H

#include "common.h"
#include <yafaray_c_api.h>

namespace yafaray_bindings::python
{
class Logger final // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	public:
		[[nodiscard]] yafaray_Logger *get() { return logger_; }
		[[nodiscard]] static PyTypeObject *getType() { return &py_type_; }

	private:
		[[nodiscard]] static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds);
		[[nodiscard]] static PyObject *destroy(Logger *self);
		[[nodiscard]] static PyObject *setCallback(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *enablePrintDateTime(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *setConsoleVerbosityLevel(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *setLogVerbosityLevel(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *logLevelFromString(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *printDebug(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *printVerbose(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *printInfo(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *printParams(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *printWarning(Logger *self, PyObject *args);
		[[nodiscard]] static PyObject *printError(Logger *self, PyObject *args);
		static void callback(yafaray_LogLevel log_level, size_t datetime, const char *time_of_day, const char *description, void *callback_user_data);

		PyObject_HEAD
		yafaray_Logger *logger_ = nullptr;
		PyObject *py_callback_ = nullptr;
		static PyMethodDef py_methods_[];
		static PyTypeObject py_type_;
};

} // namespace yafaray_bindings::python

#endif //LIBYAFARAY_BINDINGS_LOGGER_H
