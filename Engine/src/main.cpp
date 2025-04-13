#include "VulkanRenderer.h"
#include <GLFW/glfw3.h>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <memory>
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include <typeinfo>

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
    if (!glfwInit())
    {
        showError("GLFW failed to initialize");
        return 1;
    }

    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
    WIDTH = mode->width;
    HEIGHT = mode->height;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);

    if (!window)
    {
        showError("GLFW failed to create window");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    VulkanRenderer &renderer = VulkanRenderer::getInstance();
    try
    {
        renderer.initVulkan(window);

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
            std::cout << "Failed to load model" << std::endl;
        }

        std::shared_ptr<Entity> cameraEntity = scene->createEntity();
        cameraEntity->setName("Camera");
        CameraComponent camera = cameraEntity->addComponent<CameraComponent>();
        camera.width = WIDTH;
        camera.height = HEIGHT;
        scene->cameraEntity = cameraEntity;

        std::shared_ptr<Entity> lightEntity = scene->createLightEntity();
    }
    catch (const std::exception &e)
    {
        showError("Initialization failed: " + std::string(e.what()));
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window))
    {
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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
