//
// Created by Jack_ on 17/09/2025.
//

#include "dbg.h"
#include "../components/bus.h"

static char dbg_msg[1024] = {0};
static int msg_size = 0;

void dbg::dbgUpdate(const std::weak_ptr<bus>& m_bus)
{
	return;
	auto bus_ptr = m_bus.lock();
	if (!bus_ptr) return;

	static uint8_t last_control = 0;
	static uint8_t last_data = 0;

	uint8_t current_control = bus_ptr->read(0xFF02);
	uint8_t current_data = bus_ptr->read(0xFF01);

	// Log ANY changes to serial registers
	if (current_control != last_control) {
		LOG_INFO("Serial Control changed: 0x{:02X} -> 0x{:02X}", last_control, current_control);
		last_control = current_control;
	}

	if (current_data != last_data) {
		LOG_INFO("Serial Data changed: 0x{:02X} -> 0x{:02X} ('{}')",
				 last_data, current_data,
				 (current_data >= 32 && current_data <= 126) ? static_cast<char>(current_data) : '?');
		last_data = current_data;
	}

	// Original logic
	if (current_control & 0x80) {
		char c = current_data;
		LOG_WARN("Serial transfer detected: '{}' (0x{:02X})",
				 (c >= 32 && c <= 126) ? c : '?', static_cast<uint8_t>(c));
		dbg_msg[msg_size++] = c;
		bus_ptr->write(0xFF02, 0x00);  // Clear transfer bit
	}
	/*uint8_t serialControl = m_bus.lock()->read(0xFF02);

	// Log all non-zero values to see what's actually being written
	if (serialControl != 0) {
		LOG_WARN("Serial control register (0xFF02): 0x{:02X}", serialControl);
	}
	if (m_bus.lock()->read(0xFF02) & 0x80)
	{
		char c = m_bus.lock()->read(0xFF01);
		dbg_msg[msg_size++] = c;

		m_bus.lock()->write(0xFF02, 0);
	}*/
 }

void dbg::dbgPrint()
{
 	if (dbg_msg[0])
	{
		LOG_WARN("DBG: {}",dbg_msg[0]);
	}
}