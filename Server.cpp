/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#include <iostream>
#include <csignal>
#include <stdexcept>
#include <ctime>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#include "Server.h"
#include "Device.h"
#include "NetworkTools.h"

bool EHW::Server::s_terminate;

void EHW::Server::signal_setup()
{
    s_terminate = false;

    // Register SIGINT
    ::signal(SIGINT, [](int sig) -> void {
        std::cout << "Caught signal " << sig << ", terminating" << std::endl;
        s_terminate = true;
    });
}

EHW::Server::Server(uint16_t port) : m_port{port},
                                     m_socket{-1},
                                     m_socket_initialized{false}
{

}

EHW::Server::~Server()
{
    close();
}

void EHW::Server::run()
{
    setup_socket();

    // Accept incoming connections and handle incoming data
    while (!s_terminate) {
        handle_incoming();
    }

    // Print individual device statistics
    std::cout << std::endl;
    for (const auto &item : m_device_counter) {
        std::cout << "Device: " << item.first << "\ttotal messages received: " << item.second << std::endl;
    }
}

void EHW::Server::setup_socket()
{
    ::sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = NetworkTools::endian_swap(m_port);

    if ((m_socket = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
        throw std::runtime_error("cannot create socket");
    }

    int opt = 1;
    if (::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof opt) != 0) {
        ::close(m_socket);
        throw std::runtime_error("setsockopt() failed");
    }

    if (::bind(m_socket, (struct sockaddr *) &server_addr, sizeof server_addr) < 0) {
        ::close(m_socket);
        throw std::runtime_error("bind() failed");
    }

    if (::listen(m_socket, SOCKET_BACKLOG) < 0) {
        ::close(m_socket);
        throw std::runtime_error("listen() failed");
    }

    m_socket_initialized = true;
}

void EHW::Server::handle_incoming()
{
    if (!m_socket_initialized) {
        throw std::runtime_error("cannot accept connection with uninitialized socket");
    }

    // Check for new incoming connections
    auto server_pollfd = ::pollfd{};
    server_pollfd.fd = m_socket;
    server_pollfd.events = POLLIN;
    if (::poll(&server_pollfd, 1, 0) < 0) {
        return;
    }

    // New client connected
    if (server_pollfd.revents & POLLIN) {
        // Accept incoming connection
        int new_socket;
        ::sockaddr_in addr{};
        auto addr_len = sizeof addr;

        if ((new_socket = ::accept(m_socket, (struct sockaddr *) &addr, (socklen_t *) &addr_len)) < 0) {
            throw std::runtime_error("unable to accept incoming connection");
        }

        // Store handle of incoming connection
        auto client_pollfd = ::pollfd{};
        client_pollfd.fd = new_socket;
        client_pollfd.events = POLLIN;
        m_poll_set.push_back(client_pollfd);
    }

    // Poll all open socket descriptors
    if (::poll(m_poll_set.data(), m_poll_set.size(), 0) < 0) {
        return;
    }

    for (auto it = m_poll_set.begin(); it != m_poll_set.end(); it++) {
        // Do nothing if no data was received
        if (!(it->revents & POLLIN)) {
            continue;
        }

        // Check if client terminated connection
        if (!read_client_data(it->fd)) {
            // Close connection
            ::close(it->fd);

            // Remove client from poll set
            it = m_poll_set.erase(it);
            it--;
        }
    }

}

bool EHW::Server::read_client_data(int client_sock)
{
    // Attempt to read magic sequence
    uint32_t magic;
    if (!read_int_value(client_sock, magic)) {
        return false;
    }

    if (magic != NetworkTools::endian_swap(*reinterpret_cast<const uint32_t *>(Device::PROTO_MAGIC))) {
        return false;
    }

    // Attempt to read device type
    uint32_t device_type;
    if (!read_int_value(client_sock, device_type)) {
        return false;
    }

    // Attempt to read device ID
    uint32_t id_length;
    if (!read_int_value(client_sock, id_length)) {
        return false;
    }

    std::string device_id;
    if (!read_str_value(client_sock, device_id, id_length)) {
        return false;
    }

    // Attempt to read device data
    uint32_t data_len;
    if (!read_int_value(client_sock, data_len)) {
        return false;
    }

    std::string data;
    if (!read_str_value(client_sock, data, data_len)) {
        return false;
    }

    // Track message counts
    auto data_pack = DataPack(std::move(device_id), static_cast<Device::Type>(device_type), std::move(data));
    handle_data_pack(data_pack);

    return true;
}

void EHW::Server::handle_data_pack(const DataPack &pack)
{
    // Increase message counters
    auto device_id = pack.get_id();
    auto it = m_device_counter.find(device_id);
    if (it != m_device_counter.end()) {
        it->second++;
    }
        // No message from device received yet
    else {
        m_device_counter[device_id] = 1;
    }

    // Print information to stdout
    std::cout << "Received message from device: " << device_id << " type: " << static_cast<int>(pack.get_type())
              << " data: " << pack.get_data() << " ts: " << pack.get_timestamp() << std::endl;
}

void EHW::Server::close()
{
    if (!m_socket_initialized)
        return;

    m_socket_initialized = false;

    // Close listening socket
    ::close(m_socket);

    // Close all client connections
    for (auto &conn : m_poll_set) {
        ::close(conn.fd);
    }
    m_poll_set.clear();
}

EHW::DataPack::DataPack(std::string &&device_id, Device::Type device_type, std::string &&data) : m_device_id{device_id},
                                                                                                 m_device_type{
                                                                                                         device_type},
                                                                                                 m_data{data},
                                                                                                 m_timestamp{std::time(
                                                                                                         nullptr)}
{
}