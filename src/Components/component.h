//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_COMPONENT_H
#define GAMEBOYEMULATOR_COMPONENT_H
#include "componentMessanger.h"


class component
{
public:
    component(componentMessanger* compMessanger = nullptr) : messanger(compMessanger) {};
    void setComponentMessanger(componentMessanger* messanger);

protected:
    componentMessanger* messanger;
};


#endif //GAMEBOYEMULATOR_COMPONENT_H