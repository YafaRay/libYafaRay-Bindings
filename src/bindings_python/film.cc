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

#include "bindings_python/film.h"
#include "bindings_python/tile.h"
#include "bindings_python/param_map.h"
#include "bindings_python/logger.h"
#include "bindings_python/renderer.h"
#include "common/image.h"

namespace yafaray_bindings::python
{
PyMethodDef Film::py_methods_[]{
		{"create",                    reinterpret_cast<PyCFunction>(create),                    METH_VARARGS, ""},
		{"getWidth",                  reinterpret_cast<PyCFunction>(getWidth),                  METH_NOARGS,  ""},
		{"getHeight",                 reinterpret_cast<PyCFunction>(getHeight),                 METH_NOARGS,  ""},
		{"defineCamera",              reinterpret_cast<PyCFunction>(defineCamera),              METH_VARARGS, ""},
		{"createOutput",              reinterpret_cast<PyCFunction>(createOutput),              METH_VARARGS, ""},
		{"setNotifyLayerCallback",    reinterpret_cast<PyCFunction>(setNotifyLayerCallback),    METH_VARARGS, ""},
		{"setPutPixelCallback",       reinterpret_cast<PyCFunction>(setPutPixelCallback),       METH_VARARGS, ""},
		{"setHighlightPixelCallback", reinterpret_cast<PyCFunction>(setHighlightPixelCallback), METH_VARARGS, ""},
		{"setFlushAreaCallback",      reinterpret_cast<PyCFunction>(setFlushAreaCallback),      METH_VARARGS, ""},
		{"setFlushCallback",          reinterpret_cast<PyCFunction>(setFlushCallback),          METH_VARARGS, ""},
		{"setHighlightAreaCallback",  reinterpret_cast<PyCFunction>(setHighlightAreaCallback),  METH_VARARGS, ""},
		{"defineLayer",               reinterpret_cast<PyCFunction>(defineLayer),               METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject Film::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.Film", /* tp_name */
		sizeof(Film), /* tp_basicsize */
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
		"libYafaRay Film object", /* tp_doc */
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

PyObject *Film::create(PyTypeObject *type, PyObject *args, PyObject *)
{
	Logger *logger{nullptr};
	Renderer *renderer{nullptr};
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "OOsO", &logger, &renderer, &name, &param_map)) Py_RETURN_FALSE;
	auto *self = reinterpret_cast<Film *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->film_ = yafaray_createFilm(logger->get(), renderer->get(), name, param_map->get());
		if(self->film_)
		{
			self->images_collection_ = std::make_unique<ImagesCollection>(yafaray_getFilmWidth(self->film_), yafaray_getFilmHeight(self->film_));
		}
		self->setCallbacks();
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *Film::destroy(Film *self)
{
	yafaray_destroyFilm(self->film_);
	self->film_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

void Film::setCallbacks()
{
	yafaray_setNotifyLayerCallback(film_, Film::notifyLayerCallback, this);
	yafaray_setHighlightAreaCallback(film_, Film::highlightAreaCallback, this);
	yafaray_setHighlightPixelCallback(film_, Film::highlightPixelCallback, this);
	yafaray_setPutPixelCallback(film_, Film::putPixelCallback, this);
	yafaray_setFlushAreaCallback(film_, Film::flushAreaCallback, this);
	yafaray_setFlushCallback(film_, Film::flushCallback, this);
}

PyObject *Film::getWidth(Film *self, PyObject *)
{
	return PyLong_FromLong(yafaray_getFilmWidth(self->film_));
}

PyObject *Film::getHeight(Film *self, PyObject *)
{
	return PyLong_FromLong(yafaray_getFilmHeight(self->film_));
}

PyObject *Film::defineCamera(Film *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "sO", &name, &param_map)) Py_RETURN_FALSE;
	yafaray_defineCamera(self->film_, name, param_map->get());
	Py_RETURN_NONE;
}

PyObject *Film::createOutput(Film *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "sO", &name, &param_map)) Py_RETURN_FALSE;
	yafaray_createOutput(self->film_, name, param_map->get());
	Py_RETURN_NONE;
}

