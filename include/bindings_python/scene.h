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

#ifndef LIBYAFARAY_BINDINGS_SCENE_H
#define LIBYAFARAY_BINDINGS_SCENE_H

#include "common.h"

typedef struct yafaray_Scene yafaray_Scene;

namespace yafaray_bindings::python
{
class Scene final // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	public:
		[[nodiscard]] yafaray_Scene *get() { return scene_; }
		[[nodiscard]] const yafaray_Scene *get() const { return scene_; }
		[[nodiscard]] static PyTypeObject *getType() { return &py_type_; }

	private:
		[[nodiscard]] static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *);
		[[nodiscard]] static PyObject *destroy(Scene *self);
		[[nodiscard]] static PyObject *getObjectId(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *getMaterialId(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *getImageId(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *initObject(Scene *self, PyObject *);
		[[nodiscard]] static PyObject *addVertex(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addVertexTimeStep(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addVertexWithOrco(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addVertexWithOrcoTimeStep(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addNormal(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addNormalTimeStep(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addTriangle(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addTriangleWithUv(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addQuad(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addQuadWithUv(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addUv(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *smoothObjectMesh(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *createInstance(Scene *self, PyObject *);
		[[nodiscard]] static PyObject *addInstanceObject(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addInstanceOfInstance(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *addInstanceMatrix(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *createObject(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *createLight(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *createTexture(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *createMaterial(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *defineBackground(Scene *self, PyObject *);
		[[nodiscard]] static PyObject *createVolumeRegion(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *createImage(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *setImageColor(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *getImageColor(Scene *self, PyObject *args);
		[[nodiscard]] static PyObject *checkAndClearModifiedFlags(Scene *self, PyObject *);
		[[nodiscard]] static PyObject *preprocess(Scene *self, PyObject *args);

		PyObject_HEAD
		yafaray_Scene *scene_ = nullptr;
		static PyMethodDef py_methods_[];
		static PyTypeObject py_type_;
};

} // namespace yafaray_bindings::python

#endif //LIBYAFARAY_BINDINGS_SCENE_H
