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

#ifndef LIBYAFARAY_BINDINGS_RENDERER_H
#define LIBYAFARAY_BINDINGS_RENDERER_H

#include "common.h"

typedef struct yafaray_Renderer yafaray_Renderer;

namespace yafaray_bindings::python
{
class Renderer final // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	public:
		[[nodiscard]] yafaray_Renderer *get() { return renderer_; }
		[[nodiscard]] const yafaray_Renderer *get() const { return renderer_; }
		[[nodiscard]] static PyTypeObject *getType() { return &py_type_; }

	private:
		[[nodiscard]] static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *);
		[[nodiscard]] static PyObject *destroy(Renderer *self);
		[[nodiscard]] static PyObject *defineSurfaceIntegrator(Renderer *self, PyObject *args);
		[[nodiscard]] static PyObject *defineVolumeIntegrator(Renderer *self, PyObject *args);
		[[nodiscard]] static PyObject *setup(Renderer *self, PyObject *args);
		[[nodiscard]] static PyObject *render(Renderer *self, PyObject *args);
		[[nodiscard]] static PyObject *cancel(Renderer *self, PyObject *args);
		static void monitorCallback(int steps_total, int steps_done, const char *tag, void *callback_user_data);

		PyObject_HEAD
		yafaray_Renderer *renderer_ = nullptr;
		PyObject *monitor_callback_ = nullptr;
		static PyMethodDef py_methods_[];
		static PyTypeObject py_type_;
};

} // namespace yafaray_bindings::python

#endif //LIBYAFARAY_BINDINGS_RENDERER_H
