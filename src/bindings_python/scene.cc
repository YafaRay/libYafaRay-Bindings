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

#include "bindings_python/scene.h"
#include "bindings_python/logger.h"
#include "bindings_python/param_map.h"
#include "bindings_python/param_map_list.h"
#include "bindings_python/render_control.h"

namespace yafaray_bindings::python
{

PyMethodDef Scene::py_methods_[]{
		{"create",                         reinterpret_cast<PyCFunction>(create),                     METH_VARARGS, ""},
		//{"getName",                    reinterpret_cast<PyCFunction>(getName),                    METH_NOARGS,  ""},
		{"set_accelerator_params",         reinterpret_cast<PyCFunction>(setAcceleratorParams),       METH_VARARGS, ""},
		{"get_object_id",                  reinterpret_cast<PyCFunction>(getObjectId),                METH_VARARGS, ""},
		{"get_material_id",                reinterpret_cast<PyCFunction>(getMaterialId),              METH_VARARGS, ""},
		{"get_image_id",                   reinterpret_cast<PyCFunction>(getImageId),                 METH_VARARGS, ""},
		{"init_object",                    reinterpret_cast<PyCFunction>(initObject),                 METH_VARARGS, ""},
		{"add_vertex",                     reinterpret_cast<PyCFunction>(addVertex),                  METH_VARARGS, ""},
		{"add_vertex_time_step",           reinterpret_cast<PyCFunction>(addVertexTimeStep),          METH_VARARGS, ""},
		{"add_vertex_with_orco",           reinterpret_cast<PyCFunction>(addVertexWithOrco),          METH_VARARGS, ""},
		{"add_vertex_with_orco_time_step", reinterpret_cast<PyCFunction>(addVertexWithOrcoTimeStep),  METH_VARARGS, ""},
		{"add_normal",                     reinterpret_cast<PyCFunction>(addNormal),                  METH_VARARGS, ""},
		{"add_normal_time_step",           reinterpret_cast<PyCFunction>(addNormalTimeStep),          METH_VARARGS, ""},
		{"add_triangle",                   reinterpret_cast<PyCFunction>(addTriangle),                METH_VARARGS, ""},
		{"add_triangle_with_uv",           reinterpret_cast<PyCFunction>(addTriangleWithUv),          METH_VARARGS, ""},
		{"add_quad",                       reinterpret_cast<PyCFunction>(addQuad),                    METH_VARARGS, ""},
		{"add_quad_with_uv",               reinterpret_cast<PyCFunction>(addQuadWithUv),              METH_VARARGS, ""},
		{"add_uv",                         reinterpret_cast<PyCFunction>(addUv),                      METH_VARARGS, ""},
		{"smooth_object_mesh",             reinterpret_cast<PyCFunction>(smoothObjectMesh),           METH_VARARGS, ""},
		{"create_instance",                reinterpret_cast<PyCFunction>(createInstance),             METH_NOARGS,  ""},
		{"add_instance_object",            reinterpret_cast<PyCFunction>(addInstanceObject),          METH_VARARGS, ""},
		{"add_instance_of_instance",       reinterpret_cast<PyCFunction>(addInstanceOfInstance),      METH_VARARGS, ""},
		{"add_instance_matrix",            reinterpret_cast<PyCFunction>(addInstanceMatrix),          METH_VARARGS, ""},
		{"create_object",                  reinterpret_cast<PyCFunction>(createObject),               METH_VARARGS, ""},
		{"create_light",                   reinterpret_cast<PyCFunction>(createLight),                METH_VARARGS, ""},
		{"create_texture",                 reinterpret_cast<PyCFunction>(createTexture),              METH_VARARGS, ""},
		{"create_material",                reinterpret_cast<PyCFunction>(createMaterial),             METH_VARARGS, ""},
		{"define_background",              reinterpret_cast<PyCFunction>(defineBackground),           METH_VARARGS, ""},
		{"create_volume_region",           reinterpret_cast<PyCFunction>(createVolumeRegion),         METH_VARARGS, ""},
		{"create_image",                   reinterpret_cast<PyCFunction>(createImage),                METH_VARARGS, ""},
		{"set_image_color",                reinterpret_cast<PyCFunction>(setImageColor),              METH_VARARGS, ""},
		{"get_image_color",                reinterpret_cast<PyCFunction>(getImageColor),              METH_VARARGS, ""},
		{"check_and_clear_modified_flags", reinterpret_cast<PyCFunction>(checkAndClearModifiedFlags), METH_VARARGS, ""},
		{"preprocess",                     reinterpret_cast<PyCFunction>(preprocess),                 METH_VARARGS, ""},
		{nullptr} /* Sentinel */
};

//For reference about PyTypeObject see:
// https://docs.python.org/3/c-api/typeobj.html
// https://docs.python.org/3/c-api/typeobj.html#typedef-examples
PyTypeObject Scene::py_type_{
		PyVarObject_HEAD_INIT(nullptr, 0)
		"yafaray.Scene", /* tp_name */
		sizeof(Scene), /* tp_basicsize */
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
		"libYafaRay Scene object", /* tp_doc */
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

PyObject *Scene::create(PyTypeObject *type, PyObject *args, PyObject *)
{
	Logger *logger{nullptr};
	const char *name{nullptr};
	if(!PyArg_ParseTuple(args, "Os", &logger, &name)) Py_RETURN_FALSE;
	auto *self = reinterpret_cast<Scene *>(type->tp_alloc(type, 0));
	if(self)
	{
		self->scene_ = yafaray_createScene(logger->get(), name);
	}
	return reinterpret_cast<PyObject *>(self);
}

PyObject *Scene::setAcceleratorParams(Scene *self, PyObject *args)
{
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "O", &param_map)) Py_RETURN_FALSE;
	yafaray_setSceneAcceleratorParams(self->scene_, param_map->get());
	Py_RETURN_NONE;
}

