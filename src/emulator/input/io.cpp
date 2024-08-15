#include "io.h"

uint8_t io::read(std::weak_ptr<cpu> cpu, uint16_t address)
{
    //serial transfer data
    if (address == 0xFF01) {
        return serialData[0];
    }

    //serial transfer control
    if (address == 0xFF02) {
        return serialData[1];
    }
    if (auto m_cpu = cpu.lock())
    {
        //if (utility::inRange(address, 0xFF04, 0xFF07)) {
        //    return m_cpu->(address);
       // }

    
        if (address == 0xFF0F) {
            return m_cpu->getInterruptFlags();
        }
    }
    GBE_ERROR("UNSUPPORTED read: {}", address);
    return 0;
}

void io::write(std::weak_ptr<cpu> cpu, uint16_t address, uint8_t value)
{
    if (address == 0xFF01) {
        serialData[0] = value;
        return;
    }

    if (address == 0xFF02) {
        serialData[1] = value;
        return;
    }

    //if (utility::inRange(address, 0xFF04, 0xFF07)) {
    //    timer_write(address, value);
    //    return;
    //}

    if (auto m_cpu = cpu.lock())
    {
        if (address == 0xFF0F) {
            m_cpu->setInterruptFlags(value);
            return;
        }
    }

    GBE_ERROR("UNSUPPORTED read: {}", address);
}
