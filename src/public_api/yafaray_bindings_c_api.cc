/****************************************************************************
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
 */

#include "public_api/yafaray_bindings_c_api.h"
#include "common/version_build_info.h"
#include <cstring>

int yafaray_bindings_getVersionMajor() { return yafaray_bindings::buildinfo::getVersionMajor(); }
int yafaray_bindings_getVersionMinor() { return yafaray_bindings::buildinfo::getVersionMinor(); }
int yafaray_bindings_getVersionPatch() { return yafaray_bindings::buildinfo::getVersionPatch(); }

char *createCString(const std::string &std_string)
{
	const size_t string_size = std_string.size();
	char *c_string = new char[string_size + 1];
	std::strcpy(c_string, std_string.c_str());
	return c_string;
}

char *yafaray_bindings_getVersionString()
{
	return createCString(yafaray_bindings::buildinfo::getVersionString());
}

void yafaray_bindings_deallocateCharPointer(char *string_pointer_to_deallocate)
{
	delete[] string_pointer_to_deallocate;
}
