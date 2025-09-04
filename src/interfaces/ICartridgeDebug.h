//
// Created by Jack_ on 04/09/2025.
//

#ifndef GAMEBOYEMULATOR_ICARTRIDGEDEBUG_H
#define GAMEBOYEMULATOR_ICARTRIDGEDEBUG_H


struct cartridgeContext;

class ICartridgeDebug
{
public:
	virtual ~ICartridgeDebug() = default;

	virtual const cartridgeContext& peekCartridgeContext() const = 0;
};


#endif //GAMEBOYEMULATOR_ICARTRIDGEDEBUG_H