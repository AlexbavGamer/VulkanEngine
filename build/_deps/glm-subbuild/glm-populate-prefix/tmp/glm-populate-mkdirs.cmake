# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "D:/VulkanEngine/build/_deps/glm-src")
  file(MAKE_DIRECTORY "D:/VulkanEngine/build/_deps/glm-src")
endif()
file(MAKE_DIRECTORY
  "D:/VulkanEngine/build/_deps/glm-build"
  "D:/VulkanEngine/build/_deps/glm-subbuild/glm-populate-prefix"
  "D:/VulkanEngine/build/_deps/glm-subbuild/glm-populate-prefix/tmp"
  "D:/VulkanEngine/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
  "D:/VulkanEngine/build/_deps/glm-subbuild/glm-populate-prefix/src"
  "D:/VulkanEngine/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/VulkanEngine/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/VulkanEngine/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
