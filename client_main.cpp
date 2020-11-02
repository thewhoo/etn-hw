/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#include <iostream>
#include <memory>
#include <cstring>
#include <stdexcept>

#include "Device.h"
#include "Client.h"
#include "TempMonitor.h"

const char *usage = "./client SERVER_IP SERVER_PORT [DEVICE-TYPE DEVICE-ID] ... [DEVICE-TYPE DEVICE-ID]\n";

void print_help(std::ostream &s)
{
    s << usage;
    s << "Supported device types:" << std::endl;
    for (const auto &dt : EHW::Device::TYPE_STRINGS) {
        s << "\t" << dt.first << std::endl;
    }
}

int main(int argc, char **argv)
{
    if ((argc < 3) || (argc % 2 == 0)) {
        print_help(std::cerr);
        return 1;
    }
    argv++;

    // Create base client
    auto server_ip = *argv++;
    auto server_port = std::stoi(*argv++);

    auto client = EHW::Client(server_ip, server_port);
    EHW::Client::signal_setup();

    // Attach requested devices
    for (auto i = 3; i < argc; i += 2) {
        auto device_type = *argv++;
        auto device_id = *argv++;

        std::unique_ptr<EHW::Device> device;
        auto it = EHW::Device::TYPE_STRINGS.find(device_type);
        if (it == EHW::Device::TYPE_STRINGS.end()) {
            print_help(std::cerr);
            return 1;
        }

        if (it->second == EHW::Device::Type::TEMP_MONITOR) {
            device = std::make_unique<EHW::TempMonitor>(EHW::TempMonitor(device_id));
        }
        else {
            throw std::runtime_error("Unimplemented device type");
        }

        client.attach_device(std::move(device));
    }

    // Launch client
    client.run();

    return 0;
}
