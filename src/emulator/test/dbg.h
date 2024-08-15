#pragma once

#include "../cpu/cpu.h"
#include "../bus.h"

void dbg_update(std::weak_ptr<bus> bus);
void dbg_print();