#include "VulkanRenderer.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>  // Necessário para MessageBox
#include <memory>

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;


float lastX = 0.0f;
float lastY = 0.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
float mouseSensitivity = 0.1f;
bool cursorEnabled = true;

float moveSpeed = 0.1f;

void showError(const std::string& message) {
    MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR | MB_OK);
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
    // Vértices para um cubo simples (6 faces)

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
    
    // Criar buffers de vértices e índices
    renderer.getCore()->createBuffer(sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.vertexBuffer, mesh.vertexBufferMemory);
    renderer.getCore()->createBuffer(sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.indexBuffer, mesh.indexBufferMemory);
    
    renderer.getCore()->copyDataToBuffer(vertices, mesh.vertexBufferMemory, sizeof(vertices));
    renderer.getCore()->copyDataToBuffer(indices, mesh.indexBufferMemory, sizeof(indices));

    mesh.indexCount = sizeof(indices) / sizeof(indices[0]);
}

void processInput(GLFWwindow* window, Scene& scene) {
    static bool escPressed = false;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!escPressed) {
            cursorEnabled = !cursorEnabled;
            glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            if (cursorEnabled) {
                firstMouse = true;
            }
        }
        escPressed = true;
    } else {
        escPressed = false;
    }

    if (!cursorEnabled) {
        // Calculate new direction vectors based on yaw and pitch
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        glm::vec3 forward = glm::normalize(direction);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        
        // Extract current position from view matrix
        glm::mat4 view = scene.camera.view;
        glm::vec4 pos = glm::inverse(view)[3];
        glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);

        // Movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += forward * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= forward * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= right * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * moveSpeed;

        glm::vec3 target = position + forward;
        camera::updateCamera(scene.camera, position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}


void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (cursorEnabled) return;
    
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch = glm::clamp(pitch + yoffset, -89.0f, 89.0f);
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
        glfwSetWindowUserPointer(window, renderer.getScene());
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // Criar o cubo e adicioná-lo à cena
        MeshComponent cubeMesh;
        createSphere(cubeMesh);

        // Definir o cubo na cena (supondo que você tenha uma cena ou uma função de renderização)
        Scene& scene = *renderer.getScene();
        // After creating cube mesh
        Entity cubeEntity;
        cubeEntity.mesh = cubeMesh;

        // Create uniform buffer
        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        renderer.getCore()->createBuffer(
            sizeof(UBO), 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniformBuffer,
            uniformBufferMemory
        );

        // Initialize material with valid descriptor set
        MaterialComponent material;
        material.pipeline = renderer.getCore()->getPipeline()->getPipeline();
        material.pipelineLayout = renderer.getCore()->getPipeline()->getLayout();
        material.descriptorSet = renderer.getCore()->getDescriptor()->getSet(renderer.getCurrentFrame()); // Make sure this returns a valid descriptor set
        material.uniformBuffer = uniformBuffer;
        material.uniformBufferMemory = uniformBufferMemory;

        TransformComponent transform;
        transform.position = glm::vec3(0.0f, 0.0f, -5.0f); // Move back from camera
        transform.rotation = glm::vec3(0.0f);
        transform.scale = glm::vec3(1.0f);

        cubeEntity.transform = transform;
        cubeEntity.material = material;
        scene.addEntity(cubeEntity);

       CameraComponent camera;
        camera.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH)/static_cast<float>(HEIGHT), 0.1f, 100.0f);
        camera.view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),  // Camera position
            glm::vec3(0.0f, 0.0f, 0.0f),  // Look at point
            glm::vec3(0.0f, 1.0f, 0.0f)   // Up vector
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
        processInput(window, *renderer.getScene());

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


        renderer.renderFrame();
    }

    renderer.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}