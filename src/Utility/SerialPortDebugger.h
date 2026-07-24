//
// Created by Jack_ on 13/10/2025.
//

#ifndef GAMEBOYEMULATOR_SERIALPORTDEBUGGER_H
#define GAMEBOYEMULATOR_SERIALPORTDEBUGGER_H


#include "../interfaces/IComponentMessanger.h"

class bus;

/**
 * Captures the Game Boy serial port (SB/SC at 0xFF01/0xFF02) and logs each
 * completed line, used to read Blargg test-ROM output.
 */
class SerialPortDebugger {
public:
	/** Constructs an empty debugger with a cleared message buffer. */
	SerialPortDebugger();

	/**
	 * Polls the serial registers and appends any transferred byte to the buffer.
	 * @param m_bus Message bus used to read SB/SC (0xFF01/0xFF02) and acknowledge transfers.
	 * @return True when a newline completed a line and the caller should print then clear.
	 */
	bool update(IComponentMessanger& m_bus);

	/** Logs the buffered line, flagging Blargg "Passed"/"Failed" verdicts, then clears it. */
	void print();

	/** Resets the message buffer to empty. */
	void clear();

	/** @return The NUL-terminated buffered serial message. */
	const char* getMessage() const { return dbg_msg; }

	/** @return The number of characters currently buffered. */
	int getMessageSize() const { return msg_size; }

private:
	static constexpr int BUFFER_SIZE = 1024;
	char dbg_msg[BUFFER_SIZE];
	int msg_size;
};


#endif //GAMEBOYEMULATOR_SERIALPORTDEBUGGER_H
