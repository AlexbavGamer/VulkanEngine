cmake_minimum_required(VERSION 3.14)
project(VulkanEngine)

# Set policies to avoid warnings
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

message(STATUS "Starting VulkanEngine CMake configuration...")

# Definir o padrão C++ como C++17
message(STATUS "Setting C++ standard to C++17")
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
message(STATUS "Configuring custom Boost paths for Windows...")
set(BOOST_ROOT "C:/Boost")
set(BOOST_INCLUDEDIR "C:/Boost/include/boost-1_88")
set(BOOST_LIBRARYDIR "C:/Boost/lib")

# Desabilitar o auto-link do Boost no Windows
add_definitions(-DBOOST_ALL_NO_LIB)
endif()

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_STATIC_RUNTIME     ON)
set(Boost_USE_MULTITHREADED      ON)

# Encontrar Boost do sistema
message(STATUS "Looking for Boost 1.88.0...")
find_package(Boost 1.88.0 REQUIRED COMPONENTS 
    filesystem
    system
    thread
    chrono
)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Check if paths are correct: BOOST_ROOT=${BOOST_ROOT}, BOOST_INCLUDEDIR=${BOOST_INCLUDEDIR}, BOOST_LIBRARYDIR=${BOOST_LIBRARYDIR}")
endif()

