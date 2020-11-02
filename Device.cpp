/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#include <map>

#include "Device.h"

const std::map<std::string, EHW::Device::Type> EHW::Device::TYPE_STRINGS = {
        {"temp-monitor", Type::TEMP_MONITOR},
        {"uptime-monitor", Type::UPTIME_MONITOR}
};

EHW::Device::Device(const char *identifier, EHW::Device::Type type) : m_identifier{identifier},
                                                                      m_type{type}
{}

void EHW::Device::add_val_to_buffer(uint32_t val)
{
    // Convert to network byte order
    auto val_nbo = NetworkTools::endian_swap(val);

    // Insert into buffer
    auto val_bytes = reinterpret_cast<uint8_t *>(&val_nbo);
    m_serialized_buffer.insert(m_serialized_buffer.end(), val_bytes, val_bytes + sizeof val_nbo);
}

void EHW::Device::add_str_to_buffer(const std::string &str)
{
    // Insert 32-bit string length in network byte order
    add_val_to_buffer(str.length());

    // Insert string
    m_serialized_buffer.insert(m_serialized_buffer.end(), str.begin(), str.end());
}

void EHW::Device::initialize_buffer()
{
    m_serialized_buffer.clear();

    // Insert magic sequence at start of message
    m_serialized_buffer.insert(m_serialized_buffer.begin(), std::begin(PROTO_MAGIC), std::end(PROTO_MAGIC));

    // Insert 32-bit device type in network byte order
    add_val_to_buffer(static_cast<uint32_t>(m_type));

    // Insert device identifier
    add_str_to_buffer(m_identifier);
}