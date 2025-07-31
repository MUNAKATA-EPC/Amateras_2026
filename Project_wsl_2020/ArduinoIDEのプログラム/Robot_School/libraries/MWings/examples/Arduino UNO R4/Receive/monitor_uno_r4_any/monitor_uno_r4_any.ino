// Monitor example for TWELITE with Arduino UNO R4: Receive data from any TWELITE, without parsing

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720102;

void printByte(const uint8_t data);

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE with Arduino UNO R4: Any TWELITE, without parsing");
    Serial1.begin(115200);

    // Initialize TWELITE
    Twelite.begin(Serial1,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from any TWELITE
    Twelite.on([](const BarePacket& packet) {
        // Print timestamp
        static uint32_t lastTimestamp = 0;
        uint32_t currentTimestamp = millis();
        Serial.print("[Timestamp] ");
        Serial.print(currentTimestamp, DEC); Serial.print(" ms, +");
        Serial.print(currentTimestamp - lastTimestamp, DEC); Serial.println(" ms");
        lastTimestamp = currentTimestamp;
        // Print packet
        Serial.print(":");
        for (int i = 0; i < packet.u16PayloadSize; i++) {
            printByte(packet.u8Payload[i]);
        }
        printByte(packet.u8Checksum);
        Serial.println("[CR][LF]");
    });
}

void loop()
{
    // Update TWELITE
    Twelite.update();
}

void printByte(const uint8_t data)
{
    const uint8_t bit74 = ((data >> 4) & 0xF);
    const uint8_t bit30 = ((data >> 0) & 0xF);
    Serial.write((bit74 < 0xA) ? ('0' + bit74) : ('A' + bit74 - 0xA));
    Serial.write((bit30 < 0xA) ? ('0' + bit30) : ('A' + bit30 - 0xA));
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
