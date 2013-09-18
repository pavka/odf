cmake_minimum_required(VERSION 2.6.0)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
set(APPLICATION_NAME "Object Detection Framework")

# SOVERSION scheme: CURRENT.AGE.REVISION
#   If there was an incompatible interface change:
#     Increment CURRENT. Set AGE and REVISION to 0
#   If there was a compatible interface change:
#     Increment AGE. Set REVISION to 0
#   If the source code was changed, but there were no interface changes:
#     Increment REVISION.
set(LIBRARY_VERSION "0.1.0")
set(LIBRARY_SOVERSION "0")

#
# Include definitions.
#
include(DefineCMakeDefaults)
include(DefinePlatformDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)

#
# Disallow in-source build.
#
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")

#
# Set CPP flags.
#
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wall -Werror -Wno-unused-parameter -Wno-sign-compare -Wshadow")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wextra -Wall -Wno-unused-parameter -Wno-sign-compare -Wshadow")

#
# Set build directories.
#
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(ODF_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${ODF_INCLUDE_DIR})

#
# Define configurable options.
#
if (DEFINED ENV{OpenCV_DIR})
    set(OpenCV_DIR $ENV{OpenCV_DIR})
else (DEFINED ENV{OpenCV_DIR})
    set(OpenCV_DIR "/usr/local/share/OpenCV")
endif (DEFINED ENV{OpenCV_DIR})
set(WITH_OPENCV_DIR ${OpenCV_DIR} CACHE STRING "OpenCV directory containing OpenCVConfig.cmake")

#
# Find OpenCV libraries.
#
set(OpenCV_DIR ${OpenCV_DIR})
find_package(OpenCV 4 REQUIRED core ts video highgui)

set(ODF_LINK_LIBRARIES ${OpenCV_LIBS})

#
# Define root project.
#
project(odf CXX)

#
# Install headers.
#
file(GLOB odf_HEADERS "${ODF_INCLUDE_DIR}/odf/*.h")

install(
  FILES ${odf_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/odf
  COMPONENT headers
)

#
# Set odf library target name.
#

set(ODF_LIBRARY_TARGET odf-shared)

#
# Include subprojects.
#
add_subdirectory("doc")
add_subdirectory("src/odf")
add_subdirectory("src/samples")
