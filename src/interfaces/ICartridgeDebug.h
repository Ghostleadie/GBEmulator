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

	/** Returns a read-only view of the cartridge's current state for debugging.
	 * @return const reference to the implementer's live cartridgeContext (no copy; valid while the cartridge lives).
	 */
	virtual const cartridgeContext& peekCartridgeContext() const = 0;
};


#endif //GAMEBOYEMULATOR_ICARTRIDGEDEBUG_H