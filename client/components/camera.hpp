#ifndef CLIENT_CAMERA_HPP
#define CLIENT_CAMERA_HPP

#include <ECS/ECS.hpp>
#include <common/components/Position.hpp>

namespace Client {
    class Camera : public ECS::Component<Camera> {
    public:
        float zoom;
        ECS::EntityID followTarget;

        Camera(float zoom = 1.0f);
    };
}

#endif // CLIENT_CAMERA_HPP
