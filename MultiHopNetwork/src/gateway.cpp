#include "gateway.h"

MeshNetwork gatewayNetwork(GATEWAY_ADDRESS, handle);
Gateway gateway;

void handle(Message &msg, uint8_t from)
{
    switch (msg.variableHeader->controlPacketType)
    {
    case CONNECT:
    {
        ConnectHeader *connectHeader = static_cast<ConnectHeader *>(msg.variableHeader.get());
        uint8_t networkID = gateway.getNextNetworkID();
        gateway.addNode(gateway.getNextNetworkID(), connectHeader->uuid);
        Message ackMessage = createConnackMessage(msg, networkID);
        gatewayNetwork.sendMessage(from, ackMessage);
        break;
    }

    case PUBLISH:
    {
        PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

        if (msg.header.qosLevel == 1)
        {
            Message ackMessage = createPubackMessage(msg);
            gatewayNetwork.sendMessage(from, ackMessage);
        }

        // TODO @GateWay: DO SOMETHING WITH RECEIVED MESSAGE

        // Topic Name:
        std::string topicName = publishHeader->topicName;
        // Payload:
        std::string payload = msg.payload;

        // NetworkID (dont think you need it but whatever):
        uint8_t networkID = from;
        // UUID:
        std::array<uint8_t, 16> uuid = gateway.getUUIDByNetworkID(networkID);

        break;
    }

    case PUBACK:
    {
        PubackHeader *pubackHeader = static_cast<PubackHeader *>(msg.variableHeader.get());

        // TODO @noone: MARK MESSAGE AS RECEIVED ON NODE (or similar. Not relevant for our project)

        break;
    }

    case SUBSCRIBE:
    {
        SubscribeHeader *subscribeHeader = static_cast<SubscribeHeader *>(msg.variableHeader.get());

        // TODO @noone: SUBSCRIBE NODES TO TOPICS (not relevant for our project)

        Message ackMessage = createSubackMessage(msg);
        gatewayNetwork.sendMessage(from, ackMessage);
        break;
    }

    case DISCONNECT:
        DisconnectHeader *disconnectHeader = static_cast<DisconnectHeader *>(msg.variableHeader.get());

        gateway.deleteNode(from);
        break;
    }
}

void setup()
{
    gatewayNetwork.setup();
}

void loop()
{
    gatewayNetwork.loop();
}

Gateway::Gateway() : nextNetworkID(1) {}

bool Gateway::addNode(uint8_t networkID, std::array<uint8_t, 16> uuid)
{
    for (auto it = connectedNodes.begin(); it != connectedNodes.end(); ++it)
    {
        if (it->second == uuid)
        {
            connectedNodes.erase(it);
            break;
        }
    }

    if (connectedNodes.find(networkID) != connectedNodes.end())
    {
        return false;
    }
    connectedNodes[networkID] = uuid;
    while (connectedNodes.find(nextNetworkID) != connectedNodes.end())
    {
        nextNetworkID++;
    }
    return true;
}

uint8_t Gateway::getNextNetworkID() const
{
    return nextNetworkID;
}

bool Gateway::deleteNode(uint8_t networkID)
{
    auto it = connectedNodes.find(networkID);
    if (it == connectedNodes.end())
    {
        return false;
    }

    connectedNodes.erase(it);
    if (networkID < nextNetworkID)
    {
        nextNetworkID = networkID;
    }

    return true;
}

std::array<uint8_t, 16> Gateway::getUUIDByNetworkID(uint8_t networkID)
{
    return connectedNodes[networkID];
}