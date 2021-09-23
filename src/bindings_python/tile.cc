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

#include "bindings_python/tile.h"

BEGIN_YAFARAY_BINDINGS

Py_ssize_t pythonTileSize_global(const Tile *tile)
{
	const int area_w = (tile->area_x_1_ - tile->area_x_0_);
	const int area_h = (tile->area_y_1_ - tile->area_y_0_);

	return area_w * area_h;
}

PyObject *pythonTileSubscriptInt_global(const Tile *tile, int py_index)
{
	const int num_channels = tile->exported_channels_;
	// Check boundaries and fill w and h
	if(py_index >= pythonTileSize_global(tile) || py_index < 0)
	{
		PyObject* groupPix = PyTuple_New(num_channels);
		for(int i = 0; i < num_channels; ++i)
		{
			PyTuple_SET_ITEM(groupPix, i, PyFloat_FromDouble((i == 3 ? 1.f : 0.f)));
		}
		return groupPix;
	}

	const int area_w = (tile->area_x_1_ - tile->area_x_0_);
	const int area_h = (tile->area_y_1_ - tile->area_y_0_);

	// Calc position of the tile in the image region
	int vy = py_index / area_w;
	int vx = py_index - vy * area_w;

	// Map tile position to image buffer
	vx = tile->area_x_0_ + vx;
	vy = (tile->area_y_0_ + area_h - 1) - vy;

	// Get pixel
	const RgbaFloat color = tile->image_ ? tile->image_->getColor(vx, vy) : RgbaFloat();

	PyObject* groupPix = PyTuple_New(num_channels);
	if(num_channels >= 1)
	{
		PyTuple_SET_ITEM(groupPix, 0, PyFloat_FromDouble(color.getR()));
	}
	if(num_channels >= 3)
	{
		PyTuple_SET_ITEM(groupPix, 1, PyFloat_FromDouble(color.getG()));
		PyTuple_SET_ITEM(groupPix, 2, PyFloat_FromDouble(color.getB()));
	}
	if(num_channels == 4)
	{
		PyTuple_SET_ITEM(groupPix, 3, PyFloat_FromDouble(color.getA()));
	}
	return groupPix;
}

void pythonTileDelete_global(Tile *tile)
{
	PyObject_Del(tile);
}

void Tile::drawCorner(int x, int y, int border_length, CornerPosition corner_position)
{
	if(!image_) return;

	int x_min = 0;
	int y_min = 0;
	int x_max = 0;
	int y_max = 0;

	switch(corner_position)
	{
		case CornerPosition::TopLeft:
			x_min = x;
			y_min = y;
			x_max = x + border_length;
			y_max = y + border_length;
			break;

		case CornerPosition::TopRight:
			x_min = x - border_length - 1;
			y_min = y;
			x_max = x - 1;
			y_max = y + border_length;
			--x;
			break;

		case CornerPosition::BottomLeft:
			x_min = x;
			y_min = y - border_length - 1;
			x_max = x + border_length;
			y_max = y - 1;
			--y;
			break;

		case CornerPosition::BottomRight:
			x_min = x - border_length - 1;
			y_min = y - border_length - 1;
			x_max = x;
			y_max = y - 1;
			--x;
			--y;
			break;
	}

	const RgbaFloat border_color {0.625f, 0.f, 0.f, 1.f};

	for(int i = x_min; i < x_max; ++i)
	{
		image_->setColor(i, y, border_color);
	}

	for(int j = y_min; j < y_max; ++j)
	{
		image_->setColor(x, j, border_color);
	}
}

END_YAFARAY_BINDINGS