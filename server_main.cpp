/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#include <iostream>

#include "Server.h"

const char *usage = "./server PORT\n";

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << usage;
        return 1;
    }

    auto port = std::stoi(argv[1]);

    auto server = EHW::Server(port);
    EHW::Server::signal_setup();

    server.run();

    return 0;
}
