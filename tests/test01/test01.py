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

yi = libyafaray4_bindings.Interface()
yi.setConsoleVerbosityLevel(yi.logLevelFromString("debug"))
yi.setLogVerbosityLevel(yi.logLevelFromString("debug"))
yi.setConsoleLogColorsEnabled(True)
yi.enablePrintDateTime(True)

def loggingCallback(log_level, datetime, time_of_day, description):
    print("*PYTHON LOGGING CALLBACK*", log_level, datetime, time_of_day, description)

yi.setLoggingCallback(loggingCallback)

result_image_width = 400
result_image_height = 400

# Creating scene #
yi.paramsSetString("type", "yafaray")
yi.createScene()
yi.paramsClearAll()
yi.setInteractive(1)

# Creating image from RAM or file #
tex_width = 200
tex_height = 200

yi.paramsSetString("type", "ColorAlpha")
yi.paramsSetString("image_optimization", "none")  # Note: only "none" allows more HDR values > 1.f #
yi.paramsSetInt("tex_width", tex_width)
yi.paramsSetInt("tex_height", tex_height)
yi.paramsSetString("filename", "test01_tex.tga")
image = yi.createImage("Image01")
yi.paramsClearAll()

# Testing writing colors to image
for i in range(0, tex_width):
    for j in range (0, tex_height):
        yi.setImageColor(image, i, j, 0.01 * i, 0.01 * j, 0.01 * (i + j), 1)

# Testing reading colors from image
for i in range(0, 2):
    for j in range (0, 2):
        print(yi.getImageColor(image, i, j))

# Creating texture from image #
yi.paramsSetString("type", "image")
yi.paramsSetString("image_name", "Image01")
yi.createTexture("TextureTGA")
yi.paramsClearAll()

# Creating material #
# General material parameters #
yi.paramsSetString("type", "shinydiffusemat")
yi.paramsSetColor("color", 0.90, 0.90, 0.90, 1.0)
# Shader tree definition #
yi.paramsPushList()
yi.paramsSetString("element", "shader_node")
yi.paramsSetString("name", "diff_layer0")
yi.paramsSetString("input", "map0")
yi.paramsSetString("type", "layer")
yi.paramsSetString("blend_mode", "mix")
yi.paramsSetColor("upper_color", 1, 1, 1, 1)
yi.paramsPushList()
yi.paramsSetString("element", "shader_node")
yi.paramsSetString("name", "map0")
yi.paramsSetString("type", "texture_mapper")
yi.paramsSetString("mapping", "cube")
yi.paramsSetString("texco", "orco")
yi.paramsSetString("texture", "TextureTGA")
yi.paramsEndList()
# Actual material creation #
yi.paramsSetString("diffuse_shader", "diff_layer0")
yi.createMaterial("MaterialTGA")
yi.paramsClearAll()

# Creating geometric objects in the scene #
yi.startGeometry()

# Creating a geometric object #
yi.paramsSetBool("has_orco", 1)
yi.paramsSetString("type", "mesh")
yi.createObject("Cube")
yi.paramsClearAll()
# Creating vertices for the object #
yi.addVertexWithOrco(-4, 1.5, 0, -1, -1, -1)
yi.addVertexWithOrco(-4, 1.5, 2, -1, -1, 1)
yi.addVertexWithOrco(-4, 3.5, 0, -1, 1, -1)
yi.addVertexWithOrco(-4, 3.5, 2, -1, 1, 1)
yi.addVertexWithOrco(-2, 1.5, 0, 1, -1, -1)
yi.addVertexWithOrco(-2, 1.5, 2, 1, -1, 1)
yi.addVertexWithOrco(-2, 3.5, 0, 1, 1, -1)
yi.addVertexWithOrco(-2, 3.5, 2, 1, 1, 1)
# Setting up material for the faces (each face or group of faces can have different materials assigned) #
yi.setCurrentMaterial("MaterialTGA")
# Adding faces indicating the vertices indices used in each face #
yi.addTriangle(2, 0, 1)
yi.addTriangle(2, 1, 3)
yi.addTriangle(3, 7, 6)
yi.addTriangle(3, 6, 2)
yi.addTriangle(7, 5, 4)
yi.addTriangle(7, 4, 6)
yi.addTriangle(0, 4, 5)
yi.addTriangle(0, 5, 1)
yi.addTriangle(0, 2, 6)
yi.addTriangle(0, 6, 4)
yi.addTriangle(5, 7, 3)
yi.addTriangle(5, 3, 1)

# Ending definition of geometric objects #
yi.endGeometry()

