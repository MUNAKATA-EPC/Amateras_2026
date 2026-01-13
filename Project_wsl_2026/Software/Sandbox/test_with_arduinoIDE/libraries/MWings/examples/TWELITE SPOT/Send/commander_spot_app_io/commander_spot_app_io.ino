// Commander example for TWELITE SPOT: Send 0x80 command to App_IO

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 16;
const uint32_t TWE_APP_ID = 0x67720107;
const uint8_t TWE_RETRY_COUNT = 2;
const uint8_t TWE_TX_POWER = 3;

const uint8_t TWE_TARGET_LID = 0x78;

AppIoCommand command;

void printCommand();
void printHelp();

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Commander example for TWELITE SPOT: App_IO");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID,
                  TWE_RETRY_COUNT, TWE_TX_POWER);

    // Prepare initial App_Io command
    command.u8DestinationLogicalId = TWE_TARGET_LID;
    for (int i = 0; i < 12; i++) {
        command.bDiToChange[i] = true;
        command.bDiState[i] = false;    // Set Hi
    }

    // Send initial App_Io command
    if (Twelite.send(command)) {
        Serial.println("");
        Serial.println("Sent command below:");
        printCommand();
    }

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
        case '1': { command.bDiState[0] = !(command.bDiState[0]);
            Serial.print("Set I1 to "); Serial.println(command.bDiState[0] ? "Lo" : "Hi"); break;
        }
        case '2': {
            command.bDiState[1] = !(command.bDiState[1]);
            Serial.print("Set I2 to "); Serial.println(command.bDiState[1] ? "Lo" : "Hi"); break;
        }
        case '3': {
            command.bDiState[2] = !(command.bDiState[2]);
            Serial.print("Set I3 to "); Serial.println(command.bDiState[2] ? "Lo" : "Hi"); break;
        }
        case '4': {
            command.bDiState[3] = !(command.bDiState[3]);
            Serial.print("Set I4 to "); Serial.println(command.bDiState[3] ? "Lo" : "Hi"); break;
        }
        case '5': {
            command.bDiState[4] = !(command.bDiState[4]);
            Serial.print("Set I5 to "); Serial.println(command.bDiState[4] ? "Lo" : "Hi"); break;
        }
        case '6': {
            command.bDiState[5] = !(command.bDiState[5]);
            Serial.print("Set I6 to "); Serial.println(command.bDiState[5] ? "Lo" : "Hi"); break;
        }
        case '7': {
            command.bDiState[6] = !(command.bDiState[6]);
            Serial.print("Set I7 to "); Serial.println(command.bDiState[6] ? "Lo" : "Hi"); break;
        }
        case '8': {
            command.bDiState[7] = !(command.bDiState[7]);
            Serial.print("Set I8 to "); Serial.println(command.bDiState[7] ? "Lo" : "Hi"); break;
        }
        case '9': {
            command.bDiState[8] = !(command.bDiState[8]);
            Serial.print("Set I9 to "); Serial.println(command.bDiState[8] ? "Lo" : "Hi"); break;
        }
        case 'A': {
            command.bDiState[9] = !(command.bDiState[9]);
            Serial.print("Set I10 to "); Serial.println(command.bDiState[9] ? "Lo" : "Hi"); break;
        }
        case 'B': {
            command.bDiState[10] = !(command.bDiState[10]);
            Serial.print("Set I11 to "); Serial.println(command.bDiState[10] ? "Lo" : "Hi"); break;
        }
        case 'C': {
            command.bDiState[11] = !(command.bDiState[11]);
            Serial.print("Set I12 to "); Serial.println(command.bDiState[11] ? "Lo" : "Hi"); break;
        }
        case ' ': {
            if (Twelite.send(command)) {
                Serial.println("Sent command below:");
                printCommand();
            } break;
        }
        case 'S': {
            Serial.println("Current command contents are:");
            printCommand(); break;
        }
        case 'h': {
            printHelp(); break;
        }
        default: break;
        }
    }
}

void printCommand()
{
    Serial.print("Input to change: ");
    for (int i = 0; i < 12; i++) {
        Serial.print(" I"); Serial.print(i+1, DEC); Serial.print(command.bDiToChange[i] ? ":S" : ":-");
    }
    Serial.println("");
    Serial.print("Input state:     ");
    for (int i = 0; i < 12; i++) {
        Serial.print(" I"); Serial.print(i+1, DEC); Serial.print(command.bDiState[i] ? ":L" : ":H");
    }
    Serial.println("");
}

void printHelp()
{
    Serial.println("");
    Serial.println("Keys:");
    Serial.println("I1:  '1' I2:  '2' I3:  '3' I4:  '4'");
    Serial.println("I5:  '5' I6:  '6' I7:  '7' I8:  '8'");
    Serial.println("I9:  '9' I10: 'A' I11: 'B' I12: 'C'");
    Serial.println("-----------------");
    Serial.println("Send: SPACE");
    Serial.println("Show: 'S'");
    Serial.println("Help: 'h'");
    Serial.println("");
}

/*
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */
