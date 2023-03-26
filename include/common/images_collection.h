#pragma once
/****************************************************************************
 *      This is part of the libYafaRay-Gui package
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
 */

#ifndef LIBYAFARAY_GUI_IMAGES_COLLECTION_H
#define LIBYAFARAY_GUI_IMAGES_COLLECTION_H

#include <string>
#include <map>
#include <memory>

namespace yafaray_bindings
{

class RgbaFloat;
class Image;

class ImagesCollection final
{
	public:
		ImagesCollection(int width, int height) : images_width_(width), images_height_(height) { }
		[[nodiscard]] int getWidth() const { return images_width_; }
		[[nodiscard]] int getHeight() const { return images_height_; }
		void setColor(const std::string &layer_name, int x, int y, const RgbaFloat &rgba);
		[[nodiscard]] RgbaFloat getColor(const std::string &layer_name, int x, int y) const;
		[[nodiscard]] int getExportedChannels(const std::string &layer_name) const;
		[[nodiscard]] std::string getExportedLayerName(const std::string &layer_name) const;
		[[nodiscard]] const std::map<std::string, int> &getLayersAndExportedChannels() const { return layers_exported_channels_; };
		[[nodiscard]] Image *findLayer(const std::string &layer_name);
		[[nodiscard]] const Image *findLayer(const std::string &layer_name) const;
		void clear();
		void defineLayer(const std::string &internal_layer_name, const std::string &exported_layer_name, int width, int height, int channels_exported);
	private:
		int images_width_ = 0;
		int images_height_ = 0;
		std::map<std::string, std::shared_ptr<Image>> images_;
		std::map<std::string, int> layers_exported_channels_;
		std::map<std::string, std::string> dict_internal_exported_layers_;
};

} // namespace yafaray_bindings

#endif //LIBYAFARAY_GUI_IMAGES_COLLECTION_H
