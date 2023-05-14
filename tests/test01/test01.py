############################################################################
#      This is part of the libYafaRay-Bindings package
#
#      This library is free software; you can redistribute it and/or
#      modify it under the terms of the GNU Lesser General Public
#      License as published by the Free Software Foundation; either
#      version 2.1 of the License, or (at your option) any later version.
#
#      This library is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#      Lesser General Public License for more details.
#
#      You should have received a copy of the GNU Lesser General Public
#      License along with this library; if not, write to the Free Software
#      Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

import libyafaray4_bindings

logger = libyafaray4_bindings.Logger()
logger.setConsoleVerbosityLevel(logger.logLevelFromString("debug"))
logger.setLogVerbosityLevel(logger.logLevelFromString("debug"))
#logger.setConsoleLogColorsEnabled(True)
logger.enablePrintDateTime(True)


def loggingCallback(log_level, datetime, time_of_day, description):
    print("*PYTHON LOGGING CALLBACK*", log_level, datetime, time_of_day, description)


logger.setCallback(loggingCallback)

result_image_width = 400
result_image_height = 400

# Creating scene #
scene = libyafaray4_bindings.Scene(logger, "Scene1")
param_map = libyafaray4_bindings.ParamMap()
param_map.setString("type", "yafaray-kdtree-original")
scene.setAcceleratorParams(param_map)

# Creating image from RAM or file #
tex_width = 200
tex_height = 200

param_map.clear()
param_map.setString("type", "ColorAlpha")
param_map.setString("image_optimization", "none")  # Note: only "none" allows more HDR values > 1.f #
param_map.setString("filename", "test01_tex.tga")
image_id = scene.createImage("Image01", param_map)

# Testing writing colors to image
for i in range(0, tex_width):
    for j in range(0, tex_height):
        scene.setImageColor(image_id, i, j, 0.01 * i, 0.01 * j, 0.01 * (i + j), 1)

# Testing reading colors from image
for i in range(0, 2):
    for j in range(0, 2):
        print(scene.getImageColor(image_id, i, j))

# Creating texture from image #
param_map.clear()
param_map.setString("type", "image")
param_map.setString("image_name", "Image01")
scene.createTexture("TextureTGA", param_map)

# Creating material #
# Shader tree definition #
param_map_list = libyafaray4_bindings.ParamMapList()
param_map.clear()
param_map.setString("element", "shader_node")
param_map.setString("name", "diff_layer0")
param_map.setString("input", "map0")
param_map.setString("type", "layer")
param_map.setString("blend_mode", "mix")
param_map.setColor("upper_color", 1, 1, 1, 1)
param_map_list.addParamMap(param_map)
param_map.clear()
param_map.setString("element", "shader_node")
param_map.setString("name", "map0")
param_map.setString("type", "texture_mapper")
param_map.setString("mapping", "cube")
param_map.setString("texco", "orco")
param_map.setString("texture", "TextureTGA")
param_map_list.addParamMap(param_map)
# General material parameters #
param_map.clear()
param_map.setString("type", "shinydiffusemat")
param_map.setColor("color", 0.90, 0.90, 0.90, 1.0)
param_map.setString("diffuse_shader", "diff_layer0")
scene.createMaterial("MaterialTGA", param_map, param_map_list)

# Creating a geometric object #
param_map.clear()
param_map.setBool("has_orco", 1)
param_map.setString("type", "mesh")
object_id = scene.createObject("Cube", param_map)
# Creating vertices for the object #
scene.addVertexWithOrco(object_id, -4, 1.5, 0, -1, -1, -1)
scene.addVertexWithOrco(object_id, -4, 1.5, 2, -1, -1, 1)
scene.addVertexWithOrco(object_id, -4, 3.5, 0, -1, 1, -1)
scene.addVertexWithOrco(object_id, -4, 3.5, 2, -1, 1, 1)
scene.addVertexWithOrco(object_id, -2, 1.5, 0, 1, -1, -1)
scene.addVertexWithOrco(object_id, -2, 1.5, 2, 1, -1, 1)
scene.addVertexWithOrco(object_id, -2, 3.5, 0, 1, 1, -1)
scene.addVertexWithOrco(object_id, -2, 3.5, 2, 1, 1, 1)
# Setting up material for the faces (each face or group of faces can have different materials assigned) #
material_id = scene.getMaterialId("MaterialTGA")
# Adding faces indicating the vertices indices used in each face #
scene.addTriangle(object_id, 2, 0, 1, material_id)
scene.addTriangle(object_id, 2, 1, 3, material_id)
scene.addTriangle(object_id, 3, 7, 6, material_id)
scene.addTriangle(object_id, 3, 6, 2, material_id)
scene.addTriangle(object_id, 7, 5, 4, material_id)
scene.addTriangle(object_id, 7, 4, 6, material_id)
scene.addTriangle(object_id, 0, 4, 5, material_id)
scene.addTriangle(object_id, 0, 5, 1, material_id)
scene.addTriangle(object_id, 0, 2, 6, material_id)
scene.addTriangle(object_id, 0, 6, 4, material_id)
scene.addTriangle(object_id, 5, 7, 3, material_id)
scene.addTriangle(object_id, 5, 3, 1, material_id)

