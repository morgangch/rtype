#include "network.h"
#include "packets.h"
#include "rtype.h"
#include "systems.h"
#include "controllers/RoomController.h"

void rtype::server::Rtype::loop() {
    network::loop_recv(udp_server_fd);
    network::loop_send(udp_server_fd);
    packetHandler.processPackets(packetManager.fetchReceivedPackets());

    auto entities = world.GetAllEntities();
    for (const auto &e: entities) {
        systems::apply_systems(e);
    }
}


int main() {
    rtype::server::Rtype &r = root;
    r.udp_server_fd = rtype::server::network::setupUDPServer(4242);

    root.packetHandler.registerCallback(Packets::JOIN_ROOM, rtype::server::controllers::room_controller::handleJoinRoomPacket);

    while (true) {
        r.loop();
    }
    return 0;
}
