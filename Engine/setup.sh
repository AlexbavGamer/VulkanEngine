#!/bin/bash

echo "Configurando ambiente de build para VulkanEngine no Linux..."

# Verificar se o Vulkan SDK está instalado
if [ -z "$VULKAN_SDK" ]; then
    echo "ERRO: Vulkan SDK não encontrado!"
    echo "Por favor, instale o Vulkan SDK e configure a variável de ambiente VULKAN_SDK"
    exit 1
fi

# Verificar dependências necessárias
command -v cmake >/dev/null 2>&1 || { echo "ERRO: cmake não encontrado. Por favor instale cmake"; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo "ERRO: g++ não encontrado. Por favor instale g++"; exit 1; }

# Criar e entrar no diretório de build
echo "Criando diretório de build..."
mkdir -p build
cd build

# Configurar CMake
echo "Configurando CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar o projeto
echo "Compilando o projeto..."
cmake --build . -- -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build completado com sucesso!"
else
    echo "ERRO: Falha no build"
    exit 1
fi

# Tornar o executável executável
chmod +x VulkanEngine

echo "Configuração concluída!"