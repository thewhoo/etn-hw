/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>

#include "NetworkTools.h"

namespace EHW {

    /**
     * Abstract class representing a device reporting to the server
     */
    class Device {

    public:
        // Magic sequence indicating start of message sent by device
        static constexpr uint8_t PROTO_MAGIC[] = {0xde, 0xad, 0xbe, 0xef};

        // Supported types of devices
        enum class Type {
            TEMP_MONITOR,
            UPTIME_MONITOR
        };

        static const std::map<std::string, Type> TYPE_STRINGS;

    protected:
        // Unique string identifier of device
        const std::string m_identifier;
        const Type m_type;
        std::vector<uint8_t> m_serialized_buffer;

        /**
         * Constructor of abstract Device class called from subclasses
         * @param identifier Unique string identifying device
         * @param type Type of device
         */
        explicit Device(const char *identifier, Type type);


    public:
        virtual ~Device() = default;

        /**
         * Get device identifier
         * @return Device identifier
         */
        [[nodiscard]]
        const inline std::string &get_id() const
        { return m_identifier; }

        /**
         * Get device type
         * @return Device type
         */
        [[nodiscard]]
        auto inline get_type() const
        { return m_type; }

        /**
         * Access serialized buffer
         * @return Reference to serialized buffer
         */
        [[nodiscard]]
        const inline std::vector<uint8_t> &get_serialized_buffer() const
        { return m_serialized_buffer; }

        /**
         * Abstract method for finding out the delay with which device produces new measurements
         * @return Interval between producing new measurements in milliseconds
         */
        [[nodiscard]]
        virtual uint64_t get_poll_delay() const = 0;

        /**
         * Abstract method forcing a device to update its internal state to latest reading/value
         */
        virtual void update_internal_state() = 0;

        /**
         * Abstract method for serializing current measurement into transmission buffer
         */
        virtual void serialize() = 0;

    protected:
        /**
         * Insert 32-bit value into buffer in network byte order
         * @param val Value to insert
         */
        virtual void add_val_to_buffer(uint32_t val);

        /**
         * Insert a string into the buffer prepended with 32-bit length
         * @param str String to be inserted
         */
        virtual void add_str_to_buffer(const std::string &str);

        /**
         * Insert initial data into serialized buffer
         */
        virtual void initialize_buffer();

    };

}