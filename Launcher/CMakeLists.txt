# Definição mínima do CMake
cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(Launcher)

# Defina o padrão C++ para 17 ou superior
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Diretório de origem
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Busca automaticamente todos os arquivos .cpp e .h no diretório src
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)

# Cria o executável para o launcher
add_executable(Launcher ${SOURCES} ${HEADERS})

# Inclui a pasta Engine, se necessário
include_directories(${CMAKE_SOURCE_DIR}/Engine)

# Linkar o launcher com a biblioteca Engine (se necessário)
target_link_libraries(Launcher PRIVATE Engine)

# Adiciona flags específicas para Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(Launcher PRIVATE pthread dl)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()