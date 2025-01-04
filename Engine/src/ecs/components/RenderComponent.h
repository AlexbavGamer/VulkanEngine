#pragma once

#include "../Component.h"
#include <string>
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "imgui.h"

struct RenderComponent : public Component
{
    RenderComponent(std::shared_ptr<Entity> owner)
        : Component(owner) {}

    RenderComponent() {}

    std::string name;

    void renderComponent() override
    {
        char nameBuffer[256];
        strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer) - 1);
        nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Garantir terminação nula

        if (ImGui::InputText("Component Name", nameBuffer, sizeof(nameBuffer)))
        {
            // Se o texto for alterado, atualize o nome do componente
            name = nameBuffer;
        }
    }
};