//
// Created by Jack_ on 13/10/2025.
//

#ifndef GAMEBOYEMULATOR_SERIALPORTDEBUGGER_H
#define GAMEBOYEMULATOR_SERIALPORTDEBUGGER_H


#include "../components/base/componentMessanger.h"

class bus;

// Captures the Game Boy serial port (SB/SC at 0xFF01/0xFF02) and logs each
// completed line, used to read Blargg test-ROM output.
class SerialPortDebugger {
public:
	SerialPortDebugger();

	bool update(memorycomponentMessanger& m_bus);
	void print();
	void clear();

	const char* getMessage() const { return dbg_msg; }
	int getMessageSize() const { return msg_size; }

private:
	static constexpr int BUFFER_SIZE = 1024;
	char dbg_msg[BUFFER_SIZE];
	int msg_size;
};


#endif //GAMEBOYEMULATOR_SERIALPORTDEBUGGER_H
