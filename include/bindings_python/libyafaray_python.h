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

#ifndef YAFARAY_BINDINGS_LIBYAFARAY_PYTHON_H
#define YAFARAY_BINDINGS_LIBYAFARAY_PYTHON_H

#include "common/yafaray_bindings_common.h"
#include "common/images_collection.h"
#include <yafaray_c_api.h>
#define PY_SSIZE_T_CLEAN
#include <Python.h>

BEGIN_YAFARAY_BINDINGS
namespace python
{

static struct PyModuleDef yafaray_module = {
		PyModuleDef_HEAD_INIT,
		"yafaray", /* name of module */
		"libYafaRay Python Bindings", /* module documentation, may be NULL */
		-1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
		//python_public_methods
};

struct YafaRayInterface
{
	PyObject_HEAD
	yafaray_Interface_t *interf_ = nullptr;
	ImagesCollection *images_collection_ = nullptr;
	int border_start_x_ = 0;
	int border_start_y_ = 0;
	PyObject *python_logging_callback_ = nullptr;
	PyObject *python_notify_view_callback_ = nullptr;
	PyObject *python_notify_layer_callback_ = nullptr;
	PyObject *python_putpixel_callback_ = nullptr;
	PyObject *python_highlight_pixel_callback_ = nullptr;
	PyObject *python_flush_area_callback_ = nullptr;
	PyObject *python_flush_callback_ = nullptr;
	PyObject *python_highlight_callback_ = nullptr;
	PyObject *python_monitor_callback_ = nullptr;
	void setupInternalFilmCallbacks();
	static void loggerCallback(yafaray_LogLevel_t log_level, long datetime, const char *time_of_day, const char *description, void *callback_user_data);
	static void notifyViewCallback(const char *view_name, void *callback_user_data);
	static void notifyLayerCallback(const char *internal_layer_name, const char *exported_layer_name, int width, int height, int layer_exported_channels, void *callback_user_data);
	static void putPixelCallback(const char *view_name, const char *layer_name, int x, int y, float r, float g, float b, float a, void *callback_user_data);
	static void highlightPixelCallback(const char *view_name, int x, int y, float r, float g, float b, float a, void *callback_user_data);
	static void flushAreaCallback(const char *view_name, int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data);
	static void flushCallback(const char *view_name, void *callback_user_data);
	static void highlightAreaCallback(const char *view_name, int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data);
	static void monitorCallback(int steps_total, int steps_done, const char *tag, void *callback_user_data);
};

PyObject *createInterface(PyTypeObject *type, PyObject *args, PyObject *kwds);
PyObject *destroyInterface(YafaRayInterface *self);
PyObject *setLoggingCallback(YafaRayInterface *self, PyObject *args);
PyObject *createScene(YafaRayInterface *self, PyObject *);
PyObject *getSceneFilmWidth(YafaRayInterface *self, PyObject *);
PyObject *getSceneFilmHeight(YafaRayInterface *self, PyObject *);
PyObject *startGeometry(YafaRayInterface *self, PyObject *);
PyObject *endGeometry(YafaRayInterface *self, PyObject *);
PyObject *getNextFreeId(YafaRayInterface *self, PyObject *);
PyObject *endObject(YafaRayInterface *self, PyObject *);
PyObject *addVertex(YafaRayInterface *self, PyObject *args);
PyObject *addVertexWithOrco(YafaRayInterface *self, PyObject *args);
PyObject *addNormal(YafaRayInterface *self, PyObject *args);
PyObject *addTriangle(YafaRayInterface *self, PyObject *args);
PyObject *addTriangleWithUv(YafaRayInterface *self, PyObject *args);
PyObject *addUv(YafaRayInterface *self, PyObject *args);
PyObject *smoothMesh(YafaRayInterface *self, PyObject *args);
PyObject *addInstance(YafaRayInterface *self, PyObject *args);
PyObject *paramsSetVector(YafaRayInterface *self, PyObject *args);
PyObject *paramsSetString(YafaRayInterface *self, PyObject *args);
PyObject *paramsSetBool(YafaRayInterface *self, PyObject *args);
PyObject *paramsSetInt(YafaRayInterface *self, PyObject *args);
PyObject *paramsSetFloat(YafaRayInterface *self, PyObject *args);
PyObject *paramsSetColor(YafaRayInterface *self, PyObject *args);
PyObject *paramsSetMatrix(YafaRayInterface *self, PyObject *args);
PyObject *paramsClearAll(YafaRayInterface *self, PyObject *);
PyObject *paramsPushList(YafaRayInterface *self, PyObject *);
PyObject *paramsEndList(YafaRayInterface *self, PyObject *);
PyObject *setCurrentMaterial(YafaRayInterface *self, PyObject *args);
PyObject *createObject(YafaRayInterface *self, PyObject *args);
PyObject *createLight(YafaRayInterface *self, PyObject *args);
PyObject *createTexture(YafaRayInterface *self, PyObject *args);
PyObject *createMaterial(YafaRayInterface *self, PyObject *args);
PyObject *createCamera(YafaRayInterface *self, PyObject *args);
PyObject *createBackground(YafaRayInterface *self, PyObject *args);
PyObject *createIntegrator(YafaRayInterface *self, PyObject *args);
PyObject *createVolumeRegion(YafaRayInterface *self, PyObject *args);
PyObject *createRenderView(YafaRayInterface *self, PyObject *args);
PyObject *createOutput(YafaRayInterface *self, PyObject *args);
PyObject *setRenderNotifyViewCallback(YafaRayInterface *self, PyObject *args);
PyObject *setRenderNotifyLayerCallback(YafaRayInterface *self, PyObject *args);
PyObject *setRenderPutPixelCallback(YafaRayInterface *self, PyObject *args);
PyObject *setRenderHighlightPixelCallback(YafaRayInterface *self, PyObject *args);
PyObject *setRenderFlushAreaCallback(YafaRayInterface *self, PyObject *args);
PyObject *setRenderFlushCallback(YafaRayInterface *self, PyObject *args);
PyObject *setRenderHighlightAreaCallback(YafaRayInterface *self, PyObject *args);
PyObject *removeOutput(YafaRayInterface *self, PyObject *args);
PyObject *clearOutputs(YafaRayInterface *self, PyObject *);
PyObject *clearAll(YafaRayInterface *self, PyObject *);
PyObject *setupRender(YafaRayInterface *self, PyObject *);
PyObject *render(YafaRayInterface *self, PyObject *args);
PyObject *defineLayer(YafaRayInterface *self, PyObject *);
PyObject *enablePrintDateTime(YafaRayInterface *self, PyObject *args);
PyObject *setConsoleVerbosityLevel(YafaRayInterface *self, PyObject *args);
PyObject *setLogVerbosityLevel(YafaRayInterface *self, PyObject *args);
PyObject *logLevelFromString(YafaRayInterface *self, PyObject *args);
PyObject *printDebug(YafaRayInterface *self, PyObject *args);
PyObject *printVerbose(YafaRayInterface *self, PyObject *args);
PyObject *printInfo(YafaRayInterface *self, PyObject *args);
PyObject *printParams(YafaRayInterface *self, PyObject *args);
PyObject *printWarning(YafaRayInterface *self, PyObject *args);
PyObject *printError(YafaRayInterface *self, PyObject *args);
PyObject *cancelRendering(YafaRayInterface *self, PyObject *);
PyObject *setInputColorSpace(YafaRayInterface *self, PyObject *args);
PyObject *createImage(YafaRayInterface *self, PyObject *args);
PyObject *setImageColor(YafaRayInterface *self, PyObject *args);
PyObject *getImageColor(YafaRayInterface *self, PyObject *args);
PyObject *setConsoleLogColorsEnabled(YafaRayInterface *self, PyObject *args);
PyObject *xmlParseFile(YafaRayInterface *self, PyObject *args);
PyObject *xmlParseMemory(YafaRayInterface *self, PyObject *args);
PyObject *guiCreateRenderWidget(YafaRayInterface *self, PyObject *args);

static PyMethodDef yafaray_methods[] = {
		{"setLoggingCallback", reinterpret_cast<PyCFunction>(setLoggingCallback), METH_VARARGS, ""},
		{"createScene", reinterpret_cast<PyCFunction>(createScene), METH_NOARGS, ""},
		{"getSceneFilmWidth", reinterpret_cast<PyCFunction>(getSceneFilmWidth), METH_NOARGS, ""},
		{"getSceneFilmHeight", reinterpret_cast<PyCFunction>(getSceneFilmHeight), METH_NOARGS, ""},
		{"startGeometry", reinterpret_cast<PyCFunction>(startGeometry), METH_NOARGS, ""},
		{"endGeometry", reinterpret_cast<PyCFunction>(endGeometry), METH_NOARGS, ""},
		{"getNextFreeId", reinterpret_cast<PyCFunction>(getNextFreeId), METH_NOARGS, ""},
		{"endObject", reinterpret_cast<PyCFunction>(endObject), METH_NOARGS, ""},
		{"addVertex", reinterpret_cast<PyCFunction>(addVertex), METH_VARARGS, ""},
		{"addVertexWithOrco", reinterpret_cast<PyCFunction>(addVertexWithOrco), METH_VARARGS, ""},
		{"addNormal", reinterpret_cast<PyCFunction>(addNormal), METH_VARARGS, ""},
		{"addTriangle", reinterpret_cast<PyCFunction>(addTriangle), METH_VARARGS, ""},
		{"addTriangleWithUv", reinterpret_cast<PyCFunction>(addTriangleWithUv), METH_VARARGS, ""},
		{"addUv", reinterpret_cast<PyCFunction>(addUv), METH_VARARGS, ""},
		{"smoothMesh", reinterpret_cast<PyCFunction>(smoothMesh), METH_VARARGS, ""},
		{"addInstance", reinterpret_cast<PyCFunction>(addInstance), METH_VARARGS, ""},
		{"paramsSetVector", reinterpret_cast<PyCFunction>(paramsSetVector), METH_VARARGS, ""},
		{"paramsSetString", reinterpret_cast<PyCFunction>(paramsSetString), METH_VARARGS, ""},
		{"paramsSetBool", reinterpret_cast<PyCFunction>(paramsSetBool), METH_VARARGS, ""},
		{"paramsSetInt", reinterpret_cast<PyCFunction>(paramsSetInt), METH_VARARGS, ""},
		{"paramsSetFloat", reinterpret_cast<PyCFunction>(paramsSetFloat), METH_VARARGS, ""},
		{"paramsSetColor", reinterpret_cast<PyCFunction>(paramsSetColor), METH_VARARGS, ""},
		{"paramsSetMatrix", reinterpret_cast<PyCFunction>(paramsSetMatrix), METH_VARARGS, ""},
		{"paramsClearAll", reinterpret_cast<PyCFunction>(paramsClearAll), METH_NOARGS, ""},
		{"paramsPushList", reinterpret_cast<PyCFunction>(paramsPushList), METH_NOARGS, ""},
		{"paramsEndList", reinterpret_cast<PyCFunction>(paramsEndList), METH_NOARGS, ""},
		{"setCurrentMaterial", reinterpret_cast<PyCFunction>(setCurrentMaterial), METH_VARARGS, ""},
		{"createObject", reinterpret_cast<PyCFunction>(createObject), METH_VARARGS, ""},
		{"createLight", reinterpret_cast<PyCFunction>(createLight), METH_VARARGS, ""},
		{"createTexture", reinterpret_cast<PyCFunction>(createTexture), METH_VARARGS, ""},
		{"createMaterial", reinterpret_cast<PyCFunction>(createMaterial), METH_VARARGS, ""},
		{"createCamera", reinterpret_cast<PyCFunction>(createCamera), METH_VARARGS, ""},
		{"createBackground", reinterpret_cast<PyCFunction>(createBackground), METH_VARARGS, ""},
		{"createIntegrator", reinterpret_cast<PyCFunction>(createIntegrator), METH_VARARGS, ""},
		{"createVolumeRegion", reinterpret_cast<PyCFunction>(createVolumeRegion), METH_VARARGS, ""},
		{"createRenderView", reinterpret_cast<PyCFunction>(createRenderView), METH_VARARGS, ""},
		{"createOutput", reinterpret_cast<PyCFunction>(createOutput), METH_VARARGS, ""},
		{"setRenderNotifyViewCallback", reinterpret_cast<PyCFunction>(setRenderNotifyViewCallback), METH_VARARGS, ""},
		{"setRenderNotifyLayerCallback", reinterpret_cast<PyCFunction>(setRenderNotifyLayerCallback), METH_VARARGS, ""},
		{"setRenderPutPixelCallback", reinterpret_cast<PyCFunction>(setRenderPutPixelCallback), METH_VARARGS, ""},
		{"setRenderHighlightPixelCallback", reinterpret_cast<PyCFunction>(setRenderHighlightPixelCallback), METH_VARARGS, ""},
		{"setRenderFlushAreaCallback", reinterpret_cast<PyCFunction>(setRenderFlushAreaCallback), METH_VARARGS, ""},
		{"setRenderFlushCallback", reinterpret_cast<PyCFunction>(setRenderFlushCallback), METH_VARARGS, ""},
		{"setRenderHighlightAreaCallback", reinterpret_cast<PyCFunction>(setRenderHighlightAreaCallback), METH_VARARGS, ""},
		{"removeOutput", reinterpret_cast<PyCFunction>(removeOutput), METH_VARARGS, ""},
		{"clearOutputs", reinterpret_cast<PyCFunction>(clearOutputs), METH_NOARGS, ""},
		{"clearAll", reinterpret_cast<PyCFunction>(clearAll), METH_NOARGS, ""},
		{"setupRender", reinterpret_cast<PyCFunction>(setupRender), METH_NOARGS, ""},
		{"render", reinterpret_cast<PyCFunction>(render), METH_VARARGS, ""},
		{"defineLayer", reinterpret_cast<PyCFunction>(defineLayer), METH_NOARGS, ""},
		{"enablePrintDateTime", reinterpret_cast<PyCFunction>(enablePrintDateTime), METH_VARARGS, ""},
		{"setConsoleVerbosityLevel", reinterpret_cast<PyCFunction>(setConsoleVerbosityLevel), METH_VARARGS, ""},
		{"setLogVerbosityLevel", reinterpret_cast<PyCFunction>(setLogVerbosityLevel), METH_VARARGS, ""},
		{"logLevelFromString", reinterpret_cast<PyCFunction>(logLevelFromString), METH_VARARGS, ""},
		{"printDebug", reinterpret_cast<PyCFunction>(printDebug), METH_VARARGS, ""},
		{"printVerbose", reinterpret_cast<PyCFunction>(printVerbose), METH_VARARGS, ""},
		{"printInfo", reinterpret_cast<PyCFunction>(printInfo), METH_VARARGS, ""},
		{"printParams", reinterpret_cast<PyCFunction>(printParams), METH_VARARGS, ""},
		{"printWarning", reinterpret_cast<PyCFunction>(printWarning), METH_VARARGS, ""},
		{"printError", reinterpret_cast<PyCFunction>(printError), METH_VARARGS, ""},
		{"cancelRendering", reinterpret_cast<PyCFunction>(cancelRendering), METH_NOARGS, ""},
		{"setInputColorSpace", reinterpret_cast<PyCFunction>(setInputColorSpace), METH_VARARGS, ""},
		{"createImage", reinterpret_cast<PyCFunction>(createImage), METH_VARARGS, ""},
		{"setImageColor", reinterpret_cast<PyCFunction>(setImageColor), METH_VARARGS, ""},
		{"getImageColor", reinterpret_cast<PyCFunction>(getImageColor), METH_VARARGS, ""},
		{"setConsoleLogColorsEnabled", reinterpret_cast<PyCFunction>(setConsoleLogColorsEnabled), METH_VARARGS, ""},
		{"xmlParseFile", reinterpret_cast<PyCFunction>(xmlParseFile), METH_VARARGS, ""},
		{"xmlParseMemory", reinterpret_cast<PyCFunction>(xmlParseMemory), METH_VARARGS, ""},
		{"guiCreateRenderWidget", reinterpret_cast<PyCFunction>(guiCreateRenderWidget), METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
static PyTypeObject YafaRayInterface_Type = {
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.Interface", /* tp_name */
		sizeof(YafaRayInterface), /* tp_basicsize */
		0, /* tp_itemsize */
		reinterpret_cast<destructor>(destroyInterface), /* tp_dealloc */
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
		"libYafaRay Interface object", /* tp_doc */
		nullptr, /* tp_traverse */
		nullptr, /* tp_clear */
		nullptr, /* tp_richcompare */
		0, /* tp_weaklistoffset */
		nullptr, /* tp_iter */
		nullptr, /* tp_iternext */
		yafaray_methods, /* tp_methods */
		nullptr, /* tp_members */
		nullptr, /* tp_getset */
		nullptr, /* tp_base */
		nullptr, /* tp_dict */
		nullptr, /* tp_descr_get */
		nullptr, /* tp_descr_set */
		0, /* tp_dictoffset */
		nullptr, /* tp_init */
		nullptr, /* tp_alloc */
		createInterface, /* tp_new */
};

}

END_YAFARAY_BINDINGS

#endif //YAFARAY_BINDINGS_LIBYAFARAY_PYTHON_H
