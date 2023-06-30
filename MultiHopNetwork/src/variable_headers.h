#pragma once

#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

enum ControlPacketType
{
    CONNECT = 1,
    CONNACK = 2,
    PUBLISH = 3,
    PUBACK = 4,
    SUBSCRIBE = 8,
    SUBACK = 9,
    DISCONNECT = 14
};

enum ConnackReturnCode
{
    ACCEPTED = 0,
    UNSUPPORTED_VERSION = 1,
    UNAVAILABLE = 3,
    UNAUTHORIZED = 5,
};

struct VariableHeader
{
    size_t size;
    ControlPacketType controlPacketType;

    VariableHeader(size_t size, ControlPacketType controlPacketType) : size(size), controlPacketType(controlPacketType) {}
    const virtual std::string toString();
    virtual ~VariableHeader() = default;
};

struct ConnectHeader : VariableHeader
{
    uint8_t protocolVersion;
    const std::array<uint8_t, 16> uuid;

    ConnectHeader(uint8_t protocolVersion, std::array<uint8_t, 16> uuid) : VariableHeader(17, CONNECT), protocolVersion(protocolVersion), uuid(uuid)
    {
    }
    const std::string toString() override;
};

struct ConnackHeader : VariableHeader
{
    ConnackReturnCode returnCode;
    uint8_t networkID;
    const std::array<uint8_t, 16> uuid;

    ConnackHeader(ConnackReturnCode returnCode, uint8_t networkID, std::array<uint8_t, 16> uuid) : VariableHeader(18, CONNACK), returnCode(returnCode), networkID(networkID), uuid(uuid)
    {
    }
    const std::string toString() override;
};

struct PublishHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

    PublishHeader(uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(topicNameLength + 3, PUBLISH), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    const std::string toString() override;
};

struct PubackHeader : VariableHeader
{
    uint16_t packetID;

    PubackHeader(uint16_t packetID) : VariableHeader(2, PUBACK), packetID(packetID) {}
    const std::string toString() override;
};

struct SubscribeHeader : VariableHeader
{
    uint8_t topicNameLength;
    std::string topicName;
    uint16_t packetID;

    SubscribeHeader(uint8_t topicNameLength, std::string topicName, uint16_t packetID) : VariableHeader(topicNameLength + 3, SUBSCRIBE), topicNameLength(topicNameLength), topicName(topicName), packetID(packetID) {}
    const std::string toString() override;
};

struct SubackHeader : VariableHeader
{
    uint16_t packetID;

    SubackHeader(uint16_t packetID) : VariableHeader(2, SUBACK), packetID(packetID) {}
    const std::string toString() override;
};

struct DisconnectHeader : VariableHeader
{
    const std::array<uint8_t, 16> uuid;
    DisconnectHeader(std::array<uint8_t, 16> uuid) : VariableHeader(16, DISCONNECT), uuid(uuid)
    {
    }
    const std::string toString() override;
};

std::string controlPacketTypeToString(ControlPacketType ControlPacketType);
