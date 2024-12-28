#include "../Component.h"
#include <string>
#include "MeshComponent.h"
#include "MaterialComponent.h"

struct RenderComponent : public Component
{
    RenderComponent(std::string name) : name(name) {}
    RenderComponent() {}

    std::string name;
    MeshComponent mesh;
    MaterialComponent material;

    void renderComponent() override
    {
        
    }
};