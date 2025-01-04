# Install script for directory: D:/VulkanEngine/build/_deps/assimp-src/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/MultiProject")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/ucrt64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.3.0-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/VulkanEngine/build/_deps/assimp-build/lib/libassimpd.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/anim.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/aabb.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/ai_assert.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/camera.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/color4.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/color4.inl"
    "D:/VulkanEngine/build/_deps/assimp-build/code/../include/assimp/config.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/ColladaMetaData.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/commonMetaData.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/defs.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/cfileio.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/light.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/material.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/material.inl"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/matrix3x3.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/matrix3x3.inl"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/matrix4x4.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/matrix4x4.inl"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/mesh.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/ObjMaterial.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/pbrmaterial.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/GltfMaterial.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/postprocess.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/quaternion.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/quaternion.inl"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/scene.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/metadata.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/texture.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/types.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/vector2.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/vector2.inl"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/vector3.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/vector3.inl"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/version.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/cimport.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/AssertHandler.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/importerdesc.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Importer.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/DefaultLogger.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/ProgressHandler.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/IOStream.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/IOSystem.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Logger.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/LogStream.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/NullLogger.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/cexport.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Exporter.hpp"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/DefaultIOStream.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/DefaultIOSystem.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/ZipArchiveIOSystem.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/SceneCombiner.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/fast_atof.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/qnan.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/BaseImporter.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Hash.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/MemoryIOWrapper.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/ParsingUtils.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/StreamReader.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/StreamWriter.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/StringComparison.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/StringUtils.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/SGSpatialSort.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/GenericProperty.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/SpatialSort.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/SkeletonMeshBuilder.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/SmallVector.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/SmoothingGroups.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/SmoothingGroups.inl"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/StandardShapes.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/RemoveComments.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Subdivision.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Vertex.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/LineSplitter.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/TinyFormatter.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Profiler.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/LogAux.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Bitmap.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/XMLTools.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/IOStreamBuffer.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/CreateAnimMesh.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/XmlParser.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/BlobIOSystem.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/MathFunctions.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Exceptional.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/ByteSwapper.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Compiler/pushpack1.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Compiler/poppack1.h"
    "D:/VulkanEngine/build/_deps/assimp-src/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/VulkanEngine/build/_deps/assimp-build/code/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
