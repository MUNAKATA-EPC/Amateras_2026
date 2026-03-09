// Arduino plotter example for TWELITE with Arduino UNO R4: Receive data from App_PAL (AMBIENT)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 15;
const uint32_t TWE_APP_ID = 0x67726305;

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Plotter example for TWELITE with Arduino UNO R4: App_PAL (AMBIENT)");
    Serial1.begin(115200);

    // Initialize TWELITE
    Twelite.begin(Serial1,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_PAL (AMB)
    Twelite.on([](const ParsedAppPalAmbPacket& packet) {
        // Plot only the first device
        static uint32_t firstSourceSerialId = packet.u32SourceSerialId;
        if (not (packet.u32SourceSerialId == firstSourceSerialId)) { return; }
        // Print
        Serial.print("Temp(x100):");
        Serial.print(packet.i16Temp100x, DEC); Serial.print(",");
        Serial.print("Humid(x100):");
        Serial.print(packet.u16Humid100x, DEC); Serial.print(",");
        Serial.print("VCC:");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.print(",");
        Serial.print("LQI(x10):");
        Serial.print(packet.u8Lqi * 10, DEC); Serial.println("");
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
