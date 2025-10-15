#include "network.h"
#include "packets.h"
#include "rtype.h"
#include "controllers/RoomController.h"
#include "systems/PacketHandlingSystem.h"
#include <chrono>

void rtype::server::Rtype::loop(float deltaTime) {
    network::loop_recv(udp_server_fd);
    network::loop_send(udp_server_fd);
    packetHandler.processPackets(packetManager.fetchReceivedPackets());
    world.UpdateSystems(deltaTime);
}


int main() {
    rtype::server::Rtype &r = root;
    r.udp_server_fd = rtype::server::network::setupUDPServer(4242);

    root.packetHandler.registerCallback(Packets::JOIN_ROOM, rtype::server::controllers::room_controller::handleJoinRoomPacket);
    root.world.RegisterSystem<PacketHandlingSystem>();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        r.loop(deltaTime);
    }
    return 0;
}
