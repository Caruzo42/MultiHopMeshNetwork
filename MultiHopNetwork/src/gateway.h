#pragma once

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <array>
#include <map>

#include "network.h"
#include "config.h"
#include "protocol.h"
#include "protocol_common.h"
#include "display.h"

class NodeManager
{
private:
    std::map<uint8_t, std::array<uint8_t, 16>> connectedNodes;
    uint8_t nextNetworkID;

public:
    NodeManager();

    bool addNode(uint8_t networkID, std::array<uint8_t, 16> uuid);
    uint8_t getNextNetworkID() const;
    bool deleteNode(uint8_t networkID);
    std::array<uint8_t, 16> getUUIDByNetworkID(uint8_t networkID);
    int getConnectedNodesCount();
};

void handle(Message &msg, uint8_t from);

void setup();
void loop();