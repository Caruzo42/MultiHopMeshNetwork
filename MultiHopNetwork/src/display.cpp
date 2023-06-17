#include "display.h"

DisplayHandler::DisplayHandler() : display(DISPLAY_ADDRESS, DISPLAY_PORT_1, DISPLAY_PORT_2), qrcode()
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
    display.drawString(display.width() / 2, 0, "Scan Code.");
    display.drawString(display.width() / 2, 20, "UUID will be");
    display.drawString(display.width() / 2, 32, "saved in: ");

    display.setColor(BLACK);
    display.drawString(display.width() / 2, 44, String(timeLeft + 1) + "s");

    display.setColor(WHITE);
    display.drawString(display.width() / 2, 44, String(timeLeft) + "s");
    display.display();
    display.display();
}

void DisplayHandler::clearScreen()
{
    display.clear();
}