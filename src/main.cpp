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
#include "engine/loaders/ModelLoader.h"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

void showError(const std::string& message) {
#ifdef _WIN32
    MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR | MB_OK);
#else
    std::cerr << "Error: " << message << std::endl;
#endif
}

int main() {
    if (!glfwInit()) {
        showError("GLFW failed to initialize");
        return 1;
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    WIDTH = mode->width;
    HEIGHT = mode->height;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);

    if (!window) {
        showError("GLFW failed to create window");
        glfwTerminate();
        return 1;
    }

    VulkanRenderer& renderer = VulkanRenderer::getInstance();
    try {
        renderer.initVulkan(window);
        
        glfwSetWindowUserPointer(window, renderer.getCore()->getScene());
        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
            Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
            scene->handleMouseInput(window, xpos, ypos);
        });
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        Scene& scene = *renderer.getCore()->getScene();

        EngineModelLoader modelLoader(renderer);

        // Create entity using ECS
        std::shared_ptr<Entity> entity = scene.registry->createEntity();

        // Ajustar posição inicial da câmera e cubo
        if(modelLoader.LoadModel("engine/models/cubo.fbx", entity)) {
            TransformComponent& transform = entity->addComponent<TransformComponent>();
            transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
            transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        // Ajustar câmera para ver o cubo
        CameraComponent camera;
        camera.width = WIDTH;
        camera.height = HEIGHT;
        camera::updateCamera(camera, 
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, -1.0f), 
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        
        LightComponent light;
        light.position = glm::vec3(15.0f, 10.0f, 0.0f);  // Initial sun position
        light.color = glm::vec3(1.0f, 0.95f, 0.8f);      // Warm sunlight color
        light.intensity = 1.5f;                           // Bright sunlight
        scene.camera = camera;
        scene.lights.push_back(light);

    } catch (const std::exception& e) {
        showError("Initialization failed: " + std::string(e.what()));
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window)) {    
        glfwPollEvents();
        renderer.getCore()->getScene()->handleKeyboardInput(window);

        // ImGui input handling
        ImGuiIO& io = ImGui::GetIO();
        
        // Capture mouse position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));

        // Mouse button states
        io.MouseDown[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        io.MouseDown[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        io.MouseDown[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

        // Scroll wheel
        glfwSetScrollCallback(window, [](GLFWwindow*, double, double yoffset) {
            ImGuiIO& io = ImGui::GetIO();
            io.MouseWheel += static_cast<float>(yoffset);
        });

        // Keyboard input
        io.KeyCtrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                    glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        io.KeyShift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
                    glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        io.KeyAlt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || 
                    glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
        io.KeySuper = glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || 
                    glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS;

        // Character input callback
        glfwSetCharCallback(window, [](GLFWwindow*, unsigned int c) {
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharacter(c);
        });

        renderer.getCore()->renderFrame();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
