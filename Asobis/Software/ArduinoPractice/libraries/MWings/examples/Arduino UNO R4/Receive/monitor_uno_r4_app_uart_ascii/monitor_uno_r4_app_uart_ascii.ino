// Monitor example for TWELITE with Arduino UNO R4: Receive data from App_Uart (Mode A)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720103;

void printByte(const uint8_t data);

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE with Arduino UNO R4: App_Uart (Mode A)");
    Serial1.begin(115200);

    // Initialize TWELITE
    Twelite.begin(Serial1,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID,
                  2, 3, 1300, 100);

    // Attach an event handler to process simplified packets from App_Uart
    Twelite.on([](const ParsedAppUartAsciiPacket& packet) {
        Serial.println("");
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("Command ID:        0x");
        Serial.println(packet.u8CommandId, HEX);
        Serial.print("Data size:         ");
        Serial.print(packet.u16DataSize, DEC); Serial.println(" bytes");
        Serial.print("Data content:      ");
        for (int i = 0; i < packet.u16DataSize; i++) {
            printByte(packet.u8Data[i]);
        }
        Serial.println("");
    });

    // Attach an event handler to process extended packets from App_Uart
    Twelite.on([](const ParsedAppUartAsciiExtendedPacket& packet) {
        Serial.println("");
        Serial.print("Source Logical ID:     0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("Source Serial ID:      0x");
        Serial.println(packet.u32SourceSerialId, HEX);
        Serial.print("Destination Serial ID: 0x");
        if (packet.u32DestinationSerialId == 0xFFFFFFFF) {
            Serial.println(" N/A");
        } else {
            Serial.println(packet.u32DestinationSerialId, HEX);
        }
        Serial.print("LQI:                   ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Command ID:            0x");
        Serial.println(packet.u8CommandId, HEX);
        Serial.print("Data size:             ");
        Serial.print(packet.u16DataSize, DEC); Serial.println(" bytes");
        Serial.print("Data content:          ");
        for (int i = 0; i < packet.u16DataSize; i++) {
            printByte(packet.u8Data[i]);
        }
        Serial.println("");
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
