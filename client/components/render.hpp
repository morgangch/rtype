#pragma once
#include <ECS/ECS.hpp>
#include <string>

namespace Client {
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