//
// Created by Jack_ on 13/10/2025.
//

#include "SerialPortDebugger.h"
#include "../interfaces/IComponentMessanger.h"
#include <cstdio>
#include <cstring>

SerialPortDebugger::SerialPortDebugger() : msg_size(0) {
	dbg_msg[0] = '\0';
}

bool SerialPortDebugger::update(memoryComponentMessanger& m_bus) {
	if (m_bus.read(0xFF02) == 0x81) {
		char c = static_cast<char>(m_bus.read(0xFF01));
		m_bus.write(0xFF02, 0x00);

		// A newline terminates a line (e.g. a test result). Signal the caller to
		// flush + clear so each line is logged once and the buffer never grows.
		if (c == '\n') {
			return true;
		}

		if (msg_size < BUFFER_SIZE - 1) {
			dbg_msg[msg_size++] = c;
			dbg_msg[msg_size] = '\0';
		}
	}
	return false;
}

void SerialPortDebugger::print() {
	if (dbg_msg[0]) {
		LOG_INFO("SERIAL: {}", dbg_msg);

		// Distinct, clearly-marked result line. Emitted only here (on a completed
		// line), so the whole verdict word ("Passed" / "Failed #N") has arrived.
		if (std::strstr(dbg_msg, "Passed") != nullptr) {
			LOG_INFO("==== RESULT: {} ====", dbg_msg);
		} else if (std::strstr(dbg_msg, "Failed") != nullptr) {
			LOG_ERROR("==== RESULT: {} ====", dbg_msg);
		}
	}
	clear();
}

void SerialPortDebugger::clear() {
	dbg_msg[0] = '\0';
	msg_size = 0;
}
