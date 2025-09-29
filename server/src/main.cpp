#include "network.h"
#include "rtype.h"

void rtype::server::Rtype::loop() {
    (void)udp_server_fd;
}


int main() {
    rtype::server::Rtype &r = root;

    r.udp_server_fd = rtype::server::network::setupUDPServer(8080);

    while (1) {
        r.loop();
        rtype::server::network::loop_recv(r.udp_server_fd);
    }
    return 0;
}
