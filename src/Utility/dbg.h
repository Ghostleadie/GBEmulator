//
// Created by Jack_ on 17/09/2025.
//

#ifndef GAMEBOYEMULATOR_DBG_H
#define GAMEBOYEMULATOR_DBG_H


class bus;

class dbg {
public:
	static void dbgUpdate(const std::weak_ptr<bus>& m_bus);
	static void dbgPrint();
};


#endif //GAMEBOYEMULATOR_DBG_H