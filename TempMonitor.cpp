/**
 * Author: Matej Postolka <matej@postolka.net>
 * License: BSD 3-clause
 */

#include "TempMonitor.h"

EHW::TempMonitor::TempMonitor(const char *identifier) : Device(identifier, Type::TEMP_MONITOR),
                                                        m_distribution{TEMP_LB, TEMP_UB}
{
    update_internal_state();
}

uint64_t EHW::TempMonitor::get_poll_delay() const
{
    return POLL_DELAY;
}

void EHW::TempMonitor::serialize()
{
    // Create initial serialized buffer
    initialize_buffer();

    // Append current temperature
    auto temp_str = std::to_string(m_current_temp);
    add_str_to_buffer(temp_str);
}

void EHW::TempMonitor::update_internal_state()
{
    m_current_temp = m_distribution(m_re);
}