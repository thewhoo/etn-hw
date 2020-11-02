/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#pragma once

#include <cstdint>

#include <byteswap.h>

namespace EHW {

    class NetworkTools {

    public:

        /**
         * @brief Get inverted endianness of 16-bit value
         * @param data 16-bit value
         * @return value with inverted endianness
         */
        static uint16_t endian_swap(const uint16_t &data)
        { return bswap_16(data); }

        /**
          * @brief Get inverted endianness of 32-bit value
          * @param data 16-bit value
          * @return value with inverted endianness
          */
        static uint32_t endian_swap(const uint32_t &data)
        { return bswap_32(data); }

        /**
         * @brief Get inverted endianness of 64-bit value
         * @param data 16-bit value
         * @return value with inverted endianness
         */
        static uint64_t endian_swap(const uint64_t &data)
        { return bswap_64(data); }

    };

}
