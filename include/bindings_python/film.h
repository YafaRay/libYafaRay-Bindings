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

#ifndef LIBYAFARAY_BINDINGS_FILM_H
#define LIBYAFARAY_BINDINGS_FILM_H

#include "common.h"
#include "common/images_collection.h"

typedef struct yafaray_Film yafaray_Film;

namespace yafaray_bindings::python
{
class Film final // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	public:
		[[nodiscard]] yafaray_Film *get() { return film_; }
		[[nodiscard]] const yafaray_Film *get() const { return film_; }
		[[nodiscard]] static PyTypeObject *getType() { return &py_type_; }

	private:
		void setCallbacks();
		static void notifyLayerCallback(const char *internal_layer_name, const char *exported_layer_name, int width, int height, int layer_exported_channels, void *callback_user_data);
		static void putPixelCallback(const char *layer_name, int x, int y, float r, float g, float b, float a, void *callback_user_data);
		static void highlightPixelCallback(int x, int y, float r, float g, float b, float a, void *callback_user_data);
		static void flushAreaCallback(int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data);
		static void flushCallback(void *callback_user_data);
		static void highlightAreaCallback(int area_id, int x_0, int y_0, int x_1, int y_1, void *callback_user_data);
		[[nodiscard]] static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds);
		[[nodiscard]] static PyObject *destroy(Film * self);
		//[[nodiscard]] static PyObject *getName(Film *self, PyObject *);
		[[nodiscard]] static PyObject *getWidth(Film * self, PyObject *);
		[[nodiscard]] static PyObject *getHeight(Film * self, PyObject *);
		[[nodiscard]] static PyObject *defineCamera(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *createOutput(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *setNotifyLayerCallback(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *setPutPixelCallback(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *setHighlightPixelCallback(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *setFlushAreaCallback(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *setFlushCallback(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *setHighlightAreaCallback(Film *self, PyObject *args);
		[[nodiscard]] static PyObject *defineLayer(Film *self, PyObject *);

		PyObject_HEAD
		yafaray_Film *film_ = nullptr;
		std::unique_ptr<ImagesCollection> images_collection_;
		int border_start_x_ = 0;
		int border_start_y_ = 0;
		PyObject *notify_layer_callback_ = nullptr;
		PyObject *putpixel_callback_ = nullptr;
		PyObject *highlight_pixel_callback_ = nullptr;
		PyObject *flush_area_callback_ = nullptr;
		PyObject *flush_callback_ = nullptr;
		PyObject *highlight_callback_ = nullptr;
		static PyMethodDef py_methods_[];
		static PyTypeObject py_type_;
};

} // namespace yafaray_bindings::python

#endif //LIBYAFARAY_BINDINGS_FILM_H
