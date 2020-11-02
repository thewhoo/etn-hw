/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#pragma once

#include <cstdint>
#include <vector>
#include <ctime>
#include <map>

#include <unistd.h>
#include <poll.h>

#include "NetworkTools.h"
#include "Device.h"

namespace EHW {

    class DataPack final {

    private:
        std::string m_device_id;
        Device::Type m_device_type;
        std::string m_data;
        std::time_t m_timestamp;

    public:
        explicit DataPack(std::string &&device_id, Device::Type device_type, std::string &&m_data);

        [[nodiscard]]
        inline auto get_id() const
        { return m_device_id; }

        [[nodiscard]]
        inline auto get_type() const
        { return m_device_type; }

        [[nodiscard]]
        inline auto get_data() const
        { return m_data; }

        [[nodiscard]]
        inline auto get_timestamp() const
        { return m_timestamp; }

    };

    class Server final {

    private:
        static constexpr int SOCKET_BACKLOG = 32;
        const uint16_t m_port;

        int m_socket;
        bool m_socket_initialized;
        std::vector<::pollfd> m_poll_set;

        // Set by signal handler
        static bool s_terminate;

        // Message counter for individual devices
        std::map<std::string, int> m_device_counter;

    public:
        static void signal_setup();

        explicit Server(uint16_t port);

        ~Server();

        /**
         * Begin receiving data from devices at specified port
         * @throws std::runtime_error
         */
        void run();

    private:
        /**
         * Setup listening socket
         * @throws std::runtime_error
         */
        void setup_socket();

        /**
         * Handle incoming data from open connections
         * @throws std::runtime_error
         */
        void handle_incoming();

        /**
         * Read 8 to 64-bit integer values from device in network byte order and convert to host byte order
         * @tparam T value type
         * @param fd file descriptor
         * @param value destination
         * @return true if successfully read, otherwise false
         */
        template<typename T>
        static bool read_int_value(int fd, T &value)
        {
            if (::read(fd, &value, sizeof value) != sizeof value) {
                return false;
            }

            value = NetworkTools::endian_swap(value);

            return true;
        }

        static bool read_str_value(int fd, std::string &str, size_t len)
        {
            str.resize(len);
            if (::read(fd, str.data(), str.size()) != len) {
                return false;
            }

            return true;
        }

        /**
         * Process data pack received from device (increment counters, print information)
         * @param pack Data pack
         */
        void handle_data_pack(const DataPack &pack);

        /**
         * Read and parse data from client socket
         * @param client_sock Socket of client to read data from
         * @return true if read was successfull, false otherwise
         */
        bool read_client_data(int client_sock);

        /**
         * Close listening socket if open
         */
        void close();

    };

}
