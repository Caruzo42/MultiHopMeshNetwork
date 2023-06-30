#include <protocol_common.h>

std::string Message::toString()
{
    std::string str = "Message:";
    str += "\n" + header.toString();
    str += "\n" + variableHeader->toString();
    if (header.controlPacketType == PUBLISH)
    {
        str += "\nPayload: ";
        str += payload;
    }

    return str;
}

std::string FixedHeader::toString()
{
    std::string str = "Fixed Header: ";

    str += "ControlPacketType: ";
    str += controlPacketTypeToString(controlPacketType);
    str += ", IsDuplicate: ";
    str += std::to_string(isDuplicate);
    str += ", Retain: ";
    str += std::to_string(retain);
    str += ", QOS Level: ";
    str += std::to_string(qosLevel);
    str += ", Packet Length: ";
    str += std::to_string(packetLength);
    return str;
}

Message dummyMessage()
{
    FixedHeader header(CONNECT, false, false, 0, 20);

    std::array<uint8_t, 16> uuid;
    for (int i = 0; i < 16; i++)
    {
        uuid[i] = (uint8_t)11;
    }

    auto variableHeader = std::unique_ptr<VariableHeader>(new ConnectHeader(0, uuid));

    return Message(header, std::move(variableHeader), "");
}

Message createConnectionMessage(const std::array<uint8_t, 16> uuid)
{
    FixedHeader fixedHeader = FixedHeader(CONNECT, false, false, 1, 19);

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new ConnectHeader(1, uuid));

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createConnackMessage(const Message &msg, const uint8_t networkID, const ConnackReturnCode returnCode)
{
    // Request Header
    ConnectHeader *connectHeader = static_cast<ConnectHeader *>(msg.variableHeader.get());

    FixedHeader fixedHeader = FixedHeader(CONNACK, false, msg.header.retain, msg.header.qosLevel, 4);

    auto variableHeader = std::unique_ptr<VariableHeader>(new ConnackHeader(returnCode, networkID, connectHeader->uuid));

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}
Message createPublishMessage(const std::string topicName, const uint16_t packetID, const std::string payload, const bool duplicate, const bool retain, const uint8_t qosLevel)
{

    auto variableHeader = std::unique_ptr<VariableHeader>(new PublishHeader(topicName.length(), topicName, packetID));

    FixedHeader fixedHeader = FixedHeader(PUBLISH, duplicate, retain, qosLevel, HEADER_SIZE + variableHeader->size);

    Message response = Message(fixedHeader, std::move(variableHeader), payload);
    return response;
}

Message createPubackMessage(const Message &msg)
{
    // Request Header
    PublishHeader *publishHeader = static_cast<PublishHeader *>(msg.variableHeader.get());

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new PubackHeader(publishHeader->packetID));

    FixedHeader fixedHeader = FixedHeader(PUBACK, false, msg.header.retain, msg.header.qosLevel, variableHeader.get()->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createSubscribeMessage(const std::string topicName, const uint16_t packetID, const bool duplicate, const bool retain, const uint8_t qosLevel)
{
    auto variableHeader = std::unique_ptr<VariableHeader>(new SubscribeHeader(topicName.length(), topicName, packetID));

    FixedHeader fixedHeader = FixedHeader(SUBSCRIBE, duplicate, retain, qosLevel, HEADER_SIZE + variableHeader->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createSubackMessage(const Message &msg)
{
    SubscribeHeader *subscribeHeader = static_cast<SubscribeHeader *>(msg.variableHeader.get());

    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new SubackHeader(subscribeHeader->packetID));

    FixedHeader fixedHeader = FixedHeader(SUBACK, false, msg.header.retain, msg.header.qosLevel, variableHeader.get()->size);

    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

Message createDisconnectMessage(const std::array<uint8_t, 16> uuid)
{
    FixedHeader fixedHeader = FixedHeader(DISCONNECT, false, false, 0, 0);
    std::unique_ptr<VariableHeader> variableHeader = std::unique_ptr<VariableHeader>(new DisconnectHeader(uuid));
    Message response = Message(fixedHeader, std::move(variableHeader), "");
    return response;
}

UpdateBlock createUpdateBlock(uint16_t versionNumber, uint16_t numberOfBlocks, uint16_t blockIndex, std::vector<uint8_t> &blockContent)
{
    UpdateBlock block;
    block.versionNumber = versionNumber;
    block.numberOfBlocks = numberOfBlocks;
    block.blockIndex = blockIndex;
    block.blockContent = blockContent;

    return block;
}

UpdateBlock parseUpdateBlock(const uint8_t *receivedPacket, size_t size)
{
    if (size < 7)
    {
        throw std::invalid_argument("Packet too small to be valid!");
    }

    if (receivedPacket[0] != 0xFF)
    {
        throw std::invalid_argument("Malformed Packet!");
    }
    UpdateBlock block;
    block.versionNumber = (receivedPacket[1] << 8) | receivedPacket[2];
    block.numberOfBlocks = (receivedPacket[3] << 8) | receivedPacket[4];
    block.blockIndex = (receivedPacket[5] << 8) | receivedPacket[6];
    block.blockContent.assign(receivedPacket + 7, receivedPacket + size);

    return block;
}

void serializeUpdateBlock(uint8_t *updatePacketLocation, size_t reservedSize, UpdateBlock updateBlock)
{
    if (reservedSize < updateBlock.blockContent.size() + 7)
    {
        throw std::invalid_argument("Insufficient reserved size for the block content!");
    }

    updatePacketLocation[0] = 0xFF;
    updatePacketLocation[1] = updateBlock.versionNumber >> 8;
    updatePacketLocation[2] = updateBlock.versionNumber;
    updatePacketLocation[3] = updateBlock.numberOfBlocks >> 8;
    updatePacketLocation[4] = updateBlock.numberOfBlocks;
    updatePacketLocation[5] = updateBlock.blockIndex >> 8;
    updatePacketLocation[6] = updateBlock.blockIndex;
    std::memcpy(updatePacketLocation + 7, updateBlock.blockContent.data(), updateBlock.blockContent.size());
}