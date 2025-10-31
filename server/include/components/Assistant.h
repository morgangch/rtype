/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Server-side Assistant component - tag + cooldown for AI helper
*/

#ifndef SERVER_ASSISTANT_HPP
#define SERVER_ASSISTANT_HPP

#include <ECS/ECS.h>

namespace rtype::server::components {
    /**
     * @file Assistant.h
     * @brief Small server-only component used to mark an entity as an AI assistant
     *
     * The Assistant component is a lightweight, server-side tag used by
     * the `AssistantSystem` to identify AI helper entities. It currently
     * contains a single float used as a shot cooldown timer.
     *
     * Responsibilities / contract:
     * - When attached to an entity, that entity will be considered an AI
     *   assistant and the `AssistantSystem` will control its movement and
     *   firing behaviour.
     * - The `shootCooldown` field holds seconds remaining until the next
     *   allowed shot; the system should decrement it each update and reset
     *   it when a projectile is fired.
     *
     * Notes:
     * - This component is server-only and must not be serialized to the
     *   client. Clients receive entity spawn / state packets from the server
     *   and render assistant vessels like other remote players.
     * - The component is intentionally minimal to remain cheap to store
     *   across many entities.
     */
    class Assistant : public ECS::Component<Assistant> {
    public:
        /**
         * @brief Seconds remaining until next allowed shot.
         *
         * Initialized to 0.0f (can fire immediately). The server system
         * decreases this value by delta time and sets it to a positive value
         * after firing (e.g. 0.6f) to implement a firing rate.
         */
        float shootCooldown = 0.0f; // seconds until next shot allowed

        /**
         * @brief Construct a new Assistant component
         * @param cd Optional initial cooldown in seconds (default 0.0f)
         */
        Assistant(float cd = 0.0f) : shootCooldown(cd) {}
    };
}

#endif // SERVER_ASSISTANT_HPP
