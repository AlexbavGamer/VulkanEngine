#include "Entity.h"
#include "components/TransformComponent.h"

void Entity::updateTransformHierarchy()
{
    // Primeiro, atualize a transformação local desta entidade
    if (hasComponent<TransformComponent>()) {
        auto& transform = getComponent<TransformComponent>();
        
        // Recalcular a matriz local com base nos componentes atuais
        transform.updateLocalMatrix();
        
        // Se esta entidade tiver um pai, calcular a matriz global
        // combinando a matriz local com a matriz global do pai
        if (auto parentPtr = parent.lock()) {
            auto& parentTransform = parentPtr->getComponent<TransformComponent>();
            transform.setWorldMatrix(parentTransform.getWorldMatrix() * transform.getLocalMatrix());
        } else {
            // Se não tiver pai, a matriz global é igual à matriz local
            transform.setWorldMatrix(transform.getLocalMatrix());
        }
    }
    
    // Recursivamente atualizar todos os filhos
    for (auto& child : children) {
        child->updateTransformHierarchy();
    }
}