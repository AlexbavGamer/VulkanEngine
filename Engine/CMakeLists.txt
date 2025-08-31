project(VulkanEngine)

# Set policies to avoid warnings
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

message(STATUS "Starting VulkanEngine CMake configuration...")

# Para Linux, verificar e instalar dependências necessárias
if(UNIX)
    message(STATUS "Checking Linux dependencies...")

    # Verificar pacotes necessários
    find_package(PkgConfig REQUIRED)

    # Tentar encontrar GLFW3 usando pkg-config primeiro
    pkg_check_modules(GLFW3 glfw3)

    # Se pkg-config falhar para GLFW3, tentar encontrar diretamente
    if(NOT GLFW3_FOUND)
        # Tentar encontrar os headers do GLFW diretamente
        find_path(GLFW_INCLUDE_PATH GLFW/glfw3.h
            PATHS
            /usr/include
            /usr/local/include
            /usr/include/GLFW
            /usr/local/include/GLFW
        )

        if(GLFW_INCLUDE_PATH AND GLFW_LIBRARY)
            message(STATUS "Encontrados headers do GLFW em: ${GLFW_INCLUDE_PATH}")
            message(STATUS "Encontrada biblioteca do GLFW em: ${GLFW_LIBRARY}")
            set(GLFW3_FOUND TRUE)
            set(GLFW3_INCLUDE_DIRS ${GLFW_INCLUDE_PATH})
            set(GLFW3_LIBRARIES ${GLFW_LIBRARY})
        else()
            message(FATAL_ERROR "GLFW3 não encontrado! Por favor, instale com: sudo pacman -S glfw-x11 ou sudo apt-get install libglfw3-dev")
        endif()
    endif()

    # Tentar encontrar GLM usando pkg-config primeiro
    pkg_check_modules(GLM glm)

    # Se pkg-config falhar, tentar encontrar GLM diretamente
    if(NOT GLM_FOUND)
        # Tentar encontrar os headers do GLM diretamente
        find_path(GLM_INCLUDE_DIR glm/glm.hpp
            PATHS
            /usr/include
            /usr/local/include
            /usr/include/glm
            /usr/local/include/glm
        )

        if(GLM_INCLUDE_DIR)
            message(STATUS "Encontrados headers do GLM em: ${GLM_INCLUDE_DIR}")
            set(GLM_FOUND TRUE)
        else()
            message(FATAL_ERROR "GLM não encontrado! Por favor, instale com: sudo pacman -S glm ou sudo apt-get install libglm-dev")
        endif()
    endif()

    # Verificar Vulkan
    find_package(Vulkan)
    if(NOT Vulkan_FOUND)
        message(FATAL_ERROR "Vulkan não encontrado! Por favor, instale com: sudo pacman -S vulkan-icd-loader vulkan-headers ou sudo apt-get install vulkan-tools libvulkan-dev")
    endif()
endif()

# Definir o padrão C++ como C++17
message(STATUS "Setting C++ standard to C++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurar caminhos do Vulkan SDK
message(STATUS "Configuring Vulkan SDK paths...")
if(WIN32)
    if(NOT DEFINED ENV{VK_SDK_PATH})
        message(FATAL_ERROR "Vulkan SDK não encontrado! Certifique-se de que o Vulkan SDK está instalado e a variável de ambiente VULKAN_SDK está configurada corretamente.")
    endif()

    set(VULKAN_SDK $ENV{VK_SDK_PATH})
    string(REPLACE "\\" "/" VULKAN_SDK "${VULKAN_SDK}")
    set(VULKAN_LIB_DIR "${VULKAN_SDK}/Lib")
    set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/Include")
    set(VULKAN_LIB "${VULKAN_LIB_DIR}/vulkan-1.lib")
    set(GLSLC "${VULKAN_SDK}/Bin/glslc.exe")
    set(SHADER_COMPILER_OPTIONS "")
else()
    find_package(Vulkan REQUIRED)
    if(NOT Vulkan_FOUND)
        message(FATAL_ERROR "Vulkan SDK não encontrado! Por favor, instale o Vulkan SDK.")
    endif()

    set(VULKAN_INCLUDE_DIR ${Vulkan_INCLUDE_DIRS})
    set(VULKAN_LIB ${Vulkan_LIBRARIES})

    # Procurar o glslc em vários locais comuns
    find_program(GLSLC glslc
        PATHS
        /usr/bin
        /usr/local/bin
        ${Vulkan_INCLUDE_DIRS}/../bin
        /usr/lib/x86_64-linux-gnu/
        NO_DEFAULT_PATH
    )

    if(NOT GLSLC)
        # Se não encontrar glslc, usar glslangValidator
        find_program(GLSLC glslangValidator)
        if(NOT GLSLC)
            message(FATAL_ERROR "Nem glslc nem glslangValidator foram encontrados! Por favor, instale o pacote glslang-tools")
        endif()
        set(SHADER_COMPILER_ARGS "-V")
    else()
        set(SHADER_COMPILER_ARGS "")
    endif()
endif()


# Configuração do Boost# Configuração do Boost
message(STATUS "Configuring Boost...")

if(WIN32)
    message(STATUS "Configuring custom Boost paths for Windows...")
    set(BOOST_ROOT "C:/Boost")
    set(BOOST_INCLUDEDIR "C:/Boost/include/boost-1_88")
    set(BOOST_LIBRARYDIR "C:/Boost/lib")

    if(MINGW)
        set(Boost_COMPILER "-mgw14")
    endif()

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME ON)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DBOOST_THREAD_USE_LIB)

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_ROOT}/Modules")

    find_package(Boost REQUIRED COMPONENTS filesystem system thread chrono)
    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Boost não encontrado! Por favor, instale o Boost em C:/Boost")
    endif()
elseif()
    # Linux/macOS
    find_package(Boost 1.71.0 REQUIRED COMPONENTS filesystem system thread chrono)
    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Boost não encontrado! Por favor, instale o Boost (sudo apt-get install libboost-all-dev)")
    endif()
endif()

# Imprimir informações do Boost encontrado
message(STATUS "Boost found:")
message(STATUS "  Version: ${Boost_VERSION}")
message(STATUS "  Include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "  Library dirs: ${Boost_LIBRARY_DIRS}")
message(STATUS "  Libraries: ${Boost_LIBRARIES}")

if(NOT WIN32)
    message(STATUS "Configuring additional Linux libraries...")
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

# Incluir dependências multiplataforma
message(STATUS "Starting third-party dependencies configuration...")
include(FetchContent)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)

# GLFW
message(STATUS "Fetching GLFW from GitHub...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLM (matemática 3D)
message(STATUS "Fetching GLM (OpenGL Mathematics) from GitHub...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
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
    GIT_TAG docking
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


# find all font files in the engine/fonts directory
if(EXISTS "${CMAKE_SOURCE_DIR}/engine/fonts")
    file(GLOB FONT_FILES
        "${CMAKE_SOURCE_DIR}/engine/fonts/*.ttf"
        "${CMAKE_SOURCE_DIR}/engine/fonts/*.otf"
    )
else()
    file(GLOB FONT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/fonts/*.ttf"
        "${CMAKE_CURRENT_SOURCE_DIR}/engine/fonts/*.otf"
    )
endif()

foreach(FONT_FILE ${FONT_FILES})
    get_filename_component(FONT_NAME ${FONT_FILE} NAME_WE)
    string(MAKE_C_IDENTIFIER ${FONT_NAME} FONT_VARIABLE)

    set(OUTPUT_FILE "${GENERATED_DIR}/${FONT_NAME}.h")

    # Tenta usar o caminho padrão, se não existir tenta o alternativo
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/GenerateFontHeader.cmake")
        set(GENERATE_FONT_HEADER_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/GenerateFontHeader.cmake")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateFontHeader.cmake")
        set(GENERATE_FONT_HEADER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateFontHeader.cmake")
    else()
        message(FATAL_ERROR "GenerateFontHeader.cmake não encontrado em nenhum dos caminhos esperados!")
    endif()

    message(STATUS "Setting up font generation for ${FONT_NAME}")
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND}
        -DFONT_FILE=${FONT_FILE}
        -DFONT_VARIABLE=${FONT_VARIABLE}
        -DOUTPUT_FILE=${OUTPUT_FILE}
        -P "${GENERATE_FONT_HEADER_SCRIPT}"
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

# Parte a ser modificada na configuração de diretórios de inclusão
message(STATUS "Configuring include directories...")

# Improve GLFW header detection
if(UNIX)
    message(STATUS "Checking for GLFW headers on Linux...")

    # Try multiple possible paths for GLFW headers
    find_path(GLFW_INCLUDE_PATH GLFW/glfw3.h
        PATHS
        /usr/include
        /usr/local/include
        /usr/include/GLFW
        /usr/local/include/GLFW
        ${GLFW_INCLUDE_DIRS}
        ${GLFW3_INCLUDE_DIRS}
        ${glfw_SOURCE_DIR}/include
    )

    if(GLFW_INCLUDE_PATH)
        message(STATUS "Found GLFW include path: ${GLFW_INCLUDE_PATH}")
        set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_PATH})
    else()
        message(STATUS "GLFW headers not found in system paths, using fetched version")
        set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)
    endif()
else()
    set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)
endif()

message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
message(STATUS "GLM_INCLUDE_DIR: ${GLM_INCLUDE_DIR}")

# Adicionar diretórios de inclusão
target_include_directories(VulkanEngine PRIVATE
    ${VULKAN_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    $<IF:$<BOOL:${GLM_INCLUDE_DIR}>,${GLM_INCLUDE_DIR},${glm_SOURCE_DIR}>
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
    ${glfw_SOURCE_DIR}/include # Adiciona o diretório de include do GLFW diretamente
)

# Adicionar inclusões específicas para Linux
if(UNIX)
    message(STATUS "Adding Linux-specific include directories...")
    find_path(GLFW3_INCLUDE_DIR GLFW/glfw3.h
        PATHS
        /usr/include
        /usr/local/include
        ${GLFW3_INCLUDE_DIRS}
    )
    if(GLFW3_INCLUDE_DIR)
        message(STATUS "Found GLFW3 include directory: ${GLFW3_INCLUDE_DIR}")
        target_include_directories(VulkanEngine PRIVATE ${GLFW3_INCLUDE_DIR})
    endif()
endif()

message(STATUS "Setting up library linkage...")
# Links de biblioteca
if(UNIX)
    message(STATUS "Adding Linux-specific libraries...")
    target_link_libraries(VulkanEngine
        ${VULKAN_LIB}
        glfw
        assimp
        Boost::filesystem
        Boost::system
        Boost::thread
        Boost::chrono
        ${CMAKE_THREAD_LIBS_INIT}
        ${X11_LIBRARIES}
        dl
        pthread
        m
    )

    target_include_directories(VulkanEngine PRIVATE
        ${X11_INCLUDE_DIR}
    )
else()
    target_link_libraries(VulkanEngine
        ${VULKAN_LIB}
        glfw
        assimp
        Boost::filesystem
        Boost::system
        Boost::thread
        Boost::chrono
    )
endif()

message(STATUS "Setting up shader compilation...")
# Configuração de shaders
file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/engine/shaders/*.vert" "${CMAKE_SOURCE_DIR}/engine/shaders/*.frag")

# Diretório de saída dos shaders compilados
set(COMPILED_SHADERS_DIR "${CMAKE_SOURCE_DIR}/build/Engine/engine/shaders")

# Criar diretório de saída
file(MAKE_DIRECTORY ${COMPILED_SHADERS_DIR})

# Limpar listas
set(SPIRV_BINARY_FILES "")
set(SHADER_COMMANDS "")

# Debug information
message(STATUS "GLSLC compiler path: ${GLSLC}")
message(STATUS "Shader compiler args: ${SHADER_COMPILER_ARGS}")
message(STATUS "Number of shaders found: ${SHADERS}")

# Compilação de shaders
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    get_filename_component(SHADER_EXT ${SHADER} EXT)
    set(SPIRV_BINARY "${COMPILED_SHADERS_DIR}\\${SHADER_NAME}.spv")
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_BINARY})

    message(STATUS "Setting up compilation for shader: ${SHADER_NAME} -> ${SPIRV_BINARY}")
    add_custom_command(
        OUTPUT "${SPIRV_BINARY}"
        COMMAND ${GLSLC} ${SHADER_COMPILER_ARGS} "${SHADER}" -o "${SPIRV_BINARY}"
        DEPENDS "${SHADER}"
        COMMENT "Compilando shader ${SHADER_NAME}"
        VERBATIM
    )
endforeach()

# Make sure we have shaders to build
if(NOT SPIRV_BINARY_FILES)
    message(WARNING "No shader files found in ${CMAKE_SOURCE_DIR}/shaders/. Shader build target will be empty.")
endif()

message(STATUS "Creating shader build target...")
# Target para shaders
add_custom_target(ShadersBuild ALL DEPENDS ${SPIRV_BINARY_FILES})

# Dependência de shaders
add_dependencies(VulkanEngine ShadersBuild FontHeaders)

message(STATUS "Setting up asset copying...")
if(EXISTS "${CMAKE_SOURCE_DIR}/engine")
    add_custom_command(
        TARGET VulkanEngine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/engine
        ${CMAKE_BINARY_DIR}/engine
        COMMENT "Copiando pasta assets de ${CMAKE_SOURCE_DIR}/engine para o diretório de build"
    )
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/engine")
    add_custom_command(
        TARGET VulkanEngine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
        ${CMAKE_BINARY_DIR}/engine
        COMMENT "Copiando pasta assets de ${CMAKE_CURRENT_SOURCE_DIR}/engine para o diretório de build"
    )
else()
    message(WARNING "Diretório 'engine' não encontrado em ${CMAKE_SOURCE_DIR} nem em ${CMAKE_CURRENT_SOURCE_DIR}. A cópia de assets será ignorada.")
endif()

# Configurações adicionais para Linux
if(UNIX)
    message(STATUS "Configuring additional Linux settings...")
    # Links adicionais para Linux
    target_link_libraries(VulkanEngine
        dl # Biblioteca de carregamento dinâmico
        pthread # Suporte a threads
    )

    # Flags adicionais para Linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

message(STATUS "CMake configuration completed successfully!")