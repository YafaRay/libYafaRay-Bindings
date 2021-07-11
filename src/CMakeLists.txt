add_library(libyafaray4_bindings)
set_target_properties(libyafaray4_bindings PROPERTIES PREFIX "" VERSION 4.0.0 SOVERSION 4)
set_target_properties(libyafaray4_bindings PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
set_target_properties(libyafaray4_bindings PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)
target_include_directories(libyafaray4_bindings PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include)
target_include_directories(libyafaray4_bindings INTERFACE $<INSTALL_INTERFACE:include>)
target_link_libraries(libyafaray4_bindings PRIVATE LibYafaRay::libyafaray4)

add_subdirectory(common)
add_subdirectory(public_api)

message_boolean("Building with XML loading support" YAFARAY_BINDINGS_WITH_XML "yes" "no")
if(YAFARAY_BINDINGS_WITH_XML)
	find_package(LibYafaRay_Xml 4.0.0 REQUIRED)
	list(APPEND YAFARAY_BINDINGS_CUSTOM_DEFINITIONS "YAFARAY_BINDINGS_WITH_XML")
	target_link_libraries(libyafaray4_bindings PRIVATE LibYafaRay_Xml::libyafaray4_xml)
endif()

message_boolean("Building with Gui support" YAFARAY_BINDINGS_WITH_XML "yes" "no")
if(YAFARAY_BINDINGS_WITH_GUI)
	find_package(LibYafaRay_Gui 4.0.0 REQUIRED)
	list(APPEND YAFARAY_BINDINGS_CUSTOM_DEFINITIONS "YAFARAY_BINDINGS_WITH_GUI")
	target_link_libraries(libyafaray4_bindings PRIVATE LibYafaRay_Gui::libyafaray4_gui)
endif()

message_boolean("Build with Python language bindings" YAFARAY_BINDINGS_WITH_PYTHON "yes" "no")
if(YAFARAY_BINDINGS_WITH_PYTHON)
	find_package(Python REQUIRED)
	if(PYTHON_LIBRARIES)
		message("Python ${REQUIRED_PYTHON_VERSION} found at: ${PYTHON_LIBRARIES}")
		list(APPEND YAFARAY_BINDINGS_CUSTOM_DEFINITIONS "YAFARAY_BINDINGS_WITH_PYTHON")
		target_link_libraries(libyafaray4_bindings PRIVATE Python::Python)
		add_subdirectory(bindings_python)
	else()
		message("Python wasn't found, please install it to build python bindings")
	endif()
endif()

message_boolean("Build with Ruby language bindings" YAFARAY_BINDINGS_WITH_RUBY "yes" "no")
if(YAFARAY_BINDINGS_WITH_RUBY)
	find_package(Ruby 1.8 REQUIRED)
	if(SWIG_FOUND AND RUBY_FOUND)
		message("Ruby version found: ${RUBY_VERSION}")
		message("Ruby headers at: ${RUBY_INCLUDE_DIR}")
		message("Ruby library at: ${RUBY_LIBRARY}")
		list(APPEND YAFARAY_BINDINGS_CUSTOM_DEFINITIONS "YAFARAY_BINDINGS_WITH_RUBY")
		target_link_libraries(libyafaray4_bindings PRIVATE Ruby)
		add_subdirectory(bindings_ruby)
	else()
		message("Ruby wasn't found, please install it to build python bindings")
	endif()
endif()

target_include_directories(libyafaray4_bindings SYSTEM BEFORE PRIVATE ${YAF_BINDINGS_DEPS_INCLUDE_DIRS})

target_compile_definitions(libyafaray4_bindings
		PRIVATE
		"${YAFARAY_BINDINGS_CUSTOM_DEFINITIONS}"
		"YAFARAY_BINDINGS_BUILD_TYPE=\"$<UPPER_CASE:$<CONFIG>>\""
		"YAFARAY_BINDINGS_BUILD_FLAGS=\"${CMAKE_CXX_FLAGS} $<$<CONFIG:Debug>:${CMAKE_CXX_FLAGS_DEBUG}>$<$<CONFIG:Release>:${CMAKE_CXX_FLAGS_RELEASE}>$<$<CONFIG:RelWithDebInfo>:${CMAKE_CXX_FLAGS_RELWITHDEBINFO}>$<$<CONFIG:MinSizeRel>:${CMAKE_CXX_FLAGS_MINSIZEREL}>\"")

# Custom linker options
if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
	target_link_options(libyafaray4_bindings PRIVATE
			"LINKER:SHELL:--no-undefined"
			"LINKER:SHELL:--version-script=${PROJECT_SOURCE_DIR}/include/public_api/yafaray_bindings_c_api_symbols.map" # Custom symbols export and versioning map
			"$<$<CONFIG:RELEASE>:LINKER:SHELL:-s>" # Strip in release builds
			)
endif()

# Copy C API include files to binary directory for other targets to use
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include)
configure_file(${PROJECT_SOURCE_DIR}/include/public_api/yafaray_bindings_c_api.h ${PROJECT_BINARY_DIR}/include/)

# Shared target used for generating export header for both the shared and static targets
include(GenerateExportHeader)
generate_export_header(libyafaray4_bindings BASE_NAME yafaray_bindings_c_api)
generate_export_header(libyafaray4_bindings BASE_NAME yafaray_bindings_c_api EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/yafaray_bindings_c_api_export.h)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PROJECT_BINARY_DIR}/cmake/LibYafaRay_BindingsConfigVersion.cmake
		COMPATIBILITY "AnyNewerVersion"
		)

install(TARGETS libyafaray4_bindings
		EXPORT LibYafaRay_Bindings
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		)

install(FILES
		${PROJECT_SOURCE_DIR}/include/public_api/yafaray_bindings_c_api.h
		${PROJECT_BINARY_DIR}/include/yafaray_bindings_c_api_export.h
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		)

install(EXPORT LibYafaRay_Bindings
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibYafaRay_Bindings
		NAMESPACE LibYafaRay_Bindings::
		FILE LibYafaRay_BindingsConfig.cmake
		)

install(FILES
		${PROJECT_BINARY_DIR}/cmake/LibYafaRay_BindingsConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibYafaRay_Bindings
		)
