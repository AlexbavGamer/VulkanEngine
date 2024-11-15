cmake_minimum_required(VERSION 3.14)
project(VulkanEngine)

# Definir o padrão C++ como C++17
set(CMAKE_CXX_STANDARD 17)

# Configurar o Vulkan SDK no Windows
if(WIN32)
    if(NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR "Vulkan SDK não encontrado! Certifique-se de que o Vulkan SDK está instalado e a variável de ambiente VULKAN_SDK está configurada corretamente.")
    endif()

    set(VULKAN_SDK $ENV{VULKAN_SDK})
    string(REPLACE "\\" "/" VULKAN_SDK ${VULKAN_SDK})  # Troca barras invertidas por barras normais

    set(VULKAN_LIB_DIR "${VULKAN_SDK}/Lib")
    set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/Include")
    set(VULKAN_LIB "${VULKAN_LIB_DIR}/vulkan-1.lib")  # Caminho para vulkan-1.lib
    set(GLSLC "${VULKAN_SDK}/Bin/glslc.exe")          # Caminho para o compilador de shaders
elseif(UNIX)
    set(VULKAN_SDK_URL "https://sdk.lunarg.com/sdk/download/latest/linux/vulkan-sdk.tar.gz")
    set(VULKAN_LIB_DIR "${vulkan_sdk_SOURCE_DIR}/lib")
    set(VULKAN_LIB libvulkan.so)
    set(VULKAN_INCLUDE_DIR "${vulkan_sdk_SOURCE_DIR}/include")
    set(GLSLC "glslc")  # Caminho para o compilador de shaders em sistemas Unix
endif()

# Incluir GLFW (para a janela e controle de inputs)
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# Incluir GLM (para matemática 3D)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# Incluir STB (para carregamento de texturas)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Incluir ImGui (para GUI)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# ImGui implementation files
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

# Arquivos fonte e headers
file(GLOB_RECURSE SOURCES 
    "src/**.h"
    "src/**.cpp"
)

# Criar o executável
add_executable(VulkanEngine ${SOURCES} ${IMGUI_SOURCES})

# Garantir que o GLFW_INCLUDE_DIRS seja configurado corretamente
set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)

# Verifique os caminhos do GLFW e GLM
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
)

# Ligar ao Vulkan, GLFW e GLM
target_link_libraries(VulkanEngine
    "${VULKAN_LIB}"
    glfw
)

# Configuração dos shaders
file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/shaders/*.vert" "${CMAKE_SOURCE_DIR}/shaders/*.frag")

# Diretório de saída dos shaders compilados
set(COMPILED_SHADERS_DIR "${CMAKE_BINARY_DIR}/shaders")

# Cria o diretório de saída
file(MAKE_DIRECTORY ${COMPILED_SHADERS_DIR})

# Comando para compilar cada shader
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    add_custom_command(
        OUTPUT "${COMPILED_SHADERS_DIR}/${SHADER_NAME}.spv"
        COMMAND "${GLSLC}" "${SHADER}" -o "${COMPILED_SHADERS_DIR}/${SHADER_NAME}.spv"
        DEPENDS ${SHADER}
        COMMENT "Compilando shader ${SHADER_NAME} para SPIR-V"
        VERBATIM
    )
    list(APPEND SPIRV_SHADERS "${COMPILED_SHADERS_DIR}/${SHADER_NAME}.spv")
endforeach()

# Alvo para compilar shaders, marcado como ALL para forçar execução na build principal
add_custom_target(ShadersBuild ALL DEPENDS ${SPIRV_SHADERS})

# Garante que os shaders serão compilados antes do executável
add_dependencies(VulkanEngine ShadersBuild)

# Para MinGW no Windows, garantir que estamos compilando um aplicativo de console
if(WIN32 AND MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole -mwindows")
endif()

# Caso precise de configurações específicas para o MinGW (como compilar estático)
if(WIN32 AND MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()
