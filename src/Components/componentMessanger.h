//
// Created by Jack_ on 30/08/2025.
//

#ifndef GAMEBOYEMULATOR_COMPONENTMESSANGER_H
#define GAMEBOYEMULATOR_COMPONENTMESSANGER_H

class component;

class componentMessanger
{
public:
    virtual void sendMessage(const std::string& message, component* sender) = 0;
};


#endif //GAMEBOYEMULATOR_COMPONENTMESSANGER_H
