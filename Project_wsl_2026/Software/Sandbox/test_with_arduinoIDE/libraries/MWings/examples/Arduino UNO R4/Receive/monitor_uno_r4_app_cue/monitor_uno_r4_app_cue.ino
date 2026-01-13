// Monitor example for TWELITE with Arduino UNO R4: Receive data from App_CUE (CUE Mode)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720102;

void printAccelEvent(const uint8_t event);
void printMagnetState(const uint8_t state, const bool changed);

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE with Arduino UNO R4: App_CUE (CUE Mode)");
    Serial1.begin(115200);

    // Initialize TWELITE
    Twelite.begin(Serial1,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_CUE
    Twelite.on([](const ParsedAppCuePacket& packet) {
        Serial.println("");
        Serial.print("Packet Number:     #");
        Serial.println(packet.u16SequenceNumber, DEC);
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("LQI:               ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Supply Voltage:    ");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.println(" mV");
        Serial.print("Accel Event:       ");
        printAccelEvent(packet.u8AccelEvent);
        Serial.print("Accel X Axis [0]:  ");
        Serial.print(packet.i16SamplesX[0], DEC); Serial.println(" mG");
        Serial.print("Accel Y Axis [0]:  ");
        Serial.print(packet.i16SamplesY[0], DEC); Serial.println(" mG");
        Serial.print("Accel Z Axis [0]:  ");
        Serial.print(packet.i16SamplesZ[0], DEC); Serial.println(" mG");
        Serial.print("Magnet State:      ");
        printMagnetState(packet.u8MagnetState, packet.bMagnetStateChanged);
    });
}

void loop()
{
    // Update TWELITE
    Twelite.update();
}

void printAccelEvent(const uint8_t event)
{
    switch (event) {
    case 0x01: { Serial.print("Dice (1)"); break; }
    case 0x02: { Serial.print("Dice (2)"); break; }
    case 0x03: { Serial.print("Dice (3)"); break; }
    case 0x04: { Serial.print("Dice (4)"); break; }
    case 0x05: { Serial.print("Dice (5)"); break; }
    case 0x06: { Serial.print("Dice (6)"); break; }
    case 0x08: { Serial.print("Shake"); break; }
    case 0x10: { Serial.print("Move"); break; }
    default: break;
    }
    Serial.println("");
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
