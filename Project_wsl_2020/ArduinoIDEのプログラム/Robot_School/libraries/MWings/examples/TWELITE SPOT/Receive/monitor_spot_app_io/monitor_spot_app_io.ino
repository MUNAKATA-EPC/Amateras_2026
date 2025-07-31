// Monitor example for TWELITE SPOT: Receive data from App_IO

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 16;
const uint32_t TWE_APP_ID = 0x67720107;

const int TO_DIO[] = {12, 13, 11, 16, 18, 19, 4, 9, 14, 15, 5, 8}; // DIO table

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE SPOT: App_IO");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_IO
    Twelite.on([](const ParsedAppIoPacket& packet) {
        Serial.println("");
        Serial.print("Packet Timestamp:  ");
        Serial.print(packet.u16SequenceNumber / 64.0f, 1); Serial.println(" sec");
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("LQI:               ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Digital Input:    ");
        for (int i = 0; i < 12; i++) {
            if (packet.bDiValid[i]) {
                Serial.print(" DIO"); Serial.print(TO_DIO[i], DEC);
                Serial.print(packet.bDiState[i] ? ":Lo" : ":Hi");
            }
        }
    });
}

void loop()
{
    // Update TWELITE
    Twelite.update();
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
