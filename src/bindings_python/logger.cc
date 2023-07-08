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

#include "bindings_python/logger.h"

namespace yafaray_bindings::python
{

PyMethodDef Logger::py_methods_[]{
		{"create",                      reinterpret_cast<PyCFunction>(create),                   METH_NOARGS,  ""},
		{"set_callback",                reinterpret_cast<PyCFunction>(setCallback),              METH_VARARGS, ""},
		{"enable_print_date_time",      reinterpret_cast<PyCFunction>(enablePrintDateTime),      METH_VARARGS, ""},
		{"set_console_verbosity_level", reinterpret_cast<PyCFunction>(setConsoleVerbosityLevel), METH_VARARGS, ""},
		{"set_log_verbosity_level",     reinterpret_cast<PyCFunction>(setLogVerbosityLevel),     METH_VARARGS, ""},
		{"log_level_from_string",       reinterpret_cast<PyCFunction>(logLevelFromString),       METH_VARARGS, ""},
		{"print_debug",                 reinterpret_cast<PyCFunction>(printDebug),               METH_VARARGS, ""},
		{"print_verbose",               reinterpret_cast<PyCFunction>(printVerbose),             METH_VARARGS, ""},
		{"print_info",                  reinterpret_cast<PyCFunction>(printInfo),                METH_VARARGS, ""},
		{"print_params",                reinterpret_cast<PyCFunction>(printParams),              METH_VARARGS, ""},
		{"print_warning",               reinterpret_cast<PyCFunction>(printWarning),             METH_VARARGS, ""},
		{"print_error",                 reinterpret_cast<PyCFunction>(printError),               METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject Logger::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.Logger", /* tp_name */
		sizeof(Logger), /* tp_basicsize */
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
		"libYafaRay Logger object", /* tp_doc */
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

PyObject *Logger::create(PyTypeObject *type, PyObject *, PyObject *)
{
	auto *self = reinterpret_cast<Logger *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->logger_ = yafaray_createLogger(Logger::callback, self, YAFARAY_DISPLAY_CONSOLE_NORMAL);
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *Logger::destroy(Logger *self)
{
	yafaray_destroyLogger(self->logger_);
	self->logger_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

void Logger::callback(yafaray_LogLevel log_level, size_t datetime, const char *time_of_day, const char *description, void *callback_user_data)
{
	auto logger = reinterpret_cast<Logger *>(callback_user_data);
	if(logger->py_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("ilss", log_level, datetime, time_of_day, description);
		PyObject_CallObject(logger->py_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

PyObject *Logger::setCallback(Logger *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:callback", &callback))
	{
		if (!PyCallable_Check(callback))
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			Py_RETURN_FALSE;
		}
		Py_XINCREF(callback);         /* Add a reference to new callback */
		Py_XDECREF(self->py_callback_);  /* Dispose of previous callback */
		self->py_callback_ = callback;       /* Remember new callback */
		yafaray_setLoggerCallbacks(self->logger_, Logger::callback, self);
	}
	Py_RETURN_NONE;
}

PyObject *Logger::enablePrintDateTime(Logger *self, PyObject *args)
{
	int enable_print_date_time;
	if(!PyArg_ParseTuple(args, "p", &enable_print_date_time)) Py_RETURN_FALSE;
	yafaray_enablePrintDateTime(self->logger_, static_cast<yafaray_Bool>(enable_print_date_time));
	Py_RETURN_NONE;
}

PyObject *Logger::setConsoleVerbosityLevel(Logger *self, PyObject *args)
{
	int log_level;
	if(PyArg_ParseTuple(args, "i", &log_level)) yafaray_setConsoleVerbosityLevel(self->logger_, static_cast<yafaray_LogLevel>(log_level));
	Py_RETURN_NONE;
}

PyObject *Logger::setLogVerbosityLevel(Logger *self, PyObject *args)
{
	int log_level;
	if(PyArg_ParseTuple(args, "i", &log_level)) yafaray_setLogVerbosityLevel(self->logger_, static_cast<yafaray_LogLevel>(log_level));
	Py_RETURN_NONE;
}

PyObject *Logger::logLevelFromString(Logger *, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) return PyLong_FromLong(YAFARAY_LOG_LEVEL_INFO);
	else return PyLong_FromLong(yafaray_logLevelFromString(text));
}

PyObject *Logger::printDebug(Logger *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printDebug(self->logger_, text);
	Py_RETURN_NONE;
}

PyObject *Logger::printVerbose(Logger *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printVerbose(self->logger_, text);
	Py_RETURN_NONE;
}

PyObject *Logger::printInfo(Logger *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printInfo(self->logger_, text);
	Py_RETURN_NONE;
}

PyObject *Logger::printParams(Logger *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printParams(self->logger_, text);
	Py_RETURN_NONE;
}

PyObject *Logger::printWarning(Logger *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printWarning(self->logger_, text);
	Py_RETURN_NONE;
}

PyObject *Logger::printError(Logger *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printError(self->logger_, text);
	Py_RETURN_NONE;
}

} // namespace yafaray_bindings::python

