//
// Created by Jack_ on 08/09/2025.
//

#ifndef GAMEBOYEMULATOR_OPCODEFUNCTIONS_H
#define GAMEBOYEMULATOR_OPCODEFUNCTIONS_H
#include <memory>

#include "../../emulator.h"

class cpu;

class OpcodeCommand {
public:
	virtual ~OpcodeCommand() = default;
	virtual void execute(cpu& m_cpu) = 0;
};

class AdcCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

// 8-bit ADD command
class Add8BitCommand : public OpcodeCommand {
public:
	void execute(cpu& m_cpu) override;
};

// 16-bit ADD command
class Add16BitCommand : public OpcodeCommand {
public:
	void execute(cpu& m_cpu) override;
};

// SP-specific ADD command
class AddSPCommand : public OpcodeCommand {
public:
	void execute(cpu& m_cpu) override;
};

class AndCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class CallCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class CcfCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class CbCommand : public OpcodeCommand
{
public:
	void execute(cpu& m_cpu) override;
};

class CplCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class CpCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class DaaCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class DecCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class DiCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class EiCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class HaltCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class IncCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class JpCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class JphlCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class JrCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class Ld8BitCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

class Ld16BitCommand : public OpcodeCommand {
public:
	void execute(cpu& m_cpu) override;
};

class LdSpecialCommand : public OpcodeCommand {
public:
	void execute(cpu& m_cpu) override;
};

class LdhCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class NopCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class OrCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class PopCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class PushCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class RetCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class RetiCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class RlaCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class RlcaCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class RraCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class RrcaCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class RstCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class SbcCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class ScfCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class StopCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class SubCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};


class XorCommand : public OpcodeCommand {
public:
    void execute(cpu& m_cpu) override;
};

// Command Factory that integrates with existing opcodeFactory
class OpcodeCommandFactory {
public:
	static std::unique_ptr<OpcodeCommand> createCommand(uint8_t opcode);
};


#endif //GAMEBOYEMULATOR_OPCODEFUNCTIONS_H