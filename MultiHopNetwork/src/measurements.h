#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <cstdint>
#include <random>
#include "measurements.h"

class Measurements
{
public:
    Measurements();
    uint8_t getRandomBatteryLevel();

private:
    std::mt19937 gen;
};

#endif
