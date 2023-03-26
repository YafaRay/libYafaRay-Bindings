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
#ifdef YAFARAY_BINDINGS_WITH_XML
#include <yafaray_xml_c_api.h>
#endif //YAFARAY_BINDINGS_WITH_XML
#ifdef YAFARAY_BINDINGS_WITH_GUI
#include <yafaray_gui_c_api.h>
#endif //YAFARAY_BINDINGS_WITH_GUI
#include <cstring>

namespace yafaray_bindings::python
{

PyObject *setCallback(PyObject *&callback, PyObject *python_callback)
{
	if(python_callback == Py_None)
	{
		Py_XDECREF(callback);  /* Dispose of previous callback */
		callback = nullptr;
	}
	else
	{
		if(!PyCallable_Check(python_callback))
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			Py_RETURN_FALSE;
		}
		Py_XINCREF(python_callback);         /* Add a reference to new callback */
		Py_XDECREF(callback);         /* Dispose of previous callback */
		callback = python_callback;   /* Remember new callback */
	}
	Py_RETURN_NONE;
}

/*PyObject *xmlParseFile(PyObject *args)
{
#ifdef YAFARAY_BINDINGS_WITH_XML
	const char *xml_file_name;
	if(!PyArg_ParseTuple(args, "s", &xml_file_name)) Py_RETURN_FALSE;
	yafaray_xml_ParseFile(self->interf_, xml_file_name);
	Py_RETURN_NONE;
#else //YAFARAY_BINDINGS_WITH_XML
	yafaray_printError(self->interf_, "YafaRay Python Bindings not built with libYafaRay-Xml support, unable to open xml file.");
	Py_RETURN_FALSE;
#endif //YAFARAY_BINDINGS_WITH_XML
}

PyObject *xmlParseMemory(PyObject *args)
{
#ifdef YAFARAY_BINDINGS_WITH_XML
	const char *xml_text;
	if(!PyArg_ParseTuple(args, "s", &xml_text)) Py_RETURN_FALSE;
	yafaray_xml_ParseMemory(self->interf_, xml_text, strlen(xml_text));
	Py_RETURN_NONE;
#else //YAFARAY_BINDINGS_WITH_XML
	yafaray_printError(self->interf_, "YafaRay Python Bindings not built with libYafaRay-Xml support, unable to parse xml text string.");
	Py_RETURN_FALSE;
#endif //YAFARAY_BINDINGS_WITH_XML
}

PyObject *guiCreateRenderWidget(PyObject *args)
{
#ifdef YAFARAY_BINDINGS_WITH_GUI
	yafaray_gui_GuiToolKit_t gui_tool_kit = YAFARAY_GUI_QT;
	int width = 400, height = 300, border_start_x = 0, border_start_y = 0;
	yafaray_Bool auto_render = YAFARAY_BOOL_FALSE, close_after_finish = YAFARAY_BOOL_FALSE;
	if(!PyArg_ParseTuple(args, "|iiiippi", &width, &height, &border_start_x, &border_start_y, &auto_render, &close_after_finish, &gui_tool_kit)) Py_RETURN_FALSE;
	yafaray_gui_createRenderWidget(self->interf_, gui_tool_kit, width, height, border_start_x, border_start_y, auto_render, close_after_finish);
	Py_RETURN_NONE;
#else //YAFARAY_BINDINGS_WITH_GUI
	yafaray_printError(self->interf_, "YafaRay Python Bindings not built with libYafaRay-Gui support, unable to create render widget.");
	Py_RETURN_FALSE;
#endif //YAFARAY_BINDINGS_WITH_GUI
}*/

} // namespace yafaray_bindings::python