/*PyObject *Scene::getName(Scene *self, PyObject *)
{
	return PyLong_FromSize_t(yafaray_createInstance(self->scene_));
}*/

PyObject *Scene::destroy(Scene *self)
{
	yafaray_destroyScene(self->scene_);
	self->scene_ = nullptr;
	Py_TYPE(self)->tp_free((PyObject *) self);
	Py_RETURN_NONE;
}

PyObject *Scene::getObjectId(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	if(!PyArg_ParseTuple(args, "s", &name)) Py_RETURN_FALSE;
	size_t object_id;
	yafaray_ResultFlags result{yafaray_getObjectId(self->scene_, &object_id, name)};
	if(result != YAFARAY_RESULT_OK) Py_RETURN_FALSE;
	else return PyLong_FromSize_t(object_id);
}

PyObject *Scene::getMaterialId(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	if(!PyArg_ParseTuple(args, "s", &name)) Py_RETURN_FALSE;
	size_t material_id;
	yafaray_ResultFlags result{yafaray_getMaterialId(self->scene_, &material_id, name)};
	if(result != YAFARAY_RESULT_OK) Py_RETURN_FALSE;
	else return PyLong_FromSize_t(material_id);
}

PyObject *Scene::getImageId(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	if(!PyArg_ParseTuple(args, "s", &name)) Py_RETURN_FALSE;
	size_t image_id;
	yafaray_ResultFlags result{yafaray_getImageId(self->scene_, name, &image_id)};
	if(result != YAFARAY_RESULT_OK) Py_RETURN_FALSE;
	else return PyLong_FromSize_t(image_id);
}

PyObject *Scene::initObject(Scene *self, PyObject *args)
{
	unsigned long long object_id, material_id;
	if(!PyArg_ParseTuple(args, "KK", &object_id, &material_id)) Py_RETURN_FALSE;
	yafaray_initObject(self->scene_, object_id, material_id);
	Py_RETURN_NONE;
}

PyObject *Scene::addVertex(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double x, y, z;
	if(!PyArg_ParseTuple(args, "Kddd", &object_id, &x, &y, &z)) Py_RETURN_FALSE;
	yafaray_addVertex(self->scene_, object_id, x, y, z);
	Py_RETURN_NONE;
}

PyObject *Scene::addVertexTimeStep(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double x, y, z;
	int time_step;
	if(!PyArg_ParseTuple(args, "Kdddi", &object_id, &x, &y, &z, &time_step)) Py_RETURN_FALSE;
	yafaray_addVertexTimeStep(self->scene_, object_id, x, y, z, time_step);
	Py_RETURN_NONE;
}

PyObject *Scene::addVertexWithOrco(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double x, y, z, ox, oy, oz;
	if(!PyArg_ParseTuple(args, "Kdddddd", &object_id, &x, &y, &z, &ox, &oy, &oz)) Py_RETURN_FALSE;
	yafaray_addVertexWithOrco(self->scene_, object_id, x, y, z, ox, oy, oz);
	Py_RETURN_NONE;
}

PyObject *Scene::addVertexWithOrcoTimeStep(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double x, y, z, ox, oy, oz;
	int time_step;
	if(!PyArg_ParseTuple(args, "Kddddddi", &object_id, &x, &y, &z, &ox, &oy, &oz, &time_step)) Py_RETURN_FALSE;
	yafaray_addVertexWithOrcoTimeStep(self->scene_, object_id, x, y, z, ox, oy, oz, time_step);
	Py_RETURN_NONE;
}

