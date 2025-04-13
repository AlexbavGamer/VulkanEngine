#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <boost/hana.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>

struct CameraComponent : public Component
{
    CameraComponent(std::shared_ptr<Entity> owner = nullptr) : Component(owner) 
    {
        this->projection = glm::mat4(1.0f);
        this->view = glm::mat4(1.0f);
        this->width = 800.0f;
        this->height = 600.0f;

        this->yaw = -90.0f;     // Começa olhando para -Z
        this->pitch = 0.0f;     // Olhando reto (adicionei .0f para consistência)
        this->sensitivity = 0.1f;
        this->fov = 45.0f;      // FOV padrão
        this->near = 0.1f;      // Adicionar near plane
        this->far = 1000.0f;    // Far plane já existente

        // Posição inicial mais adequada
        this->position = glm::vec3(0.0f, 5.0f, 10.0f);  // Mais afastado em Z
        
        // Calcular front inicial baseado no yaw e pitch
        this->front = glm::normalize(glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        ));
        
        this->up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Inicializar matrizes
        updateProjectionMatrix();
        updateViewMatrix();
    }

    void updateProjectionMatrix()
    {
        float aspectRatio = width / height;
        projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, this->far);
        projection[1][1] *= -1;
    }

    void updateViewMatrix()
    {
        view = glm::lookAt(position, position + front, up);
    }

    void setAspectRatio(float width, float height)
    {
        this->width = width;
        this->height = height;
        updateProjectionMatrix();
    }

    void handleMouseMovement(double deltaX, double deltaY)
    {
        deltaX *= sensitivity;
        deltaY *= sensitivity;

        yaw += deltaX;
        pitch = glm::clamp(pitch + static_cast<float>(deltaY), -89.0f, 89.0f);

        // Recalcula os vetores da câmera
        glm::vec3 direction;
        float pitchRad = glm::radians(pitch);
        float yawRad = glm::radians(yaw);

        direction.x = cos(yawRad) * cos(pitchRad);
        direction.y = -sin(pitchRad); // Invertido para corresponder ao comportamento da Unity/UE
        direction.z = sin(yawRad) * cos(pitchRad);

        front = glm::normalize(direction);
        // Recalcula o vetor right baseado no novo front
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));

        updateViewMatrix();
    }

    void handleKeyboardInput(GLFWwindow* window)
    {
        static bool escPressed = false;
        static const float CAMERA_SPEED = 5.0f;
        static const float SPRINT_MULTIPLIER = 2.5f;

        // Toggle cursor lock com ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escPressed)
        {
            escPressed = true;
            bool& cursorEnabled = isCursorEnabled();
            cursorEnabled = !cursorEnabled;
            glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            if (cursorEnabled)
                isFirstMouse() = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        {
            escPressed = false;
        }

        // Se o cursor estiver habilitado ou ImGui estiver capturando input, não processe os inputs de movimento
        ImGuiIO& io = ImGui::GetIO();
        if (isCursorEnabled())
            return;

        float deltaTime = 0.016f;
        float currentSpeed = CAMERA_SPEED * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            currentSpeed *= SPRINT_MULTIPLIER;
            fov = glm::min(fov + deltaTime * 30.0f, 90.0f);
        }
        else
        {
            fov = glm::max(fov - deltaTime * 30.0f, 45.0f);
        }

        glm::vec3 movement(0.0f);
        glm::vec3 cameraForward = glm::normalize(front);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraForward, worldUp));

        // Movimento frontal/traseiro
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            movement += cameraForward;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            movement -= cameraForward;

        // Movimento lateral
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            movement += cameraRight;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            movement -= cameraRight;

        // Movimento vertical
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            movement.y += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            movement.y -= 1.0f;

        // Normalizar e aplicar movimento
        if (glm::length(movement) > 0.0f)
        {
            movement = glm::normalize(movement);
            position += movement * currentSpeed;
        }

        updateViewMatrix();
    }

    static bool& isCursorEnabled() {
        static bool cursorEnabled = true;
        return cursorEnabled;
    }

    static bool& isFirstMouse() {
        static bool firstMouse = true;
        return firstMouse;
    }

    static float& lastX() {
        static float x = 400.0f;
        return x;
    }

    static float& lastY() {
        static float y = 300.0f;
        return y;
    }

    void processMouseInput(GLFWwindow* window)
    {
        // Se o cursor estiver habilitado, não processe o movimento do mouse
        if (isCursorEnabled())
            return;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (isFirstMouse())
        {
            lastX() = xpos;
            lastY() = ypos;
            isFirstMouse() = false;
        }

        float deltaX = xpos - lastX();
        float deltaY = ypos - lastY();

        lastX() = xpos;
        lastY() = ypos;

        handleMouseMovement(deltaX, deltaY);
    }

    glm::mat4 getViewMatrix() const {
        return view;
    }

    glm::mat4 getProjectionMatrix() const
    {
        return projection;
    }

    glm::mat4 getViewProjection() const
    {
        return projection * view;
    }

    BOOST_HANA_DEFINE_STRUCT(CameraComponent,
        (glm::mat4, projection),
        (glm::mat4, view),
        (float, width),
        (float, height),
        (float, yaw),
        (float, pitch),
        (float, sensitivity),
        (float, fov),
        (float, far),
        (float, near),
        (glm::vec3, position),
        (glm::vec3, front),
        (glm::vec3, up),
        (glm::vec3, right)  // Adicionando right vector
    );
};