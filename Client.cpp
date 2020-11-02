/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#include <stdexcept>
#include <csignal>
#include <iostream>
#include <memory>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Client.h"
#include "NetworkTools.h"

bool EHW::Client::s_terminate;

void EHW::Client::signal_setup()
{
    s_terminate = false;

    // Register SIGINT
    ::signal(SIGINT, [](int sig) -> void {
        std::cout << "Caught signal " << sig << ", terminating" << std::endl;
        s_terminate = true;
    });
}

EHW::Client::Client(const char *server, uint16_t port) : m_server{server},
                                                         m_port{port},
                                                         m_socket{-1},
                                                         m_socket_initialized{false}
{
}

EHW::Client::~Client()
{
    // Close socket if open
    close();
}

void EHW::Client::attach_device(std::unique_ptr<Device> &&device)
{
    m_devices.push_back(std::move(device));
}

void EHW::Client::run()
{
    connect();

    while (!s_terminate) {
        // Update devices with fresh internal state
        update_device_data();
        // Send data from all devices to server
        send_device_data();

        ::sleep(DATA_SEND_PERIOD);
    }
}

void EHW::Client::connect()
{
    ::sockaddr_in remote_addr{};
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = NetworkTools::endian_swap(m_port);

    if ((m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw std::runtime_error("cannot create socket");
    }

    if (::inet_pton(AF_INET, m_server, &remote_addr.sin_addr) != 1) {
        ::close(m_socket);
        throw std::runtime_error("invalid IPv4 address");
    }

    if (::connect(m_socket, (struct sockaddr *) &remote_addr, sizeof remote_addr) < 0) {
        ::close(m_socket);
        throw std::runtime_error("cannot connect to server");
    }

    m_socket_initialized = true;
}

void EHW::Client::close()
{
    if (m_socket_initialized) {
        m_socket_initialized = false;
        ::close(m_socket);
    }
}

void EHW::Client::send_device_data()
{
    if (!m_socket_initialized) {
        throw std::runtime_error("socket not initialized, cannot send data");
    }

    for (auto &d : m_devices) {
        // Serialize current device state
        d->serialize();

        // Send serialized state to server
        auto device_buffer = d->get_serialized_buffer();
        if (::send(m_socket, device_buffer.data(), device_buffer.size(), 0) < 0) {
            throw std::runtime_error("unable to send data over socket");
        }
    }
}

void EHW::Client::update_device_data()
{
    for (auto &d : m_devices) {
        d->update_internal_state();
    }
}