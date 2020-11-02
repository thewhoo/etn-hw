/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#pragma once

#include <random>

#include "Device.h"

namespace EHW {

    /**
     * Emulator of device tracking uptime in milliseconds
     */
    class UptimeMonitor final : public Device {

    private:
        static constexpr int INITIAL_UPTIME = 0;
        // Increment of uptime when update method is called
        static constexpr int UPTIME_STEP = 1000;
        // Statically set polling delay
        static constexpr int POLL_DELAY = 1000;

        int m_current_uptime;

    public:
        explicit UptimeMonitor(const char *identifier);

        [[nodiscard]]
        uint64_t get_poll_delay() const override;

        void serialize() override;

        void update_internal_state() override;

    };
}