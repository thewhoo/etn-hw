/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#pragma once

#include <random>

#include "Device.h"

namespace EHW {

    /**
     * Emulator of device monitoring temperature in floating point values
     */
    class TempMonitor final : public Device {

    private:
        // Lower bound of randomly generated temperatures
        static constexpr double TEMP_LB = 0.0;
        // Upper bound of randomly generated temperatures
        static constexpr double TEMP_UB = 100.0;
        // Statically set polling delay
        static constexpr int POLL_DELAY = 1000;

        std::uniform_real_distribution<double> m_distribution;
        std::default_random_engine m_re;

        double m_current_temp;

    public:
        explicit TempMonitor(const char *identifier);

        [[nodiscard]]
        uint64_t get_poll_delay() const override;

        void serialize() override;

        void update_internal_state() override;

    };
}