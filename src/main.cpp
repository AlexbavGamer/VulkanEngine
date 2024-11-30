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

void createSphere(MeshComponent& mesh, float radius, int stacks, int sectors) 
{
    VulkanRenderer& renderer = VulkanRenderer::getInstance();
    
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // Gerar vértices e normais
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * M_PI / stacks;  // de +PI/2 a -PI/2
        float xy = radius * cosf(stackAngle);             // raio no plano XZ
        float z = radius * sinf(stackAngle);              // altura do vértice

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * M_PI / sectors;   // de 0 a 2PI

            // Posição do vértice
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            // Normal (normalizada para uma esfera)
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;

            // Cor (vamos usar um degradê baseado na posição Y)
            float r = (y + radius) / (2 * radius);
            float g = (z + radius) / (2 * radius);
            float b = (x + radius) / (2 * radius);

            // Adicionar posição, cor e normal ao vetor de vértices
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(r);  // Cor R
            vertices.push_back(g);  // Cor G
            vertices.push_back(b);  // Cor B
            vertices.push_back(nx); // Normal X
            vertices.push_back(ny); // Normal Y
            vertices.push_back(nz); // Normal Z
        }
    }

    // Gerar índices para triângulos
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int current = i * (sectors + 1) + j;
            int next = current + sectors + 1;

            // Primeiro triângulo
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // Segundo triângulo
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    // Criar buffers e enviar dados para a GPU
    renderer.getCore()->createBuffer(vertices.size() * sizeof(float), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.vertexBuffer, mesh.vertexBufferMemory);
    renderer.getCore()->createBuffer(indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.indexBuffer, mesh.indexBufferMemory);

    renderer.getCore()->copyDataToBuffer(vertices.data(), mesh.vertexBufferMemory, vertices.size() * sizeof(float));
    renderer.getCore()->copyDataToBuffer(indices.data(), mesh.indexBufferMemory, indices.size() * sizeof(uint32_t));

    mesh.indexCount = static_cast<uint32_t>(indices.size());
}


void createCube(MeshComponent& mesh) 
{
    VulkanRenderer& renderer = VulkanRenderer::getInstance();
    float vertices[] = {
        // Position           // Color            // Normal
        // Front face (Red)
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
        // Back face (Green)
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        // Top face (Blue)
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
        // Bottom face (Yellow)
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        // Right face (Magenta)
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f, 0.0f,
        // Left face (Cyan)
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, -1.0f,  0.0f, 0.0f
    };
    uint32_t indices[] = {
        0, 2, 1,  2, 0, 3,    // Front (Red)
        4, 5, 6,  6, 7, 4,    // Back (Green)
        8, 9, 10, 10, 11, 8,  // Top (Blue)
        12, 13, 14, 14, 15, 12, // Bottom (Yellow)
        16, 17, 18, 18, 19, 16, // Right (Magenta)
        20, 21, 22, 22, 23, 20  // Left (Cyan)
    };
    
    renderer.getCore()->createBuffer(sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.vertexBuffer, mesh.vertexBufferMemory);
    renderer.getCore()->createBuffer(sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.indexBuffer, mesh.indexBufferMemory);
    
    renderer.getCore()->copyDataToBuffer(vertices, mesh.vertexBufferMemory, sizeof(vertices));
    renderer.getCore()->copyDataToBuffer(indices, mesh.indexBufferMemory, sizeof(indices));

    mesh.indexCount = sizeof(indices) / sizeof(indices[0]);
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

        if(modelLoader.LoadModel("assets/models/cubo.fbx", entity)) {
            TransformComponent& transform = entity->addComponent<TransformComponent>();
            transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
            transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        // Setup camera
        CameraComponent camera;
        camera.width = WIDTH;
        camera.height = HEIGHT;
        camera::updateCamera(camera, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
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