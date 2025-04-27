#include "VulkanRenderer.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <memory>
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include <typeinfo>
#include <chrono>

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

void showError(const std::string &message)
{
#ifdef _WIN32
    MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR | MB_OK);
#else
    std::cerr << "Error: " << message << std::endl;
#endif
}

int main()
{
    std::ofstream logFile("engine_log.txt", std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        logFile << "\n[" << std::ctime(&time) << "] Iniciando engine...\n";
    }

    try {
        logFile << "[INFO] Inicializando GLFW...\n";
        if (!glfwInit()) {
            logFile << "[ERRO] Falha ao inicializar GLFW\n";
            showError("GLFW failed to initialize");
            return 1;
        }

        logFile << "[INFO] Verificando suporte ao Vulkan...\n";
        if (!glfwVulkanSupported()) {
            logFile << "[ERRO] Vulkan não é suportado neste sistema\n";
            showError("Vulkan is not supported on this system");
            glfwTerminate();
            return 1;
        }

        logFile << "[INFO] Obtendo monitor primário...\n";
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        if (!primaryMonitor) {
            logFile << "[ERRO] Falha ao obter monitor primário\n";
            showError("Failed to get primary monitor");
            glfwTerminate();
            return 1;
        }

        logFile << "[INFO] Obtendo modo de vídeo...\n";
        const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
        if (!mode) {
            logFile << "[ERRO] Falha ao obter modo de vídeo\n";
            showError("Failed to get video mode");
            glfwTerminate();
            return 1;
        }

        WIDTH = mode->width;
        HEIGHT = mode->height;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        
        logFile << "[INFO] Criando janela " << WIDTH << "x" << HEIGHT << "...\n";
        GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);

        if (!window) {
            logFile << "[ERRO] Falha ao criar janela GLFW\n";
            showError("GLFW failed to create window");
            glfwTerminate();
            return 1;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        logFile << "[INFO] Iniciando VulkanRenderer...\n";
        VulkanRenderer &renderer = VulkanRenderer::getInstance();
        try {
            logFile << "[INFO] Inicializando Vulkan...\n";
            renderer.initVulkan(window);

            logFile << "[INFO] Configurando callbacks...\n";
            glfwSetWindowUserPointer(window, renderer.getCore()->getScene());
            glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
            {
                Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
                scene->handleMouseInput(window, xpos, ypos); 
            });
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Começa com o cursor desabilitado
            CameraComponent::isCursorEnabled() = false; // Sincroniza o estado do cursor

            Scene* scene = renderer.getCore()->getScene();

            std::shared_ptr<Entity> plano = scene->createEntity();

            if(!renderer.getModelLoader()->LoadModel("engine/models/plano.fbx", plano))
            {
                logFile << "[ERRO] Falha ao carregar modelo\n";
                std::cout << "Failed to load model" << std::endl;
            }

            std::shared_ptr<Entity> cameraEntity = scene->createEntity();
            cameraEntity->setName("Camera");
            CameraComponent camera = cameraEntity->addComponent<CameraComponent>();
            camera.width = WIDTH;
            camera.height = HEIGHT;
            scene->cameraEntity = cameraEntity;

            std::shared_ptr<Entity> lightEntity = scene->createLightEntity();

            logFile << "[INFO] Iniciando loop principal...\n";
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();

                // ImGui input handling
                ImGuiIO &io = ImGui::GetIO();
                Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
                
                // Processa input do teclado primeiro
                scene->handleKeyboardInput(window);

                // Trata o mouse baseado no estado do cursor
                if (!CameraComponent::isCursorEnabled())
                {
                    // Camera control mode
                    double mouseX, mouseY;
                    glfwGetCursorPos(window, &mouseX, &mouseY);
                    scene->handleMouseInput(window, mouseX, mouseY);
                }
                else
                {
                    // UI mode - update ImGui mouse info
                    double mouseX, mouseY;
                    glfwGetCursorPos(window, &mouseX, &mouseY);
                    io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
                }

                // Mouse button states for ImGui
                io.MouseDown[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
                io.MouseDown[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
                io.MouseDown[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

                scene->updateCamera();
                renderer.getCore()->renderFrame();
            }

            logFile << "[INFO] Encerrando normalmente\n";
        }
        catch (const std::exception &e) {
            logFile << "[ERRO FATAL] Exceção durante execução: " << e.what() << "\n";
            showError("Initialization failed: " + std::string(e.what()));
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    catch (const std::exception& e) {
        logFile << "[ERRO FATAL] Exceção não tratada: " << e.what() << "\n";
        showError(std::string("Critical error: ") + e.what());
        return 1;
    }
    catch (...) {
        logFile << "[ERRO FATAL] Erro desconhecido ocorreu\n";
        showError("Unknown error occurred");
        return 1;
    }

    logFile << "[INFO] Programa finalizado com sucesso\n";
    return 0;
}