PyObject *Scene::addNormal(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double x, y, z;
	if(!PyArg_ParseTuple(args, "Kddd", &object_id, &x, &y, &z)) Py_RETURN_FALSE;
	yafaray_addNormal(self->scene_, object_id, x, y, z);
	Py_RETURN_NONE;
}

PyObject *Scene::addNormalTimeStep(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double x, y, z;
	int time_step;
	if(!PyArg_ParseTuple(args, "Kdddi", &object_id, &x, &y, &z, &time_step)) Py_RETURN_FALSE;
	yafaray_addNormalTimeStep(self->scene_, object_id, x, y, z, time_step);
	Py_RETURN_NONE;
}

PyObject *Scene::addTriangle(Scene *self, PyObject *args)
{
	unsigned long long object_id, material_id;
	int a, b, c;
	if(!PyArg_ParseTuple(args, "KiiiK", &object_id, &a, &b, &c, &material_id)) Py_RETURN_FALSE;
	yafaray_addTriangle(self->scene_, object_id, a, b, c, material_id);
	Py_RETURN_NONE;
}

PyObject *Scene::addTriangleWithUv(Scene *self, PyObject *args)
{
	unsigned long long object_id, material_id;
	int a, b, c, uv_a, uv_b, uv_c;
	if(!PyArg_ParseTuple(args, "KiiiiiiK", &object_id, &a, &b, &c, &uv_a, &uv_b, &uv_c, &material_id)) Py_RETURN_FALSE;
	yafaray_addTriangleWithUv(self->scene_, object_id, a, b, c, uv_a, uv_b, uv_c, material_id);
	Py_RETURN_NONE;
}

PyObject *Scene::addQuad(Scene *self, PyObject *args)
{
	unsigned long long object_id, material_id;
	int a, b, c, d;
	if(!PyArg_ParseTuple(args, "KiiiiK", &object_id, &a, &b, &c, &d, &material_id)) Py_RETURN_FALSE;
	yafaray_addQuad(self->scene_, object_id, a, b, c, d, material_id);
	Py_RETURN_NONE;
}

PyObject *Scene::addQuadWithUv(Scene *self, PyObject *args)
{
	unsigned long long object_id, material_id;
	int a, b, c, d, uv_a, uv_b, uv_c, uv_d;
	if(!PyArg_ParseTuple(args, "KiiiiiiiiK", &object_id, &a, &b, &c, &d, &uv_a, &uv_b, &uv_c, &uv_d, &material_id)) Py_RETURN_FALSE;
	yafaray_addQuadWithUv(self->scene_, object_id, a, b, c, d, uv_a, uv_b, uv_c, uv_d, material_id);
	Py_RETURN_NONE;
}

PyObject *Scene::addUv(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double u, v;
	if(!PyArg_ParseTuple(args, "Kdd", &object_id, &u, &v)) Py_RETURN_FALSE;
	return PyLong_FromSize_t(yafaray_addUv(self->scene_, object_id, u, v));
}

PyObject *Scene::smoothObjectMesh(Scene *self, PyObject *args)
{
	unsigned long long object_id;
	double angle;
	if(!PyArg_ParseTuple(args, "Kd", &object_id, &angle)) Py_RETURN_FALSE;
	yafaray_smoothObjectMesh(self->scene_, object_id, angle);
	Py_RETURN_NONE;
}

PyObject *Scene::createInstance(Scene *self, PyObject *)
{
	return PyLong_FromSize_t(yafaray_createInstance(self->scene_));
}

PyObject *Scene::addInstanceObject(Scene *self, PyObject *args)
{
	unsigned long long instance_id;
	unsigned long long base_object_id;
	if(!PyArg_ParseTuple(args, "KK", &instance_id, &base_object_id)) Py_RETURN_FALSE;
	yafaray_addInstanceObject(self->scene_, instance_id, base_object_id);
	Py_RETURN_NONE;
}

PyObject *Scene::addInstanceOfInstance(Scene *self, PyObject *args)
{
	unsigned long long instance_id;
	unsigned long long base_instance_id;
	if(!PyArg_ParseTuple(args, "KK", &instance_id, &base_instance_id)) Py_RETURN_FALSE;
	yafaray_addInstanceOfInstance(self->scene_, instance_id, base_instance_id);
	Py_RETURN_NONE;
}

