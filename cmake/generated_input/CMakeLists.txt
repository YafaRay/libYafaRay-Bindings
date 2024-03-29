#****************************************************************************
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

find_package(Git)
if(GIT_FOUND)
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --dirty --always --tags WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE YAFARAY_BINDINGS_VERSION_GIT OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0 WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE YAFARAY_BINDINGS_VERSION_GIT_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE YAFARAY_BINDINGS_VERSION_GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(YAFARAY_BINDINGS_VERSION_GIT MATCHES "dirty$")
		set(YAFARAY_BINDINGS_VERSION_GIT_DIRTY "dirty")
	endif()
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE YAFARAY_BINDINGS_VERSION_GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=%ci ${YAFARAY_BINDINGS_VERSION_GIT_COMMIT} WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE YAFARAY_BINDINGS_VERSION_GIT_COMMIT_DATETIME OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-list ${YAFARAY_BINDINGS_VERSION_GIT_TAG}..HEAD --count WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE YAFARAY_BINDINGS_VERSION_GIT_COMMITS_SINCE_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)

set(YAFARAY_BINDINGS_BUILD_COMPILER "${CMAKE_CXX_COMPILER_ID}") #-${CMAKE_CXX_COMPILER_VERSION}")
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	set(YAFARAY_BINDINGS_BUILD_COMPILER "GCC") #To avoid confusion, just name it GCC and not GNU when GCC is used
endif()

if(MINGW)
	string(APPEND YAFARAY_BINDINGS_BUILD_COMPILER "/MinGW")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(YAFARAY_BINDINGS_BUILD_ARCHITECTURE_BITS "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(YAFARAY_BINDINGS_BUILD_ARCHITECTURE_BITS "32")
endif()

set(YAFARAY_BINDINGS_BUILD_OS "${CMAKE_SYSTEM_NAME}")

set(YAFARAY_BINDINGS_VERSION_STRING "${YAFARAY_BINDINGS_VERSION}")
if(NOT YAFARAY_BINDINGS_VERSION_PRE_RELEASE STREQUAL "")
	string(APPEND YAFARAY_BINDINGS_VERSION_STRING "-${YAFARAY_BINDINGS_VERSION_PRE_RELEASE}")
endif()

configure_file(yafaray_bindings_version_build_info.h.in ${PROJECT_BINARY_DIR}/include/yafaray_bindings_version_build_info.h)
