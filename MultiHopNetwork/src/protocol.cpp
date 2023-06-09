#include "protocol.h"

std::set<int> validControlPackageValues{CONNECT, CONNACK, PUBLISH, PUBACK, SUBSCRIBE, SUBACK, DISCONNECT};

Message parseIncomingPacket(uint8_t *incomingPacket, size_t packetLength)
{
    if (packetLength < HEADER_SIZE)
    {
        throw std::invalid_argument("Insufficient space for header");
    }
    FixedHeader packetHeader = parseFixedHeader(incomingPacket);
    if (packetLength < packetHeader.packetLength)
    {
        throw std::invalid_argument("Packet length in header exceeds available space");
    }
    std::unique_ptr<VariableHeader> packetVariableHeader = parseVariableHeader(packetHeader.controlPacketType, incomingPacket + HEADER_SIZE, packetLength - HEADER_SIZE);
    if (HEADER_SIZE + packetVariableHeader->size > packetLength)
    {
        throw std::invalid_argument("Insufficient space for packet payload");
    }
    std::string packetPayload(incomingPacket + HEADER_SIZE + packetVariableHeader->size, incomingPacket + packetLength);

    Message finalMessage = Message(packetHeader, std::move(packetVariableHeader), packetPayload);
    return finalMessage;
}

const uint8_t CONTROL_PACKET_TYPE_MASK = 0xF0;
const uint8_t CONTROL_PACKET_TYPE_SHIFT = 4;

const uint8_t DUPLICATE_FLAG_MASK = 0b00001000;
const uint8_t QOS_LEVEL_MASK = 0b00000110;
const uint8_t RETAIN_FLAG_MASK = 0b00000001;

FixedHeader parseFixedHeader(uint8_t *headerBytes)
{
    uint8_t controlPacketTypeIndex = (headerBytes[0] & CONTROL_PACKET_TYPE_MASK) >> CONTROL_PACKET_TYPE_SHIFT;
    ControlPacketType controlPacketType = static_cast<ControlPacketType>(controlPacketTypeIndex);

    if (validControlPackageValues.find(controlPacketType) == validControlPackageValues.end())
    {
        throw std::invalid_argument("Invalid control packet type: " + controlPacketType);
    }

    bool isDuplicate = (headerBytes[0] & DUPLICATE_FLAG_MASK);
    int qosLevel = (headerBytes[0] & QOS_LEVEL_MASK) >> 1;
    bool isRetain = (headerBytes[0] & RETAIN_FLAG_MASK) > 0;
    int packetLength = headerBytes[1];

    return FixedHeader(controlPacketType, isDuplicate, isRetain, qosLevel, packetLength);
}

std::unique_ptr<VariableHeader> parseVariableHeader(ControlPacketType controlPacketType, uint8_t *serializedVariableHeader, size_t variableHeaderLength)
{
    std::unique_ptr<VariableHeader> variableHeader;
    switch (controlPacketType)
    {
    case CONNECT:
    {
        uint8_t protocolVersion = serializedVariableHeader[0];
        std::array<uint8_t, 16> uuid;
        for (int i = 0; i < 16; i++)
        {
            uuid[i] = serializedVariableHeader[i + 1];
        }
        variableHeader = std::unique_ptr<VariableHeader>(new ConnectHeader(protocolVersion, uuid));
        break;
    }

    case CONNACK:
    {
        ConnackReturnCode returnCode = static_cast<ConnackReturnCode>(serializedVariableHeader[0]);
        uint8_t networkID = serializedVariableHeader[1];
        std::array<uint8_t, 16> uuid;
        for (int i = 0; i < 16; i++)
        {
            uuid[i] = serializedVariableHeader[i + 2];
        }
        variableHeader = std::unique_ptr<VariableHeader>(new ConnackHeader(returnCode, networkID, uuid));
        break;
    }

    case PUBLISH:
    {
        uint8_t topicNameLength = serializedVariableHeader[0];
        std::string topicName(serializedVariableHeader + 1, serializedVariableHeader + topicNameLength + 1);

        uint16_t packetID = (serializedVariableHeader[topicNameLength + 1] << 8) | serializedVariableHeader[topicNameLength + 2];

        variableHeader = std::unique_ptr<VariableHeader>(new PublishHeader(topicNameLength, topicName, packetID));
        break;
    }
    case PUBACK:
    {
        uint16_t packetID = (serializedVariableHeader[0] << 8) | serializedVariableHeader[1];

        variableHeader = std::unique_ptr<VariableHeader>(new PubackHeader(packetID));
        break;
    }

    case SUBSCRIBE:
    {
        uint8_t topicNameLength = serializedVariableHeader[0];
        std::string topicName(serializedVariableHeader + 1, serializedVariableHeader + topicNameLength);

        uint16_t packetID = (serializedVariableHeader[topicNameLength + 1] << 8) | serializedVariableHeader[topicNameLength + 2];

        variableHeader = std::unique_ptr<VariableHeader>(new SubscribeHeader(topicNameLength, topicName, packetID));

        break;
    }

    case SUBACK:
    {
        uint16_t packetID = (serializedVariableHeader[0] << 8) | serializedVariableHeader[1];

        variableHeader = std::unique_ptr<VariableHeader>(new SubackHeader(packetID));
        break;
    }
    case DISCONNECT:
    {
        std::array<uint8_t, 16> uuid;
        for (int i = 0; i < 16; i++)
        {
            uuid[i] = serializedVariableHeader[i];
        }
        variableHeader = std::unique_ptr<VariableHeader>(new DisconnectHeader(uuid));
        break;
    }
    }

    if (variableHeader->size > variableHeaderLength)
    {
        throw std::invalid_argument("Insufficient space for VariableHeader");
    }

    return variableHeader;
}

void serializeHeader(FixedHeader &header, uint8_t *serializedHeaderLocation, size_t reservedSpace)
{
    if (reservedSpace < HEADER_SIZE)
    {
        throw std::invalid_argument("Allocated Space too small.");
    }
    uint8_t controlPacketIndex = static_cast<uint8_t>(header.controlPacketType);
    serializedHeaderLocation[0] = (controlPacketIndex << CONTROL_PACKET_TYPE_SHIFT) & CONTROL_PACKET_TYPE_MASK;

    if (header.isDuplicate)
    {
        serializedHeaderLocation[0] |= DUPLICATE_FLAG_MASK;
    }
    serializedHeaderLocation[0] |= (header.qosLevel << 1) & QOS_LEVEL_MASK;
    if (header.retain)
    {
        serializedHeaderLocation[0] |= RETAIN_FLAG_MASK;
    }

    serializedHeaderLocation[1] = header.packetLength;
}

constexpr size_t UUID_SIZE = 16;
constexpr uint8_t SHIFT_BYTE = 8;

void serializeVariableHeader(VariableHeader *variableHeader, uint8_t *serializedVariableHeaderLocation, size_t reservedSpace)
{
    if (reservedSpace < variableHeader->size)
    {
        throw std::invalid_argument("Allocated Space too small.");
    }
    switch (variableHeader->controlPacketType)
    {
    case CONNECT:
    {
        ConnectHeader *connectHeader = static_cast<ConnectHeader *>(variableHeader);
        if (connectHeader)
        {
            serializedVariableHeaderLocation[0] = connectHeader->protocolVersion;
            for (int i = 0; i < 16; i++)
            {
                serializedVariableHeaderLocation[i + 1] = connectHeader->uuid[i];
            }
        }
        break;
    }
    case CONNACK:
    {
        ConnackHeader *connackHeader = static_cast<ConnackHeader *>(variableHeader);
        if (connackHeader)
        {
            uint8_t returnCode = static_cast<uint8_t>(connackHeader->returnCode);
            serializedVariableHeaderLocation[0] = returnCode;
            serializedVariableHeaderLocation[1] = connackHeader->networkID;
            for (int i = 0; i < 16; i++)
            {
                serializedVariableHeaderLocation[i + 2] = connackHeader->uuid[i];
            }
        }
        break;
    }
    case PUBLISH:
    {
        PublishHeader *publishHeader = static_cast<PublishHeader *>(variableHeader);
        if (publishHeader)
        {
            serializedVariableHeaderLocation[0] = publishHeader->topicNameLength;
            std::copy(publishHeader->topicName.begin(), publishHeader->topicName.end(), serializedVariableHeaderLocation + 1);

            serializedVariableHeaderLocation[publishHeader->topicNameLength + 1] = publishHeader->packetID >> SHIFT_BYTE;
            serializedVariableHeaderLocation[publishHeader->topicNameLength + 2] = publishHeader->packetID & 0xFF;
        }
        break;
    }
    case PUBACK:
    {
        PubackHeader *pubackHeader = static_cast<PubackHeader *>(variableHeader);
        if (pubackHeader)
        {
            serializedVariableHeaderLocation[0] = pubackHeader->packetID >> 8;
            serializedVariableHeaderLocation[1] = pubackHeader->packetID & 0xFF;
        }
        break;
    }
    case SUBSCRIBE:
    {
        SubscribeHeader *subscribeHeader = static_cast<SubscribeHeader *>(variableHeader);
        if (subscribeHeader)
        {
            serializedVariableHeaderLocation[0] = subscribeHeader->topicNameLength;
            std::copy(subscribeHeader->topicName.begin(), subscribeHeader->topicName.end(), serializedVariableHeaderLocation + 1);

            serializedVariableHeaderLocation[subscribeHeader->topicNameLength + 1] = subscribeHeader->packetID >> SHIFT_BYTE;
            serializedVariableHeaderLocation[subscribeHeader->topicNameLength + 2] = subscribeHeader->packetID & 0xFF;
        }
        break;
    }
    case SUBACK:
    {
        SubackHeader *subackHeader = static_cast<SubackHeader *>(variableHeader);
        if (subackHeader)
        {
            serializedVariableHeaderLocation[0] = subackHeader->packetID >> 8;
            serializedVariableHeaderLocation[1] = subackHeader->packetID & 0xFF;
        }
        break;
    }
    case DISCONNECT:
    {
        DisconnectHeader *disconnectHeader = static_cast<DisconnectHeader *>(variableHeader);
        if (disconnectHeader)
        {
            for (int i = 0; i < 16; i++)
            {
                serializedVariableHeaderLocation[i] = disconnectHeader->uuid[i];
            }
        }
        break;
    }
    }
}

void serializeMessage(Message &message, uint8_t *serializedMessageLocation, size_t reservedSpace)
{
    serializeVariableHeader(message.variableHeader.get(), serializedMessageLocation + HEADER_SIZE, reservedSpace - HEADER_SIZE);
    size_t summedHeaderSize = message.variableHeader->size + HEADER_SIZE;

    if (message.payload.size() > MAX_MESSAGE_SIZE - summedHeaderSize)
    {
        throw std::invalid_argument("Allocated Space too small.");
    }

    message.header.packetLength = summedHeaderSize + message.payload.size();

    serializeHeader(message.header, serializedMessageLocation, reservedSpace);

    std::copy(message.payload.begin(), message.payload.end(), serializedMessageLocation + summedHeaderSize);
}