void Film::notifyLayerCallback(const char *internal_layer_name, const char *exported_layer_name, int width, int height, int layer_exported_channels, void *callback_user_data)
{
	auto film = reinterpret_cast<Film *>(callback_user_data);
	if(internal_layer_name)
	{
		film->images_collection_->defineLayer(internal_layer_name, exported_layer_name, width, height, layer_exported_channels);
	}
	if(film->notify_layer_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("siii", internal_layer_name, width, height, layer_exported_channels);
		PyObject_CallObject(film->notify_layer_callback_, arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void Film::putPixelCallback(const char *layer_name, int x, int y, float r, float g, float b, float a, void *callback_user_data)
{
	auto film = reinterpret_cast<Film *>(callback_user_data);
	film->images_collection_->setColor(layer_name, x, y, {r, g, b, a});
	if(film->putpixel_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("siiffff", layer_name, x, y, r, g, b, a);
		PyObject_CallObject(film->putpixel_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void Film::highlightPixelCallback(int x, int y, float r, float g, float b, float a, void *callback_user_data)
{
	auto film = reinterpret_cast<Film *>(callback_user_data);
	const float brightness = RgbaFloat{r, g, b, a}.absCol2Bri();
	RgbaFloat pixel_highlight_color{brightness, 0.7f, brightness, 1.f};
	if(r < g && r < b) pixel_highlight_color = {0.7f, brightness, brightness, 1.f};

	//To highlight the resampled pixels only in the combined layer
	film->images_collection_->setColor("combined", x, y, pixel_highlight_color);
/*
	//To highlight the resampled pixels in all layers
	auto layers_and_channels = yi->images_collection_->getLayersAndExportedChannels();
	for(const auto &layer : layers_and_channels)
	{
		yi->images_collection_->setColor(layer.first, x, y, pixel_highlight_color);
	}
*/
	if(film->highlight_pixel_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject *arglist = Py_BuildValue("iiffff", x, y, r, g, b, a);
		PyObject_CallObject(film->highlight_pixel_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void Film::flushAreaCallback(int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data)
{
	auto film = reinterpret_cast<Film *>(callback_user_data);
	if(film->flush_area_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		const auto layers_and_channels{film->images_collection_->getLayersAndExportedChannels()};
		PyObject *tiles = PyTuple_New(static_cast<int>(layers_and_channels.size()));
		int idx = 0;
		x_0 -= film->border_start_x_;
		y_0 -= film->border_start_y_;
		x_1 -= film->border_start_x_;
		y_1 -= film->border_start_y_;
		for(const auto &layer: layers_and_channels)
		{
			Tile *tile = PyObject_New(Tile, Tile::getType());
			tile->setAreaX0(x_0);
			tile->setAreaY0(y_0);
			tile->setAreaX1(x_1);
			tile->setAreaY1(y_1);
			tile->setImage(film->images_collection_->findLayer(layer.first));
			tile->setExportedChannels(layer.second);
			PyObject *tile_with_layer_name = Py_BuildValue("sO", film->images_collection_->getExportedLayerName(layer.first).c_str(), tile);
			PyTuple_SET_ITEM(tiles, idx, tile_with_layer_name);
			Py_DECREF(tile);
			++idx;
		}
		const int height = yafaray_getFilmHeight(film->get());
		PyObject *arglist = Py_BuildValue("iiiiiO", area_id, x_0, height - y_1, x_1, height - y_0, tiles);
		PyObject_CallObject(film->flush_area_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void Film::flushCallback(void *callback_user_data)
{
	auto film = reinterpret_cast<Film *>(callback_user_data);
	if(film->flush_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		const int width = yafaray_getFilmWidth(film->get());
		const int height = yafaray_getFilmHeight(film->get());

		const auto layers_and_channels{film->images_collection_->getLayersAndExportedChannels()};
		PyObject *tiles = PyTuple_New(static_cast<int>(layers_and_channels.size()));
		int idx = 0;
		for(const auto &layer: layers_and_channels)
		{
			Tile *tile = PyObject_New(Tile, Tile::getType());
			tile->setAreaX0(0);
			tile->setAreaY0(0);
			tile->setAreaX1(width);
			tile->setAreaY1(height);
			tile->setImage(film->images_collection_->findLayer(layer.first));
			tile->setExportedChannels(layer.second);
			PyObject *tile_with_layer_name = Py_BuildValue("sO", film->images_collection_->getExportedLayerName(layer.first).c_str(), tile);
			PyTuple_SET_ITEM(tiles, idx, tile_with_layer_name);
			Py_DECREF(tile);
			++idx;
		}
		PyObject *arglist = Py_BuildValue("iiO", width, height, tiles);
		PyObject_CallObject(film->flush_callback_, arglist);
		Py_DECREF(arglist);
		Py_DECREF(tiles);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

void Film::highlightAreaCallback(int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data)
{
	auto film = reinterpret_cast<Film *>(callback_user_data);
	if(film->highlight_callback_)
	{
		//Starting python multi-thread API calling block. If this is not done weird crashes happen when calling python callbacks from multiple C/C++ threads
		PyGILState_STATE gstate = PyGILState_Ensure();

		const int width = yafaray_getFilmWidth(film->get());
		const int height = yafaray_getFilmHeight(film->get());
		const int corner_line_length = std::min(4, std::min(height - 1, width - 1));

		const auto layers_and_channels{film->images_collection_->getLayersAndExportedChannels()};
		PyObject *tiles = PyTuple_New(static_cast<int>(layers_and_channels.size()));
		int idx = 0;
		x_0 -= film->border_start_x_;
		y_0 -= film->border_start_y_;
		x_1 -= film->border_start_x_;
		y_1 -= film->border_start_y_;
		for(const auto &layer: layers_and_channels)
		{
			Tile *tile = PyObject_New(Tile, Tile::getType());
			tile->setAreaX0(x_0);
			tile->setAreaY0(y_0);
			tile->setAreaX1(x_1);
			tile->setAreaY1(y_1);
			tile->setImage(film->images_collection_->findLayer(layer.first));
			tile->setExportedChannels(layer.second);
			tile->drawCorner(x_0, y_0, corner_line_length, Tile::CornerPosition::TopLeft);
			tile->drawCorner(x_1, y_0, corner_line_length, Tile::CornerPosition::TopRight);
			tile->drawCorner(x_0, y_1, corner_line_length, Tile::CornerPosition::BottomLeft);
			tile->drawCorner(x_1, y_1, corner_line_length, Tile::CornerPosition::BottomRight);
			PyObject *tile_with_layer_name = Py_BuildValue("sO", film->images_collection_->getExportedLayerName(layer.first).c_str(), tile);
			PyTuple_SET_ITEM(tiles, idx, tile_with_layer_name);
			Py_DECREF(tile);
			++idx;
		}
		PyObject *arglist = Py_BuildValue("iiiiiO", area_id, x_0, height - y_1, x_1, height - y_0, tiles);
		PyObject_CallObject(film->highlight_callback_, arglist);
		Py_DECREF(arglist);

		//Ending python multi-thread API calling block
		PyGILState_Release(gstate);
	}
}

PyObject *Film::setNotifyLayerCallback(Film *self, PyObject *args)
{
	PyObject *callback;

	if(PyArg_ParseTuple(args, "O:notifyLayerCallback", &callback))
	{
		PyObject *result = setCallback(self->notify_layer_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *Film::setPutPixelCallback(Film *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:putPixelCallback", &callback))
	{
		PyObject *result = setCallback(self->putpixel_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *Film::setHighlightPixelCallback(Film *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:highlightPixelCallback", &callback))
	{
		PyObject *result = setCallback(self->highlight_pixel_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *Film::setFlushAreaCallback(Film *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:flushAreaCallback", &callback))
	{
		PyObject *result = setCallback(self->flush_area_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *Film::setFlushCallback(Film *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:flushCallback", &callback))
	{
		PyObject *result = setCallback(self->flush_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *Film::setHighlightAreaCallback(Film *self, PyObject *args)
{
	PyObject *callback;
	if(PyArg_ParseTuple(args, "O:highlightAreaCallback", &callback))
	{
		PyObject *result = setCallback(self->highlight_callback_, callback);
		if(result == Py_False) Py_RETURN_FALSE;
	}
	Py_RETURN_NONE;
}

PyObject *Film::defineLayer(Film *self, PyObject *args)
{
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "O", &param_map)) Py_RETURN_FALSE;
	yafaray_defineLayer(self->film_, param_map->get());
	Py_RETURN_NONE;
}

} // namespace yafaray_bindings::python

