// Monitor example for TWELITE SPOT: Receive data from App_PAL (OPENCLOSE) or App_CUE/ARIA (OPENCLOSE PAL Mode)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 15;
const uint32_t TWE_APP_ID = 0x67726305;

void printMagnetState(const uint8_t state, const bool changed);

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE SPOT: App_PAL (OPENCLOSE) or App_CUE/ARIA (OPENCLOSE PAL Mode)");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_PAL (OPENCLOSE)
    Twelite.on([](const ParsedAppPalOpenClosePacket& packet) {
        Serial.println("");
        Serial.print("Packet Number:     #");
        Serial.println(packet.u16SequenceNumber, DEC);
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("LQI:               ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Supply Voltage:    ");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.println("mV");
        Serial.print("Magnet State:      ");
        printMagnetState(packet.u8MagnetState, packet.bMagnetStateChanged);
    });
}

void loop()
{
    // Update TWELITE
    Twelite.update();
}

void printMagnetState(const uint8_t state, const bool changed)
{
    if (changed) {
        switch (state) {
        case 0x0: { Serial.print("Leaving or Not found"); break; }
        case 0x1: { Serial.print("N-pole is getting closer"); break; }
        case 0x2: { Serial.print("S-pole is getting closer"); break; }
        default: break;
        }
    } else {
        switch (state) {
        case 0x0: { Serial.print("Not found"); break; }
        case 0x1: { Serial.print("N-pole is close"); break; }
        case 0x2: { Serial.print("S-pole is close"); break; }
        default: break;
        }
        Serial.print(" (Periodic packet)");
    }
    Serial.println("");
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
