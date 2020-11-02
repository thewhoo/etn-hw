/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#pragma once

#include <vector>
#include <cstdint>
#include <memory>

#include "Device.h"

namespace EHW {

    /**
     * A client emulates a selection of devices and sends their data to the server
     */
    class Client final {

    private:
        const char *const m_server;
        const uint16_t m_port;

        int m_socket;
        bool m_socket_initialized;

        // Set by signal handler
        static bool s_terminate;

        std::vector<std::unique_ptr<Device>> m_devices;

    public:
        // Period between sending device data in seconds
        static constexpr int DATA_SEND_PERIOD = 1;

        static void signal_setup();

        explicit Client(const char *server, uint16_t port);

        ~Client();

        /**
         * Attach a new device to the client
         * @param device Device to attach
         */
        void attach_device(std::unique_ptr<Device> &&device);

        /**
         * Begin emulating devices and sending data to the server
         */
        void run();

    private:
        /**
         * Attempt to connect to server
         * @throws std::runtime_error
         */
        void connect();

        /**
         * Close socket if open
         */
        void close();

        /**
         * Send current values of all attached devices to server
         * @throws std::runtime_error
         */
        void send_device_data();

        /**
         * Force all devices to refresh their internal states
         */
        void update_device_data();
    };

}