PyObject *Scene::addInstanceMatrix(Scene *self, PyObject *args)
{
	unsigned long long instance_id;
	double obj_to_world[4][4];
	float time;
	if(!PyArg_ParseTuple(args, "Kddddddddddddddddf", &instance_id,
		&obj_to_world[0][0], &obj_to_world[0][1], &obj_to_world[0][2], &obj_to_world[0][3],
		&obj_to_world[1][0], &obj_to_world[1][1], &obj_to_world[1][2], &obj_to_world[1][3],
		&obj_to_world[2][0], &obj_to_world[2][1], &obj_to_world[2][2], &obj_to_world[2][3],
		&obj_to_world[3][0], &obj_to_world[3][1], &obj_to_world[3][2], &obj_to_world[3][3],
		&time
	)) Py_RETURN_FALSE;
	yafaray_addInstanceMatrixArray(self->scene_, instance_id, reinterpret_cast<const double *>(obj_to_world), time);
	Py_RETURN_NONE;
}

PyObject *Scene::createObject(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "sO", &name, &param_map)) Py_RETURN_FALSE;
	size_t object_id;
	yafaray_ResultFlags result{yafaray_createObject(self->scene_, &object_id, name, param_map->get())};
	if(result != YAFARAY_RESULT_OK) Py_RETURN_FALSE;
	else return PyLong_FromSize_t(object_id);
}

PyObject *Scene::createLight(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "sO", &name, &param_map)) Py_RETURN_FALSE;
	yafaray_createLight(self->scene_, name, param_map->get());
	Py_RETURN_NONE;
}

PyObject *Scene::createTexture(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "sO", &name, &param_map)) Py_RETURN_FALSE;
	yafaray_createTexture(self->scene_, name, param_map->get());
	Py_RETURN_NONE;
}

PyObject *Scene::createMaterial(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	const ParamMapList *param_map_list{nullptr};
	if(!PyArg_ParseTuple(args, "sOO", &name, &param_map, &param_map_list)) Py_RETURN_FALSE;
	size_t material_id;
	yafaray_ResultFlags result{yafaray_createMaterial(self->scene_, &material_id, name, param_map->get(), param_map_list->get())};
	if(result != YAFARAY_RESULT_OK) Py_RETURN_FALSE;
	else return PyLong_FromSize_t(material_id);
}

PyObject *Scene::defineBackground(Scene *self, PyObject *args)
{
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "O", &param_map)) Py_RETURN_FALSE;
	yafaray_defineBackground(self->scene_, param_map->get());
	Py_RETURN_NONE;
}

PyObject *Scene::createVolumeRegion(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "sO", &name, &param_map)) Py_RETURN_FALSE;
	yafaray_createVolumeRegion(self->scene_, name, param_map->get());
	Py_RETURN_NONE;
}

PyObject *Scene::createImage(Scene *self, PyObject *args)
{
	const char *name{nullptr};
	const ParamMap *param_map{nullptr};
	if(!PyArg_ParseTuple(args, "sO", &name, &param_map)) Py_RETURN_FALSE;
	size_t image_id;
	yafaray_ResultFlags result{yafaray_createImage(self->scene_, name, &image_id, param_map->get())};
	if(result != YAFARAY_RESULT_OK) Py_RETURN_FALSE;
	else return PyLong_FromSize_t(image_id);
}

PyObject *Scene::setImageColor(Scene *self, PyObject *args)
{
	unsigned long long image_id;
	int x, y;
	float r, g, b, a;
	if(!PyArg_ParseTuple(args, "Kiiffff", &image_id, &x, &y, &r, &g, &b, &a)) Py_RETURN_FALSE;
	yafaray_setImageColor(self->scene_, image_id, x, y, r, g, b, a);
	Py_RETURN_NONE;
}

PyObject *Scene::getImageColor(Scene *self, PyObject *args)
{
	unsigned long long image_id;
	int x, y;
	if(!PyArg_ParseTuple(args, "Kii", &image_id, &x, &y)) Py_RETURN_FALSE;
	float r, g, b, a;
	yafaray_getImageColor(self->scene_, image_id, x, y, &r, &g, &b, &a);
	return Py_BuildValue("ffff", r, g, b, a);
}

PyObject *Scene::checkAndClearModifiedFlags(Scene *self, PyObject *args)
{
	const yafaray_SceneModifiedFlags scene_modified_flags{yafaray_checkAndClearSceneModifiedFlags(self->scene_)};
	return PyLong_FromLong(scene_modified_flags);
}

PyObject *Scene::preprocess(Scene *self, PyObject *args)
{
	const RenderControl *render_control{nullptr};
	yafaray_SceneModifiedFlags scene_modified_flags{YAFARAY_SCENE_MODIFIED_NOTHING};
	if(!PyArg_ParseTuple(args, "Oi", &render_control, &scene_modified_flags)) Py_RETURN_FALSE;
	yafaray_Bool result{yafaray_preprocessScene(self->scene_, render_control->get(), scene_modified_flags)};
	if(result == YAFARAY_BOOL_TRUE) Py_RETURN_TRUE;
	else Py_RETURN_FALSE;
}

} // namespace yafaray_bindings::python

