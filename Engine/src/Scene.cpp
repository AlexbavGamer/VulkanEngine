#include "Scene.h"
#include "Components.h"
#include "core/VulkanCore.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanPipeline.h"
#include "core/VulkanSwapChain.h"
#include <format>
#include <iostream>
#include <magic_enum.hpp>

Scene::Scene(VulkanCore *core) : core(core)
{
    registry = std::make_unique<Registry>();
    renderSystem = std::make_unique<RenderSystem>();
}

Scene::~Scene() {}

void Scene::addEntity(std::shared_ptr<Entity> entity)
{
    registry->addEntity(entity);
}

std::shared_ptr<Entity> Scene::createLightEntity(LightComponent::LightType lightType)
{
    std::shared_ptr<Entity> lightEntity = createEntity();
    LightComponent &lightComponent = lightEntity->addComponent<LightComponent>();
    lightComponent.setType(lightType);
    lightEntity->setName(std::string(magic_enum::enum_name(lightType).data()).append(" Light"));
    return lightEntity;
}

void Scene::updateCamera()
{
    auto &camera = cameraEntity->getComponent<CameraComponent>();
    camera.updateViewMatrix();
}

void Scene::handleKeyboardInput(GLFWwindow *window)
{
    if (cameraEntity) {
        auto& camera = cameraEntity->getComponent<CameraComponent>();
        camera.handleKeyboardInput(window);
    }
}

void Scene::handleMouseInput(GLFWwindow *window, double xpos, double ypos)
{
    if (cameraEntity) {
        cameraEntity->getComponent<CameraComponent>().processMouseInput(window);
    }
}
