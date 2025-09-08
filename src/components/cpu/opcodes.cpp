//
// Created by Jack_ on 01/09/2025.
//

#include "opcodes.h"

opcode::opcode()
{
	name = "";
	type = opcodeType::OP_NONE;
	mode = addressMode::AM_IMP;
	reg1 = registryType::RT_NONE;
	reg2 = registryType::RT_NONE;
	cond = conditionType::CT_NONE;
	param = 0;
}

opcode::opcode(const std::string& m_name, const opcodeType m_type, const addressMode m_mode, const registryType m_reg1, const registryType m_reg2, const conditionType m_cond, const uint8_t m_param)
{
	name = m_name;
	type = m_type;
	mode = m_mode;
	reg1 = m_reg1;
	reg2 = m_reg2;
	cond = m_cond;
	param = m_param;
}