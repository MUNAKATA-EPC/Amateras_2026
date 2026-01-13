// Commander example for TWELITE SPOT: Send 0x80 command to App_Twelite

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720102;
const uint8_t TWE_RETRY_COUNT = 2;
const uint8_t TWE_TX_POWER = 3;

const uint8_t TWE_TARGET_LID = 0x78;

AppTweliteCommand command;

void printCommand();
void printHelp();

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Commander example for TWELITE SPOT: App_Twelite");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID,
                  TWE_RETRY_COUNT, TWE_TX_POWER);

    // Prepare initial App_Twelite command
    command.u8DestinationLogicalId = TWE_TARGET_LID;
    for (int i = 0; i < 4; i++) {
        command.bDiToChange[i] = true;
        command.bDiState[i] = false;    // Set Hi
        command.bPwmToChange[i] = true;
        command.u16PwmDuty[i] = 0xFFFF; // Disable
    }

    // Send initial App_Twelite command
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
    // State for input
    static uint8_t pwmState[4] = {0, 0, 0, 0};

    // Update TWELITE
    Twelite.update();

    // Process input
    if (Serial.available()) {
        int c = Serial.read();
        switch (c) {
        case '1': { command.bDiState[0] = !(command.bDiState[0]);
            Serial.print("Set DI1 to "); Serial.println(command.bDiState[0] ? "Lo" : "Hi"); break;
        }
        case '2': {
            command.bDiState[1] = !(command.bDiState[1]);
            Serial.print("Set DI2 to "); Serial.println(command.bDiState[1] ? "Lo" : "Hi"); break;
        }
        case '3': {
            command.bDiState[2] = !(command.bDiState[2]);
            Serial.print("Set DI3 to "); Serial.println(command.bDiState[2] ? "Lo" : "Hi"); break;
        }
        case '4': {
            command.bDiState[3] = !(command.bDiState[3]);
            Serial.print("Set DI4 to "); Serial.println(command.bDiState[3] ? "Lo" : "Hi"); break;
        }
        case 'q': {
            command.bDiToChange[0] = !(command.bDiToChange[0]);
            Serial.print(command.bDiToChange[0] ? "Enabled" : "Disabled"); Serial.println(" DI1"); break;
        }
        case 'w': {
            command.bDiToChange[1] = !(command.bDiToChange[1]);
            Serial.print(command.bDiToChange[1] ? "Enabled" : "Disabled"); Serial.println(" DI2"); break;
        }
        case 'r': {
            command.bDiToChange[2] = !(command.bDiToChange[2]);
            Serial.print(command.bDiToChange[2] ? "Enabled" : "Disabled"); Serial.println(" DI3"); break;
        }
        case 't': {
            command.bDiToChange[3] = !(command.bDiToChange[3]);
            Serial.print(command.bDiToChange[3] ? "Enabled" : "Disabled"); Serial.println(" DI4"); break;
        }
        case 'a': {
            switch (pwmState[0]) {
            case 0: {
                command.u16PwmDuty[0] = 0; pwmState[0]++;
                Serial.println("Set PWM1 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[0] = 1024 * 0.2; pwmState[0]++;
                Serial.println("Set PWM1 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[0] = 1024 * 0.4; pwmState[0]++;
                Serial.println("Set PWM1 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[0] = 1024 * 0.6; pwmState[0]++;
                Serial.println("Set PWM1 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[0] = 1024 * 0.8; pwmState[0]++;
                Serial.println("Set PWM1 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[0] = 1024; pwmState[0]++;
                Serial.println("Set PWM1 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[0] = 0xFFFF; pwmState[0] = 0;
                Serial.println("Set PWM1 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 's': {
            switch (pwmState[1]) {
            case 0: {
                command.u16PwmDuty[1] = 0; pwmState[1]++;
                Serial.println("Set PWM2 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[1] = 1024 * 0.2; pwmState[1]++;
                Serial.println("Set PWM2 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[1] = 1024 * 0.4; pwmState[1]++;
                Serial.println("Set PWM2 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[1] = 1024 * 0.6; pwmState[1]++;
                Serial.println("Set PWM2 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[1] = 1024 * 0.8; pwmState[1]++;
                Serial.println("Set PWM2 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[1] = 1024 * 1.0; pwmState[1]++;
                Serial.println("Set PWM2 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[1] = 0xFFFF; pwmState[1] = 0;
                Serial.println("Set PWM2 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 'd': {
            switch (pwmState[2]) {
            case 0: {
                command.u16PwmDuty[2] = 0; pwmState[2]++;
                Serial.println("Set PWM3 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[2] = 1024 * 0.2; pwmState[2]++;
                Serial.println("Set PWM3 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[2] = 1024 * 0.4; pwmState[2]++;
                Serial.println("Set PWM3 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[2] = 1024 * 0.6; pwmState[2]++;
                Serial.println("Set PWM3 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[2] = 1024 * 0.8; pwmState[2]++;
                Serial.println("Set PWM3 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[2] = 1024 * 1.0; pwmState[2]++;
                Serial.println("Set PWM3 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[2] = 0xFFFF; pwmState[2] = 0;
                Serial.println("Set PWM3 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 'f': {
            switch (pwmState[3]) {
            case 0: {
                command.u16PwmDuty[3] = 0; pwmState[3]++;
                Serial.println("Set PWM4 to 0%"); break;
            }
            case 1: {
                command.u16PwmDuty[3] = 1024 * 0.2; pwmState[3]++;
                Serial.println("Set PWM4 to 20%"); break;
            }
            case 2: {
                command.u16PwmDuty[3] = 1024 * 0.4; pwmState[3]++;
                Serial.println("Set PWM4 to 40%"); break;
            }
            case 3: {
                command.u16PwmDuty[3] = 1024 * 0.6; pwmState[3]++;
                Serial.println("Set PWM4 to 60%"); break;
            }
            case 4: {
                command.u16PwmDuty[3] = 1024 * 0.8; pwmState[3]++;
                Serial.println("Set PWM4 to 80%"); break;
            }
            case 5: {
                command.u16PwmDuty[3] = 1024 * 1.0; pwmState[3]++;
                Serial.println("Set PWM4 to 100%"); break;
            }
            case 6: {
                command.u16PwmDuty[3] = 0xFFFF; pwmState[3] = 0;
                Serial.println("Set PWM4 to N/A"); break;
            }
            default: break;
            } break;
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
    Serial.print("DI to change: ");
    for (int i = 0; i < 4; i++) {
        Serial.print(" DI"); Serial.print(i+1, DEC); Serial.print(command.bDiToChange[i] ? ":S" : ":-");
    }
    Serial.println("");
    Serial.print("DI state:     ");
    for (int i = 0; i < 4; i++) {
        Serial.print(" DI"); Serial.print(i+1, DEC); Serial.print(command.bDiState[i] ? ":L" : ":H");
    }
    Serial.println("");
    Serial.print("PWM duty:     ");
    for (int i = 0; i < 4; i++) {
        Serial.print(" PWM"); Serial.print(i+1, DEC); Serial.print(":");
        if (command.u16PwmDuty[i] == 0xFFFF) {
            Serial.print("N/A");
        } else {
            Serial.print(command.u16PwmDuty[i], DEC); Serial.print("/1024");
        }
    }
    Serial.println("");
}

void printHelp()
{
    Serial.println("");
    Serial.println("Keys:");
    Serial.println("DI1:  '1' DI2:  '2' DI3:  '3' DI4:  '4'");
    Serial.println("SEL1: 'q' SEL2: 'w' SEL3: 'r' SEL4: 't'");
    Serial.println("PWM1: 'a' PWM2: 's' PWM3: 'd' PWM4: 'f'");
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
