#include "node.h"

unsigned long nextMsgTime;

MeshNetwork network(HARDCODED_NETWORK_ID ? HARDCODED_NETWORK_ID : INITIAL_NODE_ADDRESS, handle, handleUpdateMessage);
Measurements measurements;

std::array<uint8_t, 16> uuid;

bool acknowledged = false;
DisplayHandler displayHandler;

volatile bool buttonPressed = false;
unsigned long connectStartTime;

std::unordered_map<uint32_t, bool> receivedPackets;

void setup()
{
    Preferences prefs;
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);
    prefs.begin("network", false);
    displayHandler.init();

    if (USING_DEFAULT_UUID)
    {
        std::copy(HARDCODED_UUID, HARDCODED_UUID + 16, uuid.begin());
    }
    else if (!prefs.getBytesLength("uuid") || REGENERATING_UUID_EACH_START)
    {
        generateUUID(uuid.data());

        int timer = TIME_TILL_UUID_IS_SAVED;
        while (timer >= 0 && !buttonPressed)
        {
            delay(1000);
            displayHandler.displayUUID(uuid);
            displayHandler.displayInstructions(timer);
            timer--;
        }
        prefs.putBytes("uuid", uuid.data(), 16);
    }

    else
    {
        prefs.getBytes("uuid", uuid.data(), 16);
    }
    prefs.end();

    network.setup();

    connectStartTime = millis();

    displayHandler.clearScreen();
    displayHandler.displayNodeId(network.getCurrentNetworkId());
}

uint8_t res;

void loop()
{
    if (!acknowledged)
    {
        if (millis() - connectStartTime > CONNECT_TIMEOUT)
        {
            Message msg = createConnectionMessage(uuid);
            network.sendMessage(GATEWAY_ADDRESS, msg);
            displayHandler.displaySentControlPacket(msg.header.controlPacketType);
            connectStartTime = millis();
        }
    }
    else if (millis() > nextMsgTime)
    {
        Message message = createPublishMessage("v1/backend/measurements", 1234, "measurement: " + std::to_string(measurements.getRandomBatteryLevel()), false, false, 1);

        try
        {
            network.sendMessage(GATEWAY_ADDRESS, message);
            displayHandler.displaySentControlPacket(message.header.controlPacketType);
        }
        catch (std::exception e)
        {
            Serial.println(e.what());
        }
        nextMsgTime += NODE_PUBLISH_INTERVAL;
    }
    network.loop();
}

void handleButtonPress()
{
    buttonPressed = true;
}

void handle(Message &msg, uint8_t from)
{

    switch (msg.variableHeader->controlPacketType)
    {
    case CONNACK:
    {
        ConnackHeader *connackHeader = static_cast<ConnackHeader *>(msg.variableHeader.get());

        if (connackHeader->returnCode == ACCEPTED && uuid == connackHeader->uuid)
        {
            Preferences prefs;
            if (HARDCODED_NETWORK_ID)
            {
                network.updateNetworkId(HARDCODED_NETWORK_ID);
            }
            else
            {
                network.updateNetworkId(connackHeader->networkID);
            }
            acknowledged = true;
        }
        break;
    }
    case PUBLISH:
    {
        PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

        // TODO: DO SOMETHING WITH RECEIVED MESSAGE (not relevant for our project)
        break;
    }
    case PUBACK:
    {
        PubackHeader *pubackHeader = static_cast<PubackHeader *>(msg.variableHeader.get());

        // TODO: MARK MESSAGE AS RECEIVED ON GATEWAY (or similar. Not relevant for our project)
        break;
    }
    case SUBACK:
    {
        SubackHeader *subackHeader = static_cast<SubackHeader *>(msg.variableHeader.get());

        // TODO: MARK TOPIC AS SUCCESSFULLY SUBSCRIBED (or similar. Not relevant for our project)
        break;
    }

    default:
        Serial.println("Something else happened:");
        Serial.println(msg.toString().c_str());
        break;
    }
    displayHandler.clearScreen();
    displayHandler.displayMessage(true, from, network.getCurrentNetworkId(), msg);
    displayHandler.displayNodeId(network.getCurrentNetworkId());
}

void handleUpdateMessage(UpdateBlock updateBlock)
{
    uint32_t key = makeKey(updateBlock.versionNumber, updateBlock.blockIndex);
    if (!checkIfPreviouslyReceived(updateBlock.versionNumber, updateBlock.blockIndex))
    {
        receivedPackets[key] = true;
        network.broadcastUpdateBlock(updateBlock);
    }

    // TODO @UPDATE-GROUP: handle the packet. [Changed type to struct UpdateBlock, as defined in protocol_common.h]
}

bool checkIfPreviouslyReceived(uint16_t version, uint16_t blockIndex)
{
    uint32_t key = makeKey(version, blockIndex);
    return receivedPackets.find(key) != receivedPackets.end();
}

uint32_t makeKey(uint16_t version, uint16_t blockIndex)
{
    return (static_cast<uint32_t>(version) << 16) | blockIndex;
}

void requestMissingPacket(uint16_t blockIndex)
{
    // TODO @UPDATE-GROUP: Invoke this method a block. Handle the request in Gateway.cpp.
    std::string payload = "{\n  \"content\": {\n    \"missingBlockIndex\": " + std::to_string(blockIndex) + "\n  }\n}";
    Message message = createPublishMessage("v1/updates/missing", 1234, payload, false, false, 0);

    try
    {
        network.sendMessage(GATEWAY_ADDRESS, message);
    }
    catch (std::exception e)
    {
        Serial.println(e.what());
    }
}

void generateUUID(byte *uuid)
{
    for (int i = 0; i < 16; i++)
    {
        uuid[i] = esp_random() % 256;
    }
}
