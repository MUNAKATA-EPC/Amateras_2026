// Commander example for TWELITE SPOT: Send command to App_Uart (Mode A)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720103;
const uint8_t TWE_RETRY_COUNT = 2;
const uint8_t TWE_TX_POWER = 3;

const uint8_t TWE_TARGET_LID = 0x78;

constexpr int DataSize = 2;
uint8_t DataToSend[DataSize];

AppUartAsciiCommand command;

void printData();
void printByte(const uint8_t data);
void printHelp();

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Commander example for TWELITE SPOT: App_Uart (Mode A)");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID,
                  TWE_RETRY_COUNT, TWE_TX_POWER);

    // Prepare initial App_Uart command
    DataToSend[0] = 0xBE;
    DataToSend[1] = 0xEF;
    command.u8DestinationLogicalId = TWE_TARGET_LID;
    command.u8CommandId = 0x01;
    command.u16DataSize = DataSize;
    command.u8Data = DataToSend;

    // Print help message
    printHelp();
}

void loop()
{
    // Update TWELITE
    Twelite.update();

    // Process input
    if (Serial.available()) {
        int c = Serial.read();
        switch (c) {
        case ' ': {
            if (Twelite.send(command, 100)) {       // Timeout: 100ms
                Serial.println("Sent data below:");
                printData();
            } break;
        }
        case 'S': {
            Serial.println("Current data are:");
            printData(); break;
        }
        case 'h': {
            printHelp(); break;
        }
        default: break;
        }
    }
}

void printData()
{
    for (int i = 0; i < command.u16DataSize; i++) {
        printByte(command.u8Data[i]);
    }
    Serial.println("");
}

void printByte(const uint8_t data)
{
    const uint8_t bit74 = ((data >> 4) & 0xF);
    const uint8_t bit30 = ((data >> 0) & 0xF);
    Serial.write((bit74 < 0xA) ? ('0' + bit74) : ('A' + bit74 - 0xA));
    Serial.write((bit30 < 0xA) ? ('0' + bit30) : ('A' + bit30 - 0xA));
}

void printHelp()
{
    Serial.println("Keys:");
    Serial.println("-----------------");
    Serial.println("Send: SPACE");
    Serial.println("Show: 'S'");
    Serial.println("Help: 'h'");
    Serial.println("");
}

/*
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
