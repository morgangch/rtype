#include "client/include/components/render.hpp"

namespace rtype::client::components {
    Renderable::Renderable(const std::string& texturePath, float width, float height, RenderLayer layer)
        : texturePath(texturePath), width(width), height(height), layer(layer), visible(true), alpha(1.0f) {}
}