message(STATUS "Boost found:")
message(STATUS "  Version: ${Boost_VERSION}")
message(STATUS "  Include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "  Library dirs: ${Boost_LIBRARY_DIRS}")

# Configuração para diferentes plataformas
message(STATUS "Checking platform-specific configurations...")
if(WIN32)
    message(STATUS "Windows platform detected")
    # Verificação do Vulkan SDK no Windows
    if(NOT DEFINED ENV{VK_SDK_PATH})
        message(FATAL_ERROR "Vulkan SDK não encontrado! Certifique-se de que o Vulkan SDK está instalado e a variável de ambiente VULKAN_SDK está configurada corretamente.")
    endif()

    message(STATUS "Configuring Vulkan SDK paths for Windows...")
    # Configurações específicas para Windows
    set(VULKAN_SDK $ENV{VK_SDK_PATH})
    string(REPLACE "\\" "/" VULKAN_SDK ${VULKAN_SDK})  # Troca barras invertidas por barras normais
    message(STATUS "Vulkan SDK path: ${VULKAN_SDK}")

    set(VULKAN_LIB_DIR "${VULKAN_SDK}/Lib")
    set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/Include")
    set(VULKAN_LIB "${VULKAN_LIB_DIR}/vulkan-1.lib")
    set(GLSLC "${VULKAN_SDK}/Bin/glslc.exe")
    
    # Configurações específicas para MinGW no Windows
    if(MINGW)
        message(STATUS "MinGW detected, configuring specific flags...")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole -mwindows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()

elseif(UNIX)
    message(STATUS "Linux platform detected")
    # Procurar Vulkan SDK no sistema Linux
    message(STATUS "Searching for Vulkan SDK...")
    find_package(Vulkan REQUIRED)
    
    message(STATUS "Configuring Vulkan paths for Linux...")
    # Definir variáveis para Vulkan no Linux
    set(VULKAN_INCLUDE_DIR ${Vulkan_INCLUDE_DIRS})
    set(VULKAN_LIB ${Vulkan_LIBRARIES})
    
    message(STATUS "Searching for GLSL compiler...")
    # Encontrar o compilador de shaders
    find_program(GLSLC glslc REQUIRED)
endif()

# Incluir dependências multiplataforma
message(STATUS "Starting third-party dependencies configuration...")
include(FetchContent)

# GLFW
message(STATUS "Fetching GLFW from GitHub...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# GLM (matemática 3D)
message(STATUS "Fetching GLM (OpenGL Mathematics) from GitHub...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# STB (carregamento de texturas)
message(STATUS "Fetching STB image library from GitHub...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Assimp
message(STATUS "Fetching Assimp (Asset Import Library) from GitHub...")
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1
)

message(STATUS "Configuring Assimp build options...")
# Configurações do Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp tests" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Disable Assimp installation" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Enable ZLIB building" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "Disable building of Assimp samples" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "Disable building of Assimp docs" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "Disable export functionality" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Disable Assimp tools" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "Disable all importers by default" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "Enable OBJ importer" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "Enable FBX importer" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER ON CACHE BOOL "Enable Blender importer" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable warnings as errors" FORCE)

# Configurações específicas para MinGW
if(WIN32 AND MINGW)
    message(STATUS "Configuring MinGW-specific settings for Assimp...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
endif()

FetchContent_MakeAvailable(assimp)

# Magic Enum
message(STATUS "Fetching Magic Enum library from GitHub...")
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.3
)
FetchContent_MakeAvailable(magic_enum)

# ImGui
message(STATUS "Fetching Dear ImGui from GitHub...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

message(STATUS "Setting up ImGui source files...")
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

# ImGuizmo
message(STATUS "Fetching ImGuizmo from GitHub...")
FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG master
)
FetchContent_MakeAvailable(imguizmo)

# Incluir arquivos fonte do ImGuizmo
set(IMGUZMO_SOURCES
    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)

# ImGuiFileDialog
message(STATUS "Fetching ImGui File Dialog from GitHub...")
FetchContent_Declare(
    imguifiledialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG master
)
FetchContent_MakeAvailable(imguifiledialog)
target_include_directories(ImGuiFileDialog PRIVATE
    ${imgui_SOURCE_DIR}
)

message(STATUS "Collecting source files...")
# Encontrar todos os arquivos fonte
file(GLOB_RECURSE SOURCES 
    "src/**.h"
    "src/**.cpp"
)

# Gere os headers de fonte antes de qualquer outra coisa
message(STATUS "Setting up font generation...")
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Find font files
file(GLOB FONT_FILES 
    "${CMAKE_SOURCE_DIR}/engine/fonts/*.ttf"
    "${CMAKE_SOURCE_DIR}/engine/fonts/*.otf"
)

foreach(FONT_FILE ${FONT_FILES})
    get_filename_component(FONT_NAME ${FONT_FILE} NAME_WE)
    string(MAKE_C_IDENTIFIER ${FONT_NAME} FONT_VARIABLE)
    
    set(OUTPUT_FILE "${GENERATED_DIR}/${FONT_NAME}.h")
    
    message(STATUS "Setting up font generation for ${FONT_NAME}")
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND}
            -DFONT_FILE=${FONT_FILE}
            -DFONT_VARIABLE=${FONT_VARIABLE}
            -DOUTPUT_FILE=${OUTPUT_FILE}
            -P "${CMAKE_SOURCE_DIR}/cmake/GenerateFontHeader.cmake"
        DEPENDS ${FONT_FILE}
        COMMENT "Generating header for font ${FONT_NAME}"
        VERBATIM
    )
    
    list(APPEND GENERATED_HEADERS ${OUTPUT_FILE})
endforeach()

message(STATUS "Creating font headers target...")
# Create custom target for font headers
add_custom_target(FontHeaders ALL DEPENDS ${GENERATED_HEADERS})

message(STATUS "Creating executable target...")
# Criar executável
add_executable(VulkanEngine ${SOURCES} ${IMGUI_SOURCES} ${GENERATED_HEADERS})
target_sources(VulkanEngine PRIVATE 
    ${imguifiledialog_SOURCE_DIR}/ImGuiFileDialog.cpp
)
target_sources(VulkanEngine PRIVATE ${IMGUZMO_SOURCES})

message(STATUS "Configuring include directories...")
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
    ${imguifiledialog_SOURCE_DIR}
    ${imguizmo_SOURCE_DIR}
    ${GENERATED_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

message(STATUS "Setting up library linkage...")
# Links de biblioteca
target_link_libraries(VulkanEngine
    ${VULKAN_LIB}
    glfw
    assimp
    Boost::filesystem
    Boost::system
    Boost::thread
    Boost::chrono
)

message(STATUS "Setting up shader compilation...")
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
    
    message(STATUS "Setting up compilation for shader: ${SHADER_NAME}")
    add_custom_command(
        OUTPUT ${SPIRV_BINARY}
        COMMAND ${GLSLC} ${SHADER} -o ${SPIRV_BINARY}
        DEPENDS ${SHADER}
        COMMENT "Compilando shader ${SHADER_NAME}"
        VERBATIM
    )
endforeach()

message(STATUS "Creating shader build target...")
# Target para shaders
add_custom_target(ShadersBuild ALL DEPENDS ${SPIRV_BINARY_FILES})

# Dependência de shaders
add_dependencies(VulkanEngine ShadersBuild FontHeaders)

message(STATUS "Setting up asset copying...")
# Copiar pasta assets para o diretório de build
add_custom_command(
    TARGET VulkanEngine PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_BINARY_DIR}/engine
    COMMENT "Copiando pasta assets para o diretório de build"
)

# Configurações adicionais para Linux
if(UNIX)
    message(STATUS "Configuring additional Linux settings...")
    # Links adicionais para Linux
    target_link_libraries(VulkanEngine
        dl      # Biblioteca de carregamento dinâmico
        pthread # Suporte a threads
    )

    # Flags adicionais para Linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

message(STATUS "CMake configuration completed successfully!")