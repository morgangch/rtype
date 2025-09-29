#include "rtype.h"

int main() {
    rtype::server::Rtype &r = root;

    r.udp_server_fd = setupUDPServer(8080);
    return 0;
}
