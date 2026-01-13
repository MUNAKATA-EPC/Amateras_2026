// Monitor example for TWELITE SPOT: Receive data from App_Twelite

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720102;

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE SPOT: App_Twelite");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_Twelite
    Twelite.on([](const ParsedAppTwelitePacket& packet) {
        Serial.println("");
        Serial.print("Packet Timestamp:  ");
        Serial.print(packet.u16SequenceNumber / 64.0f, 1); Serial.println(" sec");
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("LQI:               ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Supply Voltage:    ");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.println(" mV");
        Serial.print("Digital Input:    ");
        Serial.print(packet.bDiState[0] ? " DI1:Lo" : " DI1:Hi");
        Serial.print(packet.bDiState[1] ? " DI2:Lo" : " DI2:Hi");
        Serial.print(packet.bDiState[2] ? " DI3:Lo" : " DI3:Hi");
        Serial.println(packet.bDiState[3] ? " DI4:Lo" : " DI4:Hi");
        Serial.print("Analog Input:     ");
        Serial.print(" AI1:"); Serial.print(packet.u16AiVoltage[0]); Serial.print(" mV");
        Serial.print(" AI2:"); Serial.print(packet.u16AiVoltage[1]); Serial.print(" mV");
        Serial.print(" AI3:"); Serial.print(packet.u16AiVoltage[2]); Serial.print(" mV");
        Serial.print(" AI4:"); Serial.print(packet.u16AiVoltage[3]); Serial.println(" mV");
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
