#include "network.h"
#include "packets.h"
#include "rtype.h"
#include "controllers/RoomController.h"
#include "systems/PacketHandlingSystem.h"
#include <chrono>
#include "systems/AdminDetectorSystem.h"

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
    root.packetHandler.registerCallback(Packets::GAME_START_REQUEST, rtype::server::controllers::room_controller::handleGameStartRequest);
    root.world.RegisterSystem<PacketHandlingSystem>();
    root.world.RegisterSystem<AdminDetectorSystem>();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        r.loop(deltaTime);
    }
    return 0;
}
