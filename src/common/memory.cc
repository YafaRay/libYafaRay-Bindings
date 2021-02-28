/****************************************************************************
 *      This is part of the libYafaRay package
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

#include "common/memory.h"
#include "common/logger.h"
#include "output/output.h"
#include "render/monitor.h"

BEGIN_YAFARAY

template <typename T>
void CustomDeleter<T>::operator()(T *object)
{
	if(!object)
	{
		if(Y_LOG_HAS_DEBUG) Y_DEBUG << "Custom deleter destruction, null pointer, exiting" << YENDL;
		return;
	}
	if(Y_LOG_HAS_DEBUG) Y_DEBUG << "Custom deleter destruction '" << object->getName() << "' auto deletion = " << (object->isAutoDeleted() ? " true (internally owned), destroying it!" : " false (externally owned), not destroying it") << YENDL;
	if(object->isAutoDeleted()) delete object;
}

template struct CustomDeleter<ColorOutput>;
template struct CustomDeleter<ProgressBar>;

END_YAFARAY