#pragma once

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <set>
#include <vector>

#include <protocol.h>
#include <config.h>

class MeshNetwork
{
private:
    RH_RF95 rf95;
    RHMesh manager;
    std::function<void(Message &, uint8_t from)> handleMessage;
    std::function<void(UpdateBlock updateBlock)> handleUpdateMessage;

public:
    MeshNetwork(uint8_t networkAddress, std::function<void(Message &, uint8_t from)> handleMessage, std::function<void(UpdateBlock updateBlock)> handleUpdateMessage = nullptr)
        : rf95(LLG_CS, LLG_DI0), manager(rf95, networkAddress), handleMessage(handleMessage), handleUpdateMessage(handleUpdateMessage) {}

    bool setup();
    void loop();
    bool sendMessage(uint8_t to, Message &msg);
    void updateNetworkId(uint8_t newID);
    uint8_t getCurrentNetworkId();
    void broadcastUpdateBlock(UpdateBlock updateBlock);
};