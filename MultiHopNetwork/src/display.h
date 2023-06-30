#pragma once

#include <Arduino.h>
#include <SSD1306.h>
#include <qrcode.h>

#include "protocol_common.h"
#include "variable_headers.h"
#include "config.h"

class DisplayHandler
{
public:
    DisplayHandler();
    void displayUUID(std::array<uint8_t, 16> &uuid);
    void displayInstructions(int timeLeft);
    void clearScreen();
    void displayMessage(bool received, uint8_t from, uint8_t to, Message &msg);
    void displaySentControlPacket(ControlPacketType controlPacketType);
    void displayNodeId(uint8_t nodeId);
    void init();

private:
    SSD1306 display;
    QRCode qrcode;
    void printMultiLine(String text, int16_t x, int16_t y);
};