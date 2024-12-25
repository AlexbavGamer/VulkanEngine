cmake_minimum_required(VERSION 3.14)
project(VulkanEngine)

# Definir o padrão C++ como C++17
set(CMAKE_CXX_STANDARD 17)

# Configuração para diferentes plataformas
if(WIN32)
    # Verificação do Vulkan SDK no Windows
    if(NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR "Vulkan SDK não encontrado! Certifique-se de que o Vulkan SDK está instalado e a variável de ambiente VULKAN_SDK está configurada corretamente.")
    endif()

    # Configurações específicas para Windows
    set(VULKAN_SDK $ENV{VULKAN_SDK})
    string(REPLACE "\\" "/" VULKAN_SDK ${VULKAN_SDK})  # Troca barras invertidas por barras normais

    set(VULKAN_LIB_DIR "${VULKAN_SDK}/Lib")
    set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/Include")
    set(VULKAN_LIB "${VULKAN_LIB_DIR}/vulkan-1.lib")  # Caminho para vulkan-1.lib
    set(GLSLC "${VULKAN_SDK}/Bin/glslc.exe")          # Caminho para o compilador de shaders
    
    # Configurações específicas para MinGW no Windows
    if(MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole -mwindows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()

elseif(UNIX)
    # Procurar Vulkan SDK no sistema Linux
    find_package(Vulkan REQUIRED)
    
    # Definir variáveis para Vulkan no Linux
    set(VULKAN_INCLUDE_DIR ${Vulkan_INCLUDE_DIRS})
    set(VULKAN_LIB ${Vulkan_LIBRARIES})
    
    # Encontrar o compilador de shaders
    find_program(GLSLC glslc REQUIRED)
endif()

# Incluir dependências multiplataforma
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# GLM (matemática 3D)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# STB (carregamento de texturas)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Assimp
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1
)

# Configurações do Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "Disable building of Assimp samples")
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "Disable building of Assimp docs")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

# Configurações específicas para MinGW
if(WIN32 AND MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
endif()

FetchContent_MakeAvailable(assimp)

# Magic Enum
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.3
)
FetchContent_MakeAvailable(magic_enum)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# Arquivos fonte do ImGui
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

# Encontrar todos os arquivos fonte
file(GLOB_RECURSE SOURCES 
    "src/**.h"
    "src/**.cpp"
)

# Criar executável
add_executable(VulkanEngine ${SOURCES} ${IMGUI_SOURCES})

# Configurar diretórios de inclusão
set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)

# Mensagens de depuração
message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
message(STATUS "GLM_INCLUDE_DIR: ${glm_SOURCE_DIR}")

# Adicionar diretórios de inclusão
target_include_directories(VulkanEngine PRIVATE
    ${VULKAN_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${glm_SOURCE_DIR}/glm
    ${stb_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/imgui
    ${assimp_BINARY_DIR}/include
    ${magic_enum_SOURCE_DIR}/include
)

# Links de biblioteca
target_link_libraries(VulkanEngine
    ${VULKAN_LIB}
    glfw
    assimp
)

# Configuração de shaders
file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/shaders/*.vert" "${CMAKE_SOURCE_DIR}/shaders/*.frag")

# Diretório de saída dos shaders compilados
set(COMPILED_SHADERS_DIR "${CMAKE_BINARY_DIR}/shaders")

# Criar diretório de saída
file(MAKE_DIRECTORY ${COMPILED_SHADERS_DIR})

# Limpar listas
set(SPIRV_BINARY_FILES "")
set(SHADER_COMMANDS "")

# Compilação de shaders
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    get_filename_component(SHADER_EXT ${SHADER} EXT)
    set(SPIRV_BINARY "${COMPILED_SHADERS_DIR}/${SHADER_NAME}.spv")
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_BINARY})
    
    add_custom_command(
        OUTPUT ${SPIRV_BINARY}
        COMMAND ${GLSLC} ${SHADER} -o ${SPIRV_BINARY}
        DEPENDS ${SHADER}
        COMMENT "Compilando shader ${SHADER_NAME}"
        VERBATIM
    )
endforeach()

# Target para shaders
add_custom_target(ShadersBuild ALL DEPENDS ${SPIRV_BINARY_FILES})

# Dependência de shaders
add_dependencies(VulkanEngine ShadersBuild)

# Copiar pasta assets para o diretório de build
add_custom_command(
    TARGET VulkanEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copiando pasta assets para o diretório de build"
)

# Configurações adicionais para Linux
if(UNIX)
    # Links adicionais para Linux
    target_link_libraries(VulkanEngine
        dl      # Biblioteca de carregamento dinâmico
        pthread # Suporte a threads
    )

    # Flags adicionais para Linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()