# Creating light/lamp #
param_map.clear()
param_map.setString("type", "pointlight")
param_map.setColor("color", 1, 1, 1, 1)
param_map.setVector("from", 5.3, -4.9, 8.9)
param_map.setFloat("power", 150)
scene.createLight("light_1", param_map)

# Creating scene background #
param_map.clear()
param_map.setString("type", "constant")
param_map.setColor("color", 1, 1, 1, 1)
scene.defineBackground(param_map)

# Creating SurfaceIntegrator #
param_map.clear()
param_map.setString("type", "photonmapping")
# param_map.setInt("AA_minsamples",  50)
param_map.setInt("AA_passes", 5)
param_map.setInt("AA_inc_samples", 2)
param_map.setFloat("AA_threshold", 0.05)
param_map.setInt("threads", -1)
param_map.setInt("threads_photons", -1)
surface_integrator = libyafaray4_bindings.SurfaceIntegrator(logger, "SurfaceIntegrator1", param_map)

# Creating volume integrator #
param_map.clear()
param_map.setString("type", "none")
surface_integrator.defineVolumeIntegrator(scene, param_map)

# Creating Film #
param_map.clear()
film = libyafaray4_bindings.Film(logger, surface_integrator, "Film1", param_map)

# Creating camera #
param_map.clear()
param_map.setString("type", "perspective")
param_map.setInt("resx", result_image_width)
param_map.setInt("resy", result_image_height)
param_map.setFloat("focal", 1.1)
param_map.setVector("from", 8.6, -7.2, 8.1)
param_map.setVector("to", 8.0, -6.7, 7.6)
param_map.setVector("up", 8.3, -6.8, 9)
film.defineCamera("cam_1", param_map)

# Creating image output #
param_map.clear()
param_map.setString("image_path", "./test01-output1.tga")
film.createOutput("output1_tga", param_map)


def notifyLayerCallback(layer_name, width, height, layer_exported_channels):
    print("*PYTHON NOTIFY LAYER CALLBACK*", layer_name, width, height, layer_exported_channels)


film.setNotifyLayerCallback(notifyLayerCallback)


def putPixelCallback(layer_name, x, y, r, g, b, a):
    if x % 100 == 0 and y % 100 == 0:
        print("*PYTHON PUTPIXEL CALLBACK*", layer_name, x, y, r, g, b, a)


film.setPutPixelCallback(putPixelCallback)


def flushAreaCallback(area_id, x_0, y_0, x_1, y_1, tiles):
    print("*PYTHON FLUSHAREA CALLBACK*", area_id, x_0, y_0, x_1, y_1, tiles)


film.setFlushAreaCallback(flushAreaCallback)


def flushCallback(w, h, tiles):
    print("*PYTHON FLUSH CALLBACK*", w, h, tiles)


film.setFlushCallback(flushCallback)


def highlightAreaCallback(area_id, x_0, y_0, x_1, y_1, tiles):
    print("*PYTHON HIGHLIGHT AREA CALLBACK*", area_id, x_0, y_0, x_1, y_1, tiles)


film.setHighlightAreaCallback(highlightAreaCallback)


def highlightPixelCallback(x, y, r, g, b, a):
    if x % 10 == 0 and y % 10 == 0:
        print("*PYTHON HIGHLIGHT PIXEL CALLBACK*", x, y, r, g, b, a)


film.setHighlightPixelCallback(highlightPixelCallback)


# Rendering #
def monitorCallback(steps_total, steps_done, tag):
    print("*PYTHON MONITOR CALLBACK*", steps_total, steps_done, tag)

# Creating RenderControl #
render_control = libyafaray4_bindings.RenderControl()

# Creating RenderMonitor #
render_monitor = libyafaray4_bindings.RenderMonitor(monitorCallback)

render_control.setForNormalStart()
scene_modified_flags = scene.checkAndClearModifiedFlags()
scene.preprocess(render_control, scene_modified_flags)
surface_integrator.preprocess(render_monitor, render_control, scene)
surface_integrator.render(render_control, render_monitor, film)
# yi.guiCreateRenderWidget(1000, 600)

# yi.xmlParseFile("/home/david/yafa/src/libYafaRay-Xml/tests/test01/test01.xml")
# yi.guiCreateRenderWidget(1000, 600)
#
# del render_monitor
# del render_control
# del film
# del surface_integrator
# del scene
# del param_map_list
# del param_map
# del logger