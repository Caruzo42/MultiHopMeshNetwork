#pragma once

#include <SPI.h>
#include <cstring>
#include <vector>

#include "config.h"
#include "variable_headers.h"

struct FixedHeader
{
    ControlPacketType controlPacketType;
    bool isDuplicate;
    bool retain;
    uint8_t qosLevel;
    uint8_t packetLength;
    FixedHeader(ControlPacketType cpt, bool duplicate, bool r, uint8_t qos, uint8_t length)
        : controlPacketType(cpt), isDuplicate(duplicate), retain(r), qosLevel(qos), packetLength(length) {}
    std::string toString();
};

struct Message
{
    FixedHeader header;
    std::unique_ptr<VariableHeader> variableHeader;
    std::string payload;

    Message(FixedHeader hdr, std::unique_ptr<VariableHeader> vHdr, std::string pl) : header(hdr), variableHeader(std::move(vHdr)), payload(pl) {}
    std::string toString();
};

struct UpdateBlock
{
    uint16_t versionNumber;
    uint16_t numberOfBlocks;
    uint16_t blockIndex;
    std::vector<uint8_t> blockContent;
};

Message createConnectionMessage(const std::array<uint8_t, 16> uuid);

Message createConnackMessage(const Message &msg, const uint8_t networkID, const ConnackReturnCode returnCode = ACCEPTED);

Message createPublishMessage(const std::string topicName, const uint16_t packetID, const std::string payload, const bool duplicate = false, const bool retain = false, const uint8_t qosLevel = 0);

Message createPubackMessage(const Message &msg);

Message createSubscribeMessage(const std::string topicName, const uint16_t packetID, const bool duplicate = false, const bool retain = false, const uint8_t qosLevel = 1);

Message createSubackMessage(const Message &msg);

Message createDisconnectMessage(const std::array<uint8_t, 16> uuid);

UpdateBlock parseUpdateBlock(const uint8_t *receivedPacket, size_t size);

void serializeUpdateBlock(uint8_t *updatePacketLocation, size_t reservedSize, UpdateBlock updateBlock);

UpdateBlock createUpdateBlock(uint16_t versionNumber, uint16_t numberOfBlocks, uint16_t blockIndex, std::vector<uint8_t> &blockContent);