# Creating light/lamp #
yi.paramsSetString("type", "pointlight")
yi.paramsSetColor("color", 1, 1, 1, 1)
yi.paramsSetVector("from", 5.3, -4.9, 8.9)
yi.paramsSetFloat("power", 150)
yi.createLight("light_1")
yi.paramsClearAll()

# Creating scene background #
yi.paramsSetString("type", "constant")
yi.paramsSetColor("color", 1, 1, 1, 1)
yi.createBackground("world_background")
yi.paramsClearAll()

# Creating camera #
yi.paramsSetString("type", "perspective")
yi.paramsSetInt("resx", result_image_width)
yi.paramsSetInt("resy", result_image_height)
yi.paramsSetFloat("focal", 1.1)
yi.paramsSetVector("from", 8.6, -7.2, 8.1)
yi.paramsSetVector("to", 8.0, -6.7, 7.6)
yi.paramsSetVector("up", 8.3, -6.8, 9)
yi.createCamera("cam_1")
yi.paramsClearAll()

# Creating scene view #
yi.paramsSetString("camera_name", "cam_1")
yi.createRenderView("view_1")
yi.paramsClearAll()

# Creating image output #
yi.paramsSetString("image_path", "./test01-output1.tga")
yi.createOutput("output1_tga")
yi.paramsClearAll()

def notifyViewCallback(view_name):
    print("*PYTHON NOTIFY VIEW CALLBACK*", view_name)

yi.setRenderNotifyViewCallback(notifyViewCallback)

def notifyLayerCallback(layer_name, weight, height, layer_exported_channels):
    print("*PYTHON NOTIFY LAYER CALLBACK*", layer_name, weight, height, layer_exported_channels)

yi.setRenderNotifyLayerCallback(notifyLayerCallback)

def putPixelCallback(view_name, layer_name, x, y, r, g, b, a):
    if x % 100 == 0 and y % 100 == 0:
        print("*PYTHON PUTPIXEL CALLBACK*", view_name, layer_name, x, y, r, g, b, a)

yi.setRenderPutPixelCallback(putPixelCallback)

def flushAreaCallback(view_name, area_id, x_0, y_0, x_1, y_1, tiles):
    print("*PYTHON FLUSHAREA CALLBACK*", view_name, area_id, x_0, y_0, x_1, y_1, tiles)

yi.setRenderFlushAreaCallback(flushAreaCallback)

def flushCallback(view_name, w, h, tiles):
    print("*PYTHON FLUSH CALLBACK*", view_name, w, h, tiles)

yi.setRenderFlushCallback(flushCallback)

def highlightAreaCallback(view_name, area_id, x_0, y_0, x_1, y_1, tiles):
    print("*PYTHON HIGHLIGHT AREA CALLBACK*", view_name, area_id, x_0, y_0, x_1, y_1, tiles)

yi.setRenderHighlightAreaCallback(highlightAreaCallback)

def highlightPixelCallback(view, x, y, r, g, b, a):
    if x % 10 == 0 and y % 10 == 0:
        print("*PYTHON HIGHLIGHT PIXEL CALLBACK*", view, x, y, r, g, b, a)

yi.setRenderHighlightPixelCallback(highlightPixelCallback)


# Creating surface integrator #
yi.paramsSetString("type", "photonmapping")
yi.createIntegrator("surfintegr")
yi.paramsClearAll()

# Creating volume integrator #
yi.paramsSetString("type", "none")
yi.createIntegrator("volintegr")
yi.paramsClearAll()

# Setting up render parameters #
yi.paramsSetString("integrator_name", "surfintegr")
yi.paramsSetString("volintegrator_name", "volintegr")
yi.paramsSetString("scene_accelerator", "yafaray-kdtree-original")
yi.paramsSetString("background_name", "world_background")
yi.paramsSetInt("width", result_image_width)
yi.paramsSetInt("height", result_image_height)
# yi.paramsSetInt("AA_minsamples",  50)
yi.paramsSetInt("AA_passes",  2)
yi.paramsSetInt("AA_inc_samples",  2)
yi.paramsSetFloat("AA_threshold",  0.05)
yi.paramsSetInt("threads", -1)
yi.paramsSetInt("threads_photons", -1)
yi.setupRender()
yi.paramsClearAll()


# Rendering #
def monitorCallback(steps_total, steps_done, tag):
    print("*PYTHON MONITOR CALLBACK*", steps_total, steps_done, tag)

yi.render(0, 0, monitorCallback)
#yi.guiCreateRenderWidget(1000, 600)

#yi.xmlParseFile("/home/david/yafa/src/libYafaRay-Xml/tests/test01/test01.xml")
#yi.guiCreateRenderWidget(1000, 600)

del yi