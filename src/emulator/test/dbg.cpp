#include "dbg.h"

static char dbgMsg[1024] = { 0 };
static int msgSize = 0;

void dbg_update(std::weak_ptr<bus> bus)
{
    if (auto m_bus = bus.lock())
    {
        if (m_bus->read(0xFF02) == 0x81) {
            char c = m_bus->read(0xFF01);

            dbgMsg[msgSize++] = c;

            m_bus->write((uint16_t)0xFF02, (uint16_t)0);
        }
    }
}

void dbg_print()
{
    if (dbgMsg[0]) {
        GBE_INFO("DBG: {}", dbgMsg);
    }
}
