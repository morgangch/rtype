#ifndef COMMON_PLAYER_HPP
#define COMMON_PLAYER_HPP

#include <ECS/ECS.h>
#include <utility>
#include <string>
#include "VesselClass.h"

namespace rtype::common::components {
    
    class Player : public ECS::Component<Player> {
    public:
        /**
         * @brief The name of the player.
         */
        std::string name;

        /**
         * @brief The server ID of the player.
         * Used to identify the player on the client side.
         */
        unsigned int serverId = 0;
        
        /**
         * @brief The vessel class/type chosen by the player.
         * Default: CrimsonStriker (balanced class)
         */
        VesselType vesselType = VesselType::CrimsonStriker;

    public:
        explicit Player(std::string name, unsigned int serverId = 0, VesselType vessel = VesselType::CrimsonStriker) 
            : name(std::move(name)), serverId(serverId), vesselType(vessel) {}
    };
}

#endif // COMMON_PLAYER_HPP
