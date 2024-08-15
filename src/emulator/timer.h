#pragma once
class interrupts;

struct timerContext
{
	uint16_t div;
	uint8_t tima;
	uint8_t tma;
	uint8_t tac;
};

class timer
{
public:
	void init();
	void tick(std::weak_ptr<interrupts> interrupts);

	void write(uint16_t address, uint8_t value);
	uint8_t read(uint16_t address);

	timerContext& getTimerContext();

private:
	timerContext timerCtx;
};