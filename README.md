# VulkanEngine

Engine 3D baseada em Vulkan com suporte para Windows e Linux.

## Pré-requisitos

### Windows
- Visual Studio 2019 ou superior
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows)
- CMake 3.14 ou superior
- Boost (C:\Boost)

### Linux
- GCC/G++ 8.0 ou superior
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#linux)
- CMake 3.14 ou superior
- Dependências necessárias:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake libboost-all-dev libvulkan-dev vulkan-tools libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

## Configuração do Ambiente

### Windows
1. Instale o Vulkan SDK
2. Configure a variável de ambiente VULKAN_SDK
3. Execute o script setup.bat

### Linux
1. Instale o Vulkan SDK seguindo as instruções do site oficial
2. Configure a variável de ambiente VULKAN_SDK:
   ```bash
   export VULKAN_SDK=/path/to/vulkan/sdk
   ```
3. Torne o script setup.sh executável e execute-o:
   ```bash
   chmod +x setup.sh
   ./setup.sh
   ```

## Compilação

### Windows
```batch
setup.bat
```

### Linux
```bash
./setup.sh
```

## Execução
Após a compilação, o executável estará localizado em:

### Windows
```
build/VulkanEngine.exe
```

### Linux
```
build/VulkanEngine
```

## Solução de Problemas

### Linux
Se encontrar erros relacionados ao Vulkan:
1. Verifique se o driver Vulkan está instalado:
   ```bash
   vulkaninfo
   ```
2. Certifique-se de que seu hardware suporta Vulkan
3. Verifique se todas as dependências estão instaladas

### Windows
Se encontrar erros relacionados ao Vulkan:
1. Verifique se o Vulkan SDK está instalado corretamente
2. Confirme se a variável de ambiente VULKAN_SDK está configurada
3. Verifique se seu GPU tem suporte a Vulkan