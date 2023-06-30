#pragma once

#include <cstdint>

constexpr auto GATEWAY_ADDRESS = 0;
constexpr auto INITIAL_NODE_ADDRESS = 254;

constexpr auto MAX_MESSAGE_SIZE = 244;
constexpr auto HEADER_SIZE = 2;

constexpr auto TIME_TILL_UUID_IS_SAVED = 180;

constexpr auto LLG_SCK = 5;
constexpr auto LLG_MISO = 19;
constexpr auto LLG_MOSI = 27;
constexpr auto LLG_CS = 18;
constexpr auto LLG_RST = 16;
constexpr auto LLG_DI0 = 26;
constexpr auto LLG_DI1 = 35;
constexpr auto LLG_DI2 = 34;

constexpr auto DISPLAY_ADDRESS = 0x3c;
constexpr auto DISPLAY_PORT_1 = 4;
constexpr auto DISPLAY_PORT_2 = 15;

constexpr auto DISPLAY_PADDING = 2;
constexpr auto BUTTON_PIN = 0;

constexpr auto CONNECT_TIMEOUT = 5000;
constexpr auto NODE_PUBLISH_INTERVAL = 5000;

constexpr auto MEASUREMENTS_TOPIC = "v1/backend/measurements";
constexpr auto UPDATE_PACKET_REQUEST_TOPIC = "v1/updates/missing";

// DEBUGGING:

constexpr auto HARDCODED_NETWORK_ID = 0;

constexpr bool REGENERATING_UUID_EACH_START = true;
constexpr bool USING_DEFAULT_UUID = false;
constexpr uint8_t HARDCODED_UUID[16] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};