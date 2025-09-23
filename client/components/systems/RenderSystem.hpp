#pragma once
#include <common/core/System.hpp>

namespace Client {
    class RenderSystem : public System {
    public:
        void update(ComponentManager& cm, float deltaTime) override;
        
    private:
        void renderLayer(RenderLayer layer);
        void sortByDepth();
    };
}
