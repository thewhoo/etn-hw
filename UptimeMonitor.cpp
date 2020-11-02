/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#include "UptimeMonitor.h"

EHW::UptimeMonitor::UptimeMonitor(const char *identifier) : Device(identifier, Type::UPTIME_MONITOR),
                                                            m_current_uptime(INITIAL_UPTIME)
{

}

uint64_t EHW::UptimeMonitor::get_poll_delay() const
{
    return POLL_DELAY;
}

void EHW::UptimeMonitor::serialize()
{
    // Create initial serialized buffer
    initialize_buffer();

    // Append current uptime
    auto uptime_str = std::to_string(m_current_uptime);
    add_str_to_buffer(uptime_str);
}

void EHW::UptimeMonitor::update_internal_state()
{
    m_current_uptime += UPTIME_STEP;
}