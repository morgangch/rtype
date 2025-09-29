#ifndef CLIENT_RENDER_SYSTEM_HPP
#define CLIENT_RENDER_SYSTEM_HPP

#include <common/core/System.hpp>

namespace rtype::client::systems {
    class RenderSystem : public System {
    public:
        void update(ComponentManager& cm, float deltaTime) override;

    private:
        void renderLayer(int layer);
        void sortByDepth();
    };
}

#endif // CLIENT_RENDER_SYSTEM_HPP
