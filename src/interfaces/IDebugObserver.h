//
// Created by Jack_ on 04/09/2025.
//

#ifndef GAMEBOYEMULATOR_IDEBUGOBSERVER_H
#define GAMEBOYEMULATOR_IDEBUGOBSERVER_H
#include <cstdint>


class IDebugObserver
{
public:
	virtual ~IDebugObserver() = default;

	virtual void onComponentUpdated(const std::string& name, const std::string& info) = 0;
};


#endif //GAMEBOYEMULATOR_IDEBUGOBSERVER_H