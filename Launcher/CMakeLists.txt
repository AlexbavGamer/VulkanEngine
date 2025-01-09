cmake_minimum_required(VERSION 3.21)
project(Launcher LANGUAGES CXX)

find_package(Slint QUIET)
if(NOT Slint_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Slint
        GIT_REPOSITORY https://github.com/slint-ui/slint.git
        GIT_TAG release/1
        SOURCE_SUBDIR api/cpp
    )
    FetchContent_MakeAvailable(Slint)
endif()

# Coletar todos os arquivos de código-fonte
file(GLOB_RECURSE SRC_FILES
    "src/**.h"
    "src/**.cpp"
)

# Coletar todos os arquivos .slint
file(GLOB SLINT_FILES "ui/*.slint")

# Criar o executável
add_executable(Launcher ${SRC_FILES})

# Processar arquivos Slint
slint_target_sources(Launcher ${SLINT_FILES})

# Linkar a biblioteca Slint
target_link_libraries(Launcher PRIVATE Slint::Slint)

# Configuração específica para Windows
if(WIN32)
    add_custom_command(
        TARGET Launcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Launcher> $<TARGET_FILE_DIR:Launcher>
        COMMAND_EXPAND_LISTS
    )
endif()
