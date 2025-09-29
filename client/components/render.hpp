#ifndef CLIENT_RENDERABLE_HPP
#define CLIENT_RENDERABLE_HPP

#include <ECS/ECS.hpp>
#include <string>

namespace rtype::client::components {
    enum class RenderLayer {
        Background = 0,
        Entities,
        Effects,
        UI,
        Debug
    };

    class Renderable : public ECS::Component<Renderable> {
    public:
        std::string texturePath;
        float width, height;
        RenderLayer layer;
        bool visible;
        float alpha;

        Renderable(const std::string& texturePath = "", float width = 32.0f, float height = 32.0f);
    };
}

#endif // CLIENT_RENDERABLE_HPP
