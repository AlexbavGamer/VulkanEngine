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

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

void showError(const std::string& message) {
#ifdef _WIN32
    MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR | MB_OK);
#else
    std::cerr << "Error: " << message << std::endl;
#endif
}

void createSphere(MeshComponent& mesh) {
    VulkanRenderer& renderer = VulkanRenderer::getInstance();

    const int sectors = 36;
    const int stacks = 18;
    const float radius = 0.5f;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * float(i) / float(stacks);
        for (int j = 0; j <= sectors; ++j) {
            float theta = 2.0f * M_PI * float(j) / float(sectors);

            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Color (gradient based on position)
            vertices.push_back(std::abs(x/radius));
            vertices.push_back(std::abs(y/radius));
            vertices.push_back(std::abs(z/radius));
        }
    }

    // Generate indices
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    renderer.getCore()->createBuffer(vertices.size() * sizeof(float), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.vertexBuffer, mesh.vertexBufferMemory);
    renderer.getCore()->createBuffer(indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.indexBuffer, mesh.indexBufferMemory);
    
    renderer.getCore()->copyDataToBuffer(vertices.data(), mesh.vertexBufferMemory, vertices.size() * sizeof(float));
    renderer.getCore()->copyDataToBuffer(indices.data(), mesh.indexBufferMemory, indices.size() * sizeof(uint32_t));

    mesh.indexCount = indices.size();    
}

void createCube(MeshComponent& mesh) 
{
    VulkanRenderer& renderer = VulkanRenderer::getInstance();
    float vertices[] = {
        // Front face (Red)
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        // Back face (Green)
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        // Top face (Blue)
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        // Bottom face (Yellow)
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        // Right face (Magenta)
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        // Left face (Cyan)
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f
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
        
        // Create entity using ECS
        std::shared_ptr<Entity> entity = scene.registry->createEntity();

        // Add mesh component
        MeshComponent& meshComponent = entity->addComponent<MeshComponent>();
        createSphere(meshComponent);

        // Create and add material component
        MaterialComponent& materialComponent = entity->addComponent<MaterialComponent>();
        
        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        renderer.getCore()->createBuffer(
            sizeof(UBO), 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniformBuffer,
            uniformBufferMemory
        );

        materialComponent.pipeline = renderer.getCore()->getPipeline()->getPipeline();
        materialComponent.pipelineLayout = renderer.getCore()->getPipeline()->getLayout();
        materialComponent.descriptorSet = renderer.getCore()->getDescriptor()->getSet(renderer.getCore()->getCurrentFrame());
        materialComponent.uniformBuffer = uniformBuffer;
        materialComponent.uniformBufferMemory = uniformBufferMemory;

        RenderComponent& renderComponent = entity->addComponent<RenderComponent>();
        renderComponent.material = materialComponent;
        renderComponent.mesh = meshComponent;
        renderComponent.name = "Sphere";

        // Add transform component
        TransformComponent& transformComponent = entity->addComponent<TransformComponent>();
        transformComponent.position = glm::vec3(0.0f, 0.0f, -5.0f);
        transformComponent.rotation = glm::vec3(0.0f);
        transformComponent.scale = glm::vec3(1.0f);

        // Setup camera
        CameraComponent camera;
        camera.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH)/static_cast<float>(HEIGHT), 0.1f, 100.0f);
        camera.view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        
        LightComponent light;
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