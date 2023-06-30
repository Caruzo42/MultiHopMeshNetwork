#include "display.h"

DisplayHandler::DisplayHandler() : display(DISPLAY_ADDRESS, DISPLAY_PORT_1, DISPLAY_PORT_2), qrcode()
{
}

void DisplayHandler::init()
{
    pinMode(16, OUTPUT);
    digitalWrite(16, LOW);
    delay(50);
    digitalWrite(16, HIGH);
    display.init();
}

void DisplayHandler::displayUUID(std::array<uint8_t, 16> &uuid)
{
    char uuidStr[37];
    snprintf(uuidStr, sizeof(uuidStr),
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7],
             uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, uuidStr);

    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
            {
                display.setPixel(x * 2, y * 2);
                display.setPixel(x * 2 + 1, y * 2);
                display.setPixel(x * 2, y * 2 + 1);
                display.setPixel(x * 2 + 1, y * 2 + 1);
            }
        }
    }
    display.display();
}

void DisplayHandler::displayInstructions(int timeLeft)
{
    display.setFont(ArialMT_Plain_10);
    display.drawString(display.width() / 2, 0, "UUID will be");
    display.drawString(display.width() / 2, 12, "saved in: ");

    display.setColor(BLACK);
    display.drawString(display.width() / 2, 24, String(timeLeft + 1) + "s");

    display.setColor(WHITE);
    display.drawString(display.width() / 2, 24, String(timeLeft) + "s");

    display.drawString(display.width() / 2, 46, "Skip: (PRG)");
    display.display();
    display.display();
}

void DisplayHandler::clearScreen()
{
    display.clear();
}

void DisplayHandler::displayMessage(bool received, uint8_t from, uint8_t to, Message &msg)
{
    display.clear();
    if (received)
    {
        display.drawString(0, 0, "Incoming: " + String(from));
    }
    else
    {
        display.drawString(0, 0, "Sending: " + String(to));
    }
    display.drawString(0, 14, "Type: " + String(controlPacketTypeToString(msg.header.controlPacketType).c_str()));
    if (msg.header.controlPacketType == PUBLISH)
    {
        printMultiLine(msg.payload.c_str(), 0, 30);
    }

    display.display();
}

void DisplayHandler::displayNodeId(uint8_t nodeId)
{
    String nodeIdStr = String(nodeId);
    int charWidth = 10;
    int width = nodeIdStr.length() * charWidth + 6;

    int startDrawPos = (128 - width) - 1;

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(startDrawPos + width / 2, 0, nodeIdStr);
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    display.drawRect(startDrawPos - 1, 0, width + 2, 18);

    display.display();
}

void DisplayHandler::printMultiLine(String text, int16_t x, int16_t y)
{
    int lineSpacing = 10;
    int lineCount = 0;

    while (text.length() > 0)
    {
        uint16_t charsDrawn = display.drawString(0, y + lineSpacing * lineCount, text);
        text = text.substring(charsDrawn);
        lineCount++;
    }
    display.display();
}

void DisplayHandler::displaySentControlPacket(ControlPacketType controlPacketType)
{
    display.drawLine(0, 48, display.width(), 48);
    display.drawString(0, 50, "Sent: " + String(controlPacketTypeToString(controlPacketType).c_str()));
    display.display();
}
