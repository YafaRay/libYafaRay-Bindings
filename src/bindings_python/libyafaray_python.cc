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

#include "bindings_python/libyafaray_python.h"
#include "common/image.h"
#include "bindings_python/tile.h"
#ifdef YAFARAY_BINDINGS_WITH_XML
#include <yafaray_xml_c_api.h>
#endif //YAFARAY_BINDINGS_WITH_XML
#ifdef YAFARAY_BINDINGS_WITH_GUI
#include <yafaray_gui_c_api.h>
#endif //YAFARAY_BINDINGS_WITH_GUI
#include <cstring>

BEGIN_YAFARAY_BINDINGS

namespace python
{

PyObject *createInterface(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	auto *self = reinterpret_cast<YafaRayInterface *>(type->tp_alloc(type, 0));
	if(self)
	{
		const char *exported_file_path = nullptr;
		if(!PyArg_ParseTuple(args, "|s", &exported_file_path)) Py_RETURN_FALSE;
		std::string file_path;
		std::string file_extension;
		if(exported_file_path)
		{
			file_path = exported_file_path;
			const size_t extension_dot_position = file_path.find_last_of('.');
			file_extension = file_path.substr(extension_dot_position + 1, file_path.size() - (extension_dot_position + 1));
		}
		if(file_extension == "xml")
		{
			self->interf_ = yafaray_createInterface(YAFARAY_INTERFACE_EXPORT_XML, exported_file_path, nullptr, nullptr, YAFARAY_DISPLAY_CONSOLE_NORMAL);
		}
		else if(file_extension == "c")
		{
			self->interf_ = yafaray_createInterface(YAFARAY_INTERFACE_EXPORT_C, exported_file_path, nullptr, nullptr, YAFARAY_DISPLAY_CONSOLE_NORMAL);
		}
		else if(file_extension == "py")
		{
			self->interf_ = yafaray_createInterface(YAFARAY_INTERFACE_EXPORT_PYTHON, exported_file_path, nullptr, nullptr, YAFARAY_DISPLAY_CONSOLE_NORMAL);
		}
		else
		{
			self->interf_ = yafaray_createInterface(YAFARAY_INTERFACE_FOR_RENDERING, nullptr, nullptr, nullptr, YAFARAY_DISPLAY_CONSOLE_NORMAL);
		}
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *destroyInterface(YafaRayInterface *self)
{
	yafaray_destroyInterface(self->interf_);
	self->interf_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

void YafaRayInterface::setupInternalFilmCallbacks()
{
	yafaray_setRenderNotifyViewCallback(this->interf_, YafaRayInterface::notifyViewCallback, this);
	yafaray_setRenderNotifyLayerCallback(this->interf_, YafaRayInterface::notifyLayerCallback, this);
	yafaray_setRenderHighlightAreaCallback(this->interf_, YafaRayInterface::highlightAreaCallback, this);
	yafaray_setRenderHighlightPixelCallback(this->interf_, YafaRayInterface::highlightPixelCallback, this);
	yafaray_setRenderPutPixelCallback(this->interf_, YafaRayInterface::putPixelCallback, this);
	yafaray_setRenderFlushAreaCallback(this->interf_, YafaRayInterface::flushAreaCallback, this);
	yafaray_setRenderFlushCallback(this->interf_, YafaRayInterface::flushCallback, this);
}

void YafaRayInterface::loggerCallback(yafaray_LogLevel_t log_level, long datetime, const char *time_of_day, const char *description, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	if(yi->python_logging_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("ilss", log_level, datetime, time_of_day, description);
		PyObject_CallObject(yi->python_logging_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

PyObject *setLoggingCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:loggingCallback", &callback))
	{
		if (!PyCallable_Check(callback))
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			Py_RETURN_FALSE;
		}
		Py_XINCREF(callback);         /* Add a reference to new callback */
		Py_XDECREF(self->python_logging_callback_);  /* Dispose of previous callback */
		self->python_logging_callback_ = callback;       /* Remember new callback */
		yafaray_setLoggingCallback(self->interf_, YafaRayInterface::loggerCallback, self);
	}
	Py_RETURN_NONE;
}

PyObject *createScene(YafaRayInterface *self, PyObject *)
{
	yafaray_createScene(self->interf_);
	Py_RETURN_NONE;
}

PyObject *getSceneFilmWidth(YafaRayInterface *self, PyObject *)
{
	return PyLong_FromLong(yafaray_getSceneFilmWidth(self->interf_));
}

PyObject *getSceneFilmHeight(YafaRayInterface *self, PyObject *)
{
	return PyLong_FromLong(yafaray_getSceneFilmHeight(self->interf_));
}

PyObject *startGeometry(YafaRayInterface *self, PyObject *)
{
	yafaray_startGeometry(self->interf_);
	Py_RETURN_NONE;
}

PyObject *endGeometry(YafaRayInterface *self, PyObject *)
{
	yafaray_endGeometry(self->interf_);
	Py_RETURN_NONE;
}

PyObject *getNextFreeId(YafaRayInterface *self, PyObject *)
{
	return PyLong_FromLong(yafaray_getNextFreeId(self->interf_));
}

PyObject *endObject(YafaRayInterface *self, PyObject *)
{
	yafaray_endObject(self->interf_);
	Py_RETURN_NONE;
}

PyObject *addVertex(YafaRayInterface *self, PyObject *args)
{
	float x, y, z;
	if(!PyArg_ParseTuple(args, "fff", &x, &y, &z)) Py_RETURN_FALSE;
	yafaray_addVertex(self->interf_, x, y, z);
	Py_RETURN_NONE;
}

PyObject *addVertexTimeStep(YafaRayInterface *self, PyObject *args)
{
	float x, y, z;
	int time_step;
	if(!PyArg_ParseTuple(args, "fffi", &x, &y, &z, &time_step)) Py_RETURN_FALSE;
	yafaray_addVertexTimeStep(self->interf_, x, y, z, time_step);
	Py_RETURN_NONE;
}

PyObject *addVertexWithOrco(YafaRayInterface *self, PyObject *args)
{
	float x, y, z, ox, oy, oz;
	if(!PyArg_ParseTuple(args, "ffffff", &x, &y, &z, &ox, &oy, &oz)) Py_RETURN_FALSE;
	yafaray_addVertexWithOrco(self->interf_, x, y, z, ox, oy, oz);
	Py_RETURN_NONE;
}

PyObject *addVertexWithOrcoTimeStep(YafaRayInterface *self, PyObject *args)
{
	float x, y, z, ox, oy, oz;
	int time_step;
	if(!PyArg_ParseTuple(args, "ffffffi", &x, &y, &z, &ox, &oy, &oz, &time_step)) Py_RETURN_FALSE;
	yafaray_addVertexWithOrcoTimeStep(self->interf_, x, y, z, ox, oy, oz, time_step);
	Py_RETURN_NONE;
}

PyObject *addNormal(YafaRayInterface *self, PyObject *args)
{
	float x, y, z;
	if(!PyArg_ParseTuple(args, "fff", &x, &y, &z)) Py_RETURN_FALSE;
	yafaray_addNormal(self->interf_, x, y, z);
	Py_RETURN_NONE;
}

PyObject *addNormalTimeStep(YafaRayInterface *self, PyObject *args)
{
	float x, y, z;
	int time_step;
	if(!PyArg_ParseTuple(args, "fffi", &x, &y, &z, &time_step)) Py_RETURN_FALSE;
	yafaray_addNormalTimeStep(self->interf_, x, y, z, time_step);
	Py_RETURN_NONE;
}

PyObject *addTriangle(YafaRayInterface *self, PyObject *args)
{
	int a, b, c;
	if(!PyArg_ParseTuple(args, "iii", &a, &b, &c)) Py_RETURN_FALSE;
	yafaray_addTriangle(self->interf_, a, b, c);
	Py_RETURN_NONE;
}

PyObject *addTriangleWithUv(YafaRayInterface *self, PyObject *args)
{
	int a, b, c, uv_a, uv_b, uv_c;
	if(!PyArg_ParseTuple(args, "iiiiii", &a, &b, &c, &uv_a, &uv_b, &uv_c)) Py_RETURN_FALSE;
	yafaray_addTriangleWithUv(self->interf_, a, b, c, uv_a, uv_b, uv_c);
	Py_RETURN_NONE;
}

PyObject *addQuad(YafaRayInterface *self, PyObject *args)
{
	int a, b, c, d;
	if(!PyArg_ParseTuple(args, "iiii", &a, &b, &c, &d)) Py_RETURN_FALSE;
	yafaray_addQuad(self->interf_, a, b, c, d);
	Py_RETURN_NONE;
}

PyObject *addQuadWithUv(YafaRayInterface *self, PyObject *args)
{
	int a, b, c, d, uv_a, uv_b, uv_c, uv_d;
	if(!PyArg_ParseTuple(args, "iiiiiiii", &a, &b, &c, &d, &uv_a, &uv_b, &uv_c, &uv_d)) Py_RETURN_FALSE;
	yafaray_addQuadWithUv(self->interf_, a, b, c, d, uv_a, uv_b, uv_c, uv_d);
	Py_RETURN_NONE;
}

PyObject *addUv(YafaRayInterface *self, PyObject *args)
{
	float u, v;
	if(!PyArg_ParseTuple(args, "ff", &u, &v)) Py_RETURN_FALSE;
	return PyLong_FromLong(yafaray_addUv(self->interf_, u, v));
}

PyObject *smoothMesh(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	float angle;
	if(!PyArg_ParseTuple(args, "sf", &str, &angle)) Py_RETURN_FALSE;
	yafaray_smoothMesh(self->interf_, str, angle);
	Py_RETURN_NONE;
}

PyObject *createInstance(YafaRayInterface *self, PyObject *)
{
	return PyLong_FromLong(yafaray_createInstance(self->interf_));
}

PyObject *addInstanceObject(YafaRayInterface *self, PyObject *args)
{
	int instance_id;
	const char *base_object_name;
	if(!PyArg_ParseTuple(args, "is", &instance_id, &base_object_name)) Py_RETURN_FALSE;
	yafaray_addInstanceObject(self->interf_, instance_id, base_object_name);
	Py_RETURN_NONE;
}

PyObject *addInstanceOfInstance(YafaRayInterface *self, PyObject *args)
{
	int instance_id;
	int base_instance_id;
	if(!PyArg_ParseTuple(args, "ii", &instance_id, &base_instance_id)) Py_RETURN_FALSE;
	yafaray_addInstanceOfInstance(self->interf_, instance_id, base_instance_id);
	Py_RETURN_NONE;
}

PyObject *addInstanceMatrix(YafaRayInterface *self, PyObject *args)
{
	int instance_id;
	float obj_to_world[4][4];
	float time;
	if(!PyArg_ParseTuple(args, "ifffffffffffffffff", &instance_id,
		&obj_to_world[0][0], &obj_to_world[0][1], &obj_to_world[0][2], &obj_to_world[0][3],
		&obj_to_world[1][0], &obj_to_world[1][1], &obj_to_world[1][2], &obj_to_world[1][3],
		&obj_to_world[2][0], &obj_to_world[2][1], &obj_to_world[2][2], &obj_to_world[2][3],
		&obj_to_world[3][0], &obj_to_world[3][1], &obj_to_world[3][2], &obj_to_world[3][3],
		&time
	)) Py_RETURN_FALSE;
	yafaray_addInstanceMatrixArray(self->interf_, instance_id, reinterpret_cast<const float *>(obj_to_world), time);
	Py_RETURN_NONE;
}

PyObject *paramsSetVector(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	float x, y, z;
	if(!PyArg_ParseTuple(args, "sfff", &str, &x, &y, &z)) Py_RETURN_FALSE;
	yafaray_paramsSetVector(self->interf_, str, x, y, z);
	Py_RETURN_NONE;
}

PyObject *paramsSetString(YafaRayInterface *self, PyObject *args)
{
	const char *str, *str_val;
	if(!PyArg_ParseTuple(args, "ss", &str, &str_val)) Py_RETURN_FALSE;
	yafaray_paramsSetString(self->interf_, str, str_val);
	Py_RETURN_NONE;
}

PyObject *paramsSetBool(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	int val;
	if(!PyArg_ParseTuple(args, "sp", &str, &val)) Py_RETURN_FALSE;
	yafaray_paramsSetBool(self->interf_, str, static_cast<yafaray_bool_t>(val));
	Py_RETURN_NONE;
}

PyObject *paramsSetInt(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	int val;
	if(!PyArg_ParseTuple(args, "si", &str, &val)) Py_RETURN_FALSE;
	yafaray_paramsSetInt(self->interf_, str, val);
	Py_RETURN_NONE;
}

PyObject *paramsSetFloat(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	float val;
	if(!PyArg_ParseTuple(args, "sf", &str, &val)) Py_RETURN_FALSE;
	yafaray_paramsSetFloat(self->interf_, str, val);
	Py_RETURN_NONE;
}

PyObject *paramsSetColor(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	float r, g, b, a = 1.f;
	if(!PyArg_ParseTuple(args, "sfff|f", &str, &r, &g, &b, &a)) Py_RETURN_FALSE;
	yafaray_paramsSetColor(self->interf_, str, r, g, b, a);
	Py_RETURN_NONE;
}

PyObject *paramsSetMatrix(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	float matrix[4][4];
	int transpose = YAFARAY_BOOL_FALSE;
	if(!PyArg_ParseTuple(args, "sffffffffffffffff|p", &str,
						 &matrix[0][0], &matrix[0][1], &matrix[0][2], &matrix[0][3],
						 &matrix[1][0], &matrix[1][1], &matrix[1][2], &matrix[1][3],
						 &matrix[2][0], &matrix[2][1], &matrix[2][2], &matrix[2][3],
						 &matrix[3][0], &matrix[3][1], &matrix[3][2], &matrix[3][3],
						 &transpose
	)) Py_RETURN_FALSE;
	yafaray_paramsSetMatrixArray(self->interf_, str, reinterpret_cast<const float *>(matrix), static_cast<yafaray_bool_t>(transpose));
	Py_RETURN_NONE;
}

PyObject *paramsClearAll(YafaRayInterface *self, PyObject *)
{
	yafaray_paramsClearAll(self->interf_);
	Py_RETURN_NONE;
}

PyObject *paramsPushList(YafaRayInterface *self, PyObject *)
{
	yafaray_paramsPushList(self->interf_);
	Py_RETURN_NONE;
}

PyObject *paramsEndList(YafaRayInterface *self, PyObject *)
{
	yafaray_paramsEndList(self->interf_);
	Py_RETURN_NONE;
}

PyObject *setCurrentMaterial(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_setCurrentMaterial(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createObject(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createObject(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createLight(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createLight(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createTexture(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createTexture(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createMaterial(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createMaterial(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createCamera(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createCamera(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createBackground(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createBackground(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createIntegrator(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createIntegrator(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createVolumeRegion(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createVolumeRegion(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createRenderView(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createRenderView(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *createOutput(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_createOutput(self->interf_, str);
	Py_RETURN_NONE;
}

void YafaRayInterface::notifyViewCallback(const char *view_name, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	if(view_name)
	{
		yi->images_collection_->defineView(view_name);
	}
	if(yi->python_notify_view_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		//Note: use parenthesis in the format, for example Py_BuildValue("(s)", ...) to force a tuple with only one element, otherwise a weird error happens when calling the callback (it complains about not using a tuple)
		PyObject *arglist = Py_BuildValue("(s)", view_name);
		PyObject_CallObject(yi->python_notify_view_callback_, arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void YafaRayInterface::notifyLayerCallback(const char *internal_layer_name, const char *exported_layer_name, int width, int height, int layer_exported_channels, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	if(internal_layer_name)
	{
		yi->images_collection_->defineLayer(internal_layer_name, exported_layer_name, width, height, layer_exported_channels);
	}
	if(yi->python_notify_layer_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("siii", internal_layer_name, width, height, layer_exported_channels);
		PyObject_CallObject(yi->python_notify_layer_callback_, arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void YafaRayInterface::putPixelCallback(const char *view_name, const char *layer_name, int x, int y, float r, float g, float b, float a, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	yi->images_collection_->setColor(view_name, layer_name, x, y, {r, g, b, a});
	if(yi->python_putpixel_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("ssiiffff", view_name, layer_name, x, y, r, g, b, a);
		PyObject_CallObject(yi->python_putpixel_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void YafaRayInterface::highlightPixelCallback(const char *view_name, int x, int y, float r, float g, float b, float a, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	const float brightness = RgbaFloat{r, g, b, a}.absCol2Bri();
	RgbaFloat pixel_highlight_color{brightness, 0.7f, brightness, 1.f};
	if(r < g && r < b) pixel_highlight_color = {0.7f, brightness, brightness, 1.f};

	//To highlight the resampled pixels only in the combined layer
	yi->images_collection_->setColor(view_name, "combined", x, y, pixel_highlight_color);
/*
	//To highlight the resampled pixels in all layers
	auto layers_and_channels = yi->images_collection_->getLayersAndExportedChannels();
	for(const auto &layer : layers_and_channels)
	{
		yi->images_collection_->setColor(view_name, layer.first, x, y, pixel_highlight_color);
	}
*/
	if(yi->python_highlight_pixel_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("siiffff", view_name, x, y, r, g, b, a);
		PyObject_CallObject(yi->python_highlight_pixel_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void YafaRayInterface::flushAreaCallback(const char *view_name, int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	if(yi->python_flush_area_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		auto layers_and_channels = yi->images_collection_->getLayersAndExportedChannels();
		PyObject *tiles = PyTuple_New(layers_and_channels.size());
		int idx = 0;
		x_0 -= yi->border_start_x_;
		y_0 -= yi->border_start_y_;
		x_1 -= yi->border_start_x_;
		y_1 -= yi->border_start_y_;
		for(const auto &layer : layers_and_channels)
		{
			Tile *tile = PyObject_New(Tile, &python_tile_type_global);
			tile->area_x_0_ = x_0;
			tile->area_y_0_ = y_0;
			tile->area_x_1_ = x_1;
			tile->area_y_1_ = y_1;
			tile->image_ = yi->images_collection_->findLayer(view_name, layer.first);
			tile->exported_channels_ = layer.second;
			PyObject* tile_with_layer_name = Py_BuildValue("sO", yi->images_collection_->getExportedLayerName(layer.first).c_str(), tile);
			PyTuple_SET_ITEM(tiles, idx, tile_with_layer_name);
			Py_DECREF(tile);
			++idx;
		}
		const int height = yafaray_getSceneFilmHeight(yi->interf_);
		PyObject *arglist = Py_BuildValue("siiiiiO", view_name, area_id, x_0, height - y_1, x_1, height - y_0, tiles);
		PyObject_CallObject(yi->python_flush_area_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void YafaRayInterface::flushCallback(const char *view_name, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	if(yi->python_flush_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		const int width = yafaray_getSceneFilmWidth(yi->interf_);
		const int height = yafaray_getSceneFilmHeight(yi->interf_);

		auto layers_and_channels = yi->images_collection_->getLayersAndExportedChannels();
		PyObject *tiles = PyTuple_New(layers_and_channels.size());
		int idx = 0;
		for(const auto &layer : layers_and_channels)
		{
			Tile *tile = PyObject_New(Tile, &python_tile_type_global);
			tile->area_x_0_ = 0;
			tile->area_y_0_ = 0;
			tile->area_x_1_ = width;
			tile->area_y_1_ = height;
			tile->image_ = yi->images_collection_->findLayer(view_name, layer.first);
			tile->exported_channels_ = layer.second;
			PyObject* tile_with_layer_name = Py_BuildValue("sO", yi->images_collection_->getExportedLayerName(layer.first).c_str(), tile);
			PyTuple_SET_ITEM(tiles, idx, tile_with_layer_name);
			Py_DECREF(tile);
			++idx;
		}
		PyObject *arglist = Py_BuildValue("siiO", view_name, width, height, tiles);
		PyObject_CallObject(yi->python_flush_callback_, arglist);
		Py_DECREF(arglist);
		Py_DECREF(tiles);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void YafaRayInterface::highlightAreaCallback(const char *view_name, int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	if(yi->python_highlight_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		const int width = yafaray_getSceneFilmWidth(yi->interf_);
		const int height = yafaray_getSceneFilmHeight(yi->interf_);
		const int corner_line_length = std::min(4, std::min(height - 1, width - 1 ));

		auto layers_and_channels = yi->images_collection_->getLayersAndExportedChannels();
		PyObject *tiles = PyTuple_New(layers_and_channels.size());
		int idx = 0;
		x_0 -= yi->border_start_x_;
		y_0 -= yi->border_start_y_;
		x_1 -= yi->border_start_x_;
		y_1 -= yi->border_start_y_;
		for(const auto &layer : layers_and_channels)
		{
			Tile *tile = PyObject_New(Tile, &python_tile_type_global);
			tile->area_x_0_ = x_0;
			tile->area_y_0_ = y_0;
			tile->area_x_1_ = x_1;
			tile->area_y_1_ = y_1;
			tile->image_ = yi->images_collection_->findLayer(view_name, layer.first);
			tile->exported_channels_ = layer.second;
			tile->drawCorner(x_0, y_0, corner_line_length, Tile::CornerPosition::TopLeft);
			tile->drawCorner(x_1, y_0, corner_line_length, Tile::CornerPosition::TopRight);
			tile->drawCorner(x_0, y_1, corner_line_length, Tile::CornerPosition::BottomLeft);
			tile->drawCorner(x_1, y_1, corner_line_length, Tile::CornerPosition::BottomRight);
			PyObject* tile_with_layer_name = Py_BuildValue("sO", yi->images_collection_->getExportedLayerName(layer.first).c_str(), tile);
			PyTuple_SET_ITEM(tiles, idx, tile_with_layer_name);
			Py_DECREF(tile);
			++idx;
		}
		PyObject *arglist = Py_BuildValue("siiiiiO", view_name, area_id, x_0, height - y_1, x_1, height - y_0, tiles);
		PyObject_CallObject(yi->python_highlight_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

PyObject *setPythonRenderCallback(PyObject *&yafaray_interface_python_callback, PyObject *python_callback)
{
	if(python_callback == Py_None)
	{
		Py_XDECREF(yafaray_interface_python_callback);  /* Dispose of previous callback */
		yafaray_interface_python_callback = nullptr;
	}
	else
	{
		if(!PyCallable_Check(python_callback))
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			Py_RETURN_FALSE;
		}
		Py_XINCREF(python_callback);         /* Add a reference to new callback */
		Py_XDECREF(yafaray_interface_python_callback);  /* Dispose of previous callback */
		yafaray_interface_python_callback = python_callback;       /* Remember new callback */
	}
	Py_RETURN_NONE;
}

PyObject *setRenderNotifyViewCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;

	if(PyArg_ParseTuple(args, "O:notifyViewCallback", &callback))
	{
		PyObject *result = setPythonRenderCallback(self->python_notify_view_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *setRenderNotifyLayerCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;

	if(PyArg_ParseTuple(args, "O:notifyLayerCallback", &callback))
	{
		PyObject *result = setPythonRenderCallback(self->python_notify_layer_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *setRenderPutPixelCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:putPixelCallback", &callback))
	{
		PyObject *result = setPythonRenderCallback(self->python_putpixel_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *setRenderHighlightPixelCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:highlightPixelCallback", &callback))
	{
		PyObject *result = setPythonRenderCallback(self->python_highlight_pixel_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *setRenderFlushAreaCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:flushAreaCallback", &callback))
	{
		PyObject *result = setPythonRenderCallback(self->python_flush_area_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *setRenderFlushCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:flushCallback", &callback))
	{
		PyObject *result = setPythonRenderCallback(self->python_flush_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *setRenderHighlightAreaCallback(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:highlightAreaCallback", &callback))
	{
		PyObject *result = setPythonRenderCallback(self->python_highlight_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *removeOutput(YafaRayInterface *self, PyObject *args)
{
	const char *str;
	if(!PyArg_ParseTuple(args, "s", &str)) Py_RETURN_FALSE;
	yafaray_removeOutput(self->interf_, str);
	Py_RETURN_NONE;
}

PyObject *clearOutputs(YafaRayInterface *self, PyObject *)
{
	yafaray_clearOutputs(self->interf_);
	Py_RETURN_NONE;
}

PyObject *clearAll(YafaRayInterface *self, PyObject *)
{
	yafaray_clearAll(self->interf_);
	Py_RETURN_NONE;
}

PyObject *setupRender(YafaRayInterface *self, PyObject *)
{
	self->setupInternalFilmCallbacks();
	yafaray_setupRender(self->interf_);
	Py_RETURN_NONE;
}

void YafaRayInterface::monitorCallback(int steps_total, int steps_done, const char *tag, void *callback_user_data)
{
	auto yi = reinterpret_cast<YafaRayInterface *>(callback_user_data);
	if(yi->python_monitor_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("iis", steps_total, steps_done, tag);
		PyObject_CallObject(yi->python_monitor_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

PyObject *render(YafaRayInterface *self, PyObject *args)
{
	PyObject *callback = nullptr;
	if(PyArg_ParseTuple(args, "ii|O:monitorCallback", &self->border_start_x_, &self->border_start_y_, &callback))
	{
		const int w = yafaray_getSceneFilmWidth(self->interf_);
		const int h = yafaray_getSceneFilmHeight(self->interf_);
		self->images_collection_ = new ImagesCollection(w, h);

		//Save Python main thread state so the Python GIL is released and can be used in the render threads
		PyThreadState *save_thread = PyEval_SaveThread();

		if(!callback)
		{
			yafaray_render(self->interf_, nullptr, nullptr, YAFARAY_DISPLAY_CONSOLE_NORMAL);
		}
		else
		{
			if(!PyCallable_Check(callback))
			{
				PyErr_SetString(PyExc_TypeError, "parameter must be callable");
				Py_RETURN_FALSE;
			}
			Py_XINCREF(callback);         /* Add a reference to new callback */
			Py_XDECREF(self->python_monitor_callback_);  /* Dispose of previous callback */
			self->python_monitor_callback_ = callback;       /* Remember new callback */
			yafaray_render(self->interf_, YafaRayInterface::monitorCallback, self, YAFARAY_DISPLAY_CONSOLE_HIDDEN);
		}

		//Restore Python main thread state so the main thread retrieves the Python GIL
		PyEval_RestoreThread(save_thread);

		delete self->images_collection_;
		self->images_collection_ = nullptr;
	}
	Py_RETURN_NONE;
}

PyObject *defineLayer(YafaRayInterface *self, PyObject *)
{
	yafaray_defineLayer(self->interf_);
	Py_RETURN_NONE;
}

PyObject *enablePrintDateTime(YafaRayInterface *self, PyObject *args)
{
	int enable_print_date_time;
	if(!PyArg_ParseTuple(args, "p", &enable_print_date_time)) Py_RETURN_FALSE;
	yafaray_enablePrintDateTime(self->interf_, static_cast<yafaray_bool_t>(enable_print_date_time));
	Py_RETURN_NONE;
}

PyObject *setConsoleVerbosityLevel(YafaRayInterface *self, PyObject *args)
{
	int log_level;
	if(PyArg_ParseTuple(args, "i", &log_level)) yafaray_setConsoleVerbosityLevel(self->interf_, static_cast<yafaray_LogLevel_t>(log_level));
	Py_RETURN_NONE;
}

PyObject *setLogVerbosityLevel(YafaRayInterface *self, PyObject *args)
{
	int log_level;
	if(PyArg_ParseTuple(args, "i", &log_level)) yafaray_setLogVerbosityLevel(self->interf_, static_cast<yafaray_LogLevel_t>(log_level));
	Py_RETURN_NONE;
}

PyObject *logLevelFromString(YafaRayInterface *, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) return PyLong_FromLong(YAFARAY_LOG_LEVEL_INFO);
	else return PyLong_FromLong(yafaray_logLevelFromString(text));
}

PyObject *printDebug(YafaRayInterface *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printDebug(self->interf_, text);
	Py_RETURN_NONE;
}

PyObject *printVerbose(YafaRayInterface *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printVerbose(self->interf_, text);
	Py_RETURN_NONE;
}

PyObject *printInfo(YafaRayInterface *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printInfo(self->interf_, text);
	Py_RETURN_NONE;
}

PyObject *printParams(YafaRayInterface *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printParams(self->interf_, text);
	Py_RETURN_NONE;
}

PyObject *printWarning(YafaRayInterface *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printWarning(self->interf_, text);
	Py_RETURN_NONE;
}

PyObject *printError(YafaRayInterface *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_printError(self->interf_, text);
	Py_RETURN_NONE;
}

PyObject *cancelRendering(YafaRayInterface *self, PyObject *)
{
	yafaray_cancelRendering(self->interf_);
	Py_RETURN_NONE;
}

PyObject *setInputColorSpace(YafaRayInterface *self, PyObject *args)
{
	const char *color_space;
	float gamma;
	if(!PyArg_ParseTuple(args, "sf", &color_space, &gamma)) Py_RETURN_FALSE;
	yafaray_setInputColorSpace(self->interf_, color_space, gamma);
	Py_RETURN_NONE;
}

PyObject *createImage(YafaRayInterface *self, PyObject *args)
{
	const char *text;
	if(!PyArg_ParseTuple(args, "s", &text)) Py_RETURN_FALSE;
	yafaray_Image *image = yafaray_createImage(self->interf_, text);
	if(image) return PyCapsule_New(static_cast<void *>(image), "yafaray.Image", nullptr);
	else Py_RETURN_FALSE;
}

PyObject *setImageColor(YafaRayInterface *self, PyObject *args)
{
	PyObject *image_object;
	int x, y;
	float r, g, b, a;
	if(!PyArg_ParseTuple(args, "Oiiffff", &image_object, &x, &y, &r, &g, &b, &a)) Py_RETURN_FALSE;
	auto image = static_cast<yafaray_Image_t *>(PyCapsule_GetPointer(image_object, "yafaray.Image"));
	yafaray_setImageColor(image, x, y, r, g, b, a);
	Py_RETURN_NONE;
}

PyObject *getImageColor(YafaRayInterface *self, PyObject *args)
{
	PyObject *image_object;
	int x, y;
	if(!PyArg_ParseTuple(args, "Oii", &image_object, &x, &y)) Py_RETURN_FALSE;
	auto image = static_cast<yafaray_Image_t *>(PyCapsule_GetPointer(image_object, "yafaray.Image"));
	float r, g, b, a;
	yafaray_getImageColor(image, x, y, &r, &g, &b, &a);
	return Py_BuildValue("ffff", r, g, b, a);
}

PyObject *setConsoleLogColorsEnabled(YafaRayInterface *self, PyObject *args)
{
	int colors_enabled;
	if(!PyArg_ParseTuple(args, "p", &colors_enabled)) Py_RETURN_FALSE;
	yafaray_setConsoleLogColorsEnabled(self->interf_, static_cast<yafaray_bool_t>(colors_enabled));
	Py_RETURN_NONE;
}


PyObject *xmlParseFile(YafaRayInterface *self, PyObject *args)
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

PyObject *xmlParseMemory(YafaRayInterface *self, PyObject *args)
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

PyObject *guiCreateRenderWidget(YafaRayInterface *self, PyObject *args)
{
#ifdef YAFARAY_BINDINGS_WITH_GUI
	yafaray_gui_GuiToolKit_t gui_tool_kit = YAFARAY_GUI_QT;
	int width = 400, height = 300, border_start_x = 0, border_start_y = 0;
	yafaray_bool_t auto_render = YAFARAY_BOOL_FALSE, close_after_finish = YAFARAY_BOOL_FALSE;
	if(!PyArg_ParseTuple(args, "|iiiippi", &width, &height, &border_start_x, &border_start_y, &auto_render, &close_after_finish, &gui_tool_kit)) Py_RETURN_FALSE;
	yafaray_gui_createRenderWidget(self->interf_, gui_tool_kit, width, height, border_start_x, border_start_y, auto_render, close_after_finish);
	Py_RETURN_NONE;
#else //YAFARAY_BINDINGS_WITH_GUI
	yafaray_printError(self->interf_, "YafaRay Python Bindings not built with libYafaRay-Gui support, unable to create render widget.");
	Py_RETURN_FALSE;
#endif //YAFARAY_BINDINGS_WITH_GUI
}

}

END_YAFARAY_BINDINGS

