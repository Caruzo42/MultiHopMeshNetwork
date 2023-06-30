#pragma once

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <Preferences.h>
#include <sstream>
#include <unordered_map>

#include "network.h"
#include "config.h"
#include "protocol.h"
#include "protocol_common.h"
#include "variable_headers.h"
#include "display.h"
#include "measurements.h"

void handle(Message &msg, uint8_t from);
void generateUUID(byte *uuid);
void handleButtonPress();
void handleUpdateMessage(UpdateBlock updateBlock);
bool checkIfPreviouslyReceived(uint16_t version, uint16_t blockIndex);
uint32_t makeKey(uint16_t version, uint16_t blockIndex);
void requestMissingPacket(uint16_t blockIndex);

void setup();
void loop();
