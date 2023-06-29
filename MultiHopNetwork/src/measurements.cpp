#include "Measurements.h"

Measurements::Measurements()
    : gen(std::random_device()())
{
}

uint8_t Measurements::getRandomBatteryLevel()
{
    std::uniform_int_distribution<> distrib(0, 100);
    return static_cast<uint8_t>(distrib(gen));
}
