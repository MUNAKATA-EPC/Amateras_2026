// Commander example for TWELITE with Arduino UNO R4: Send manually-created 0x80 command to App_Twelite

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 18;
const uint32_t TWE_APP_ID = 0x67720102;
const uint8_t TWE_RETRY_COUNT = 2;
const uint8_t TWE_TX_POWER = 3;

const uint8_t TWE_TARGET_LID = 0x78;

constexpr int CommandDataSize = 11;
uint8_t CommandData[CommandDataSize] = {0x01, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void printCommand();
void printByte(const uint8_t data);
void printHelp();

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Commander example for TWELITE with Arduino UNO R4: App_Twelite, manually");
    Serial1.begin(115200);

    // Initialize TWELITE
    Twelite.begin(Serial1,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID,
                  TWE_RETRY_COUNT, TWE_TX_POWER);

    // Send initial App_Twelite command
    if (Twelite.send(TWE_TARGET_LID, 0x80, CommandData, CommandDataSize)) {
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
        case '1': {
            CommandData[1] ^= (1 << 0);
            Serial.print("Set DI1 to "); Serial.println((CommandData[1] & (1 << 0)) ? "Lo" : "Hi"); break;
        }
        case '2': {
            CommandData[1] ^= (1 << 1);
            Serial.print("Set DI2 to "); Serial.println((CommandData[1] & (1 << 1)) ? "Lo" : "Hi"); break;
        }
        case '3': {
            CommandData[1] ^= (1 << 2);
            Serial.print("Set DI3 to "); Serial.println((CommandData[1] & (1 << 2)) ? "Lo" : "Hi"); break;
        }
        case '4': {
            CommandData[1] ^= (1 << 3);
            Serial.print("Set DI4 to "); Serial.println((CommandData[1] & (1 << 3)) ? "Lo" : "Hi"); break;
        }
        case 'q': {
            CommandData[2] ^= (1 << 0);
            Serial.print((CommandData[1] & (1 << 0)) ? "Enabled" : "Disabled"); Serial.println(" DI1"); break;
        }
        case 'w': {
            CommandData[2] ^= (1 << 1);
            Serial.print((CommandData[1] & (1 << 1)) ? "Enabled" : "Disabled"); Serial.println(" DI2"); break;
        }
        case 'r': {
            CommandData[2] ^= (1 << 2);
            Serial.print((CommandData[1] & (1 << 2)) ? "Enabled" : "Disabled"); Serial.println(" DI3"); break;
        }
        case 't': {
            CommandData[2] ^= (1 << 3);
            Serial.print((CommandData[1] & (1 << 3)) ? "Enabled" : "Disabled"); Serial.println(" DI4"); break;
        }
        case 'a': {
            switch (pwmState[0]) {
            case 0: {
                CommandData[3] = 0; CommandData[4] = 0;
                pwmState[0]++;
                Serial.println("Set PWM1 to 0%"); break;
            }
            case 1: {
                const uint16_t duty = 1024 * 0.2;
                CommandData[3] = (duty >> 8) & 0xFF; CommandData[4] = (duty >> 0) & 0xFF;
                pwmState[0]++;
                Serial.println("Set PWM1 to 20%"); break;
            }
            case 2: {
                const uint16_t duty = 1024 * 0.4;
                CommandData[3] = (duty >> 8) & 0xFF; CommandData[4] = (duty >> 0) & 0xFF;
                pwmState[0]++;
                Serial.println("Set PWM1 to 40%"); break;
            }
            case 3: {
                const uint16_t duty = 1024 * 0.6;
                CommandData[3] = (duty >> 8) & 0xFF; CommandData[4] = (duty >> 0) & 0xFF;
                pwmState[0]++;
                Serial.println("Set PWM1 to 60%"); break;
            }
            case 4: {
                const uint16_t duty = 1024 * 0.8;
                CommandData[3] = (duty >> 8) & 0xFF; CommandData[4] = (duty >> 0) & 0xFF;
                pwmState[0]++;
                Serial.println("Set PWM1 to 80%"); break;
            }
            case 5: {
                const uint16_t duty = 1024;
                CommandData[3] = (duty >> 8) & 0xFF; CommandData[4] = (duty >> 0) & 0xFF;
                pwmState[0]++;
                Serial.println("Set PWM1 to 100%"); break;
            }
            case 6: {
                CommandData[3] = 0xFF; CommandData[4] = 0xFF;
                pwmState[0] = 0;
                Serial.println("Set PWM1 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 's': {
            switch (pwmState[1]) {
            case 0: {
                CommandData[5] = 0; CommandData[6] = 0;
                pwmState[1]++;
                Serial.println("Set PWM2 to 0%"); break;
            }
            case 1: {
                const uint16_t duty = 1024 * 0.2;
                CommandData[5] = (duty >> 8) & 0xFF; CommandData[6] = (duty >> 0) & 0xFF;
                pwmState[1]++;
                Serial.println("Set PWM2 to 20%"); break;
            }
            case 2: {
                const uint16_t duty = 1024 * 0.4;
                CommandData[5] = (duty >> 8) & 0xFF; CommandData[6] = (duty >> 0) & 0xFF;
                pwmState[1]++;
                Serial.println("Set PWM2 to 40%"); break;
            }
            case 3: {
                const uint16_t duty = 1024 * 0.6;
                CommandData[5] = (duty >> 8) & 0xFF; CommandData[6] = (duty >> 0) & 0xFF;
                pwmState[1]++;
                Serial.println("Set PWM2 to 60%"); break;
            }
            case 4: {
                const uint16_t duty = 1024 * 0.8;
                CommandData[5] = (duty >> 8) & 0xFF; CommandData[6] = (duty >> 0) & 0xFF;
                pwmState[1]++;
                Serial.println("Set PWM2 to 80%"); break;
            }
            case 5: {
                const uint16_t duty = 1024;
                CommandData[5] = (duty >> 8) & 0xFF; CommandData[6] = (duty >> 0) & 0xFF;
                pwmState[1]++;
                Serial.println("Set PWM2 to 100%"); break;
            }
            case 6: {
                CommandData[5] = 0xFF; CommandData[6] = 0xFF;
                pwmState[1] = 0;
                Serial.println("Set PWM2 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 'd': {
            switch (pwmState[2]) {
            case 0: {
                CommandData[7] = 0; CommandData[8] = 0;
                pwmState[2]++;
                Serial.println("Set PWM3 to 0%"); break;
            }
            case 1: {
                const uint16_t duty = 1024 * 0.2;
                CommandData[7] = (duty >> 8) & 0xFF; CommandData[8] = (duty >> 0) & 0xFF;
                pwmState[2]++;
                Serial.println("Set PWM3 to 20%"); break;
            }
            case 2: {
                const uint16_t duty = 1024 * 0.4;
                CommandData[7] = (duty >> 8) & 0xFF; CommandData[8] = (duty >> 0) & 0xFF;
                pwmState[2]++;
                Serial.println("Set PWM3 to 40%"); break;
            }
            case 3: {
                const uint16_t duty = 1024 * 0.6;
                CommandData[7] = (duty >> 8) & 0xFF; CommandData[8] = (duty >> 0) & 0xFF;
                pwmState[2]++;
                Serial.println("Set PWM3 to 60%"); break;
            }
            case 4: {
                const uint16_t duty = 1024 * 0.8;
                CommandData[7] = (duty >> 8) & 0xFF; CommandData[8] = (duty >> 0) & 0xFF;
                pwmState[2]++;
                Serial.println("Set PWM3 to 80%"); break;
            }
            case 5: {
                const uint16_t duty = 1024;
                CommandData[7] = (duty >> 8) & 0xFF; CommandData[8] = (duty >> 0) & 0xFF;
                pwmState[2]++;
                Serial.println("Set PWM3 to 100%"); break;
            }
            case 6: {
                CommandData[7] = 0xFF; CommandData[8] = 0xFF;
                pwmState[2] = 0;
                Serial.println("Set PWM3 to N/A"); break;
            }
            default: break;
            } break;
        }
        case 'f': {
            switch (pwmState[3]) {
            case 0: {
                CommandData[9] = 0; CommandData[10] = 0;
                pwmState[3]++;
                Serial.println("Set PWM4 to 0%"); break;
            }
            case 1: {
                const uint16_t duty = 1024 * 0.2;
                CommandData[9] = (duty >> 8) & 0xFF; CommandData[10] = (duty >> 0) & 0xFF;
                pwmState[3]++;
                Serial.println("Set PWM4 to 20%"); break;
            }
            case 2: {
                const uint16_t duty = 1024 * 0.4;
                CommandData[9] = (duty >> 8) & 0xFF; CommandData[10] = (duty >> 0) & 0xFF;
                pwmState[3]++;
                Serial.println("Set PWM4 to 40%"); break;
            }
            case 3: {
                const uint16_t duty = 1024 * 0.6;
                CommandData[9] = (duty >> 8) & 0xFF; CommandData[10] = (duty >> 0) & 0xFF;
                pwmState[3]++;
                Serial.println("Set PWM4 to 60%"); break;
            }
            case 4: {
                const uint16_t duty = 1024 * 0.8;
                CommandData[9] = (duty >> 8) & 0xFF; CommandData[10] = (duty >> 0) & 0xFF;
                pwmState[3]++;
                Serial.println("Set PWM4 to 80%"); break;
            }
            case 5: {
                const uint16_t duty = 1024;
                CommandData[9] = (duty >> 8) & 0xFF; CommandData[10] = (duty >> 0) & 0xFF;
                pwmState[3]++;
                Serial.println("Set PWM4 to 100%"); break;
            }
            case 6: {
                CommandData[9] = 0xFF; CommandData[10] = 0xFF;
                pwmState[3] = 0;
                Serial.println("Set PWM4 to N/A"); break;
            }
            default: break;
            } break;
        }
        case ' ': {
            if (Twelite.send(TWE_TARGET_LID, 0x80, CommandData, CommandDataSize)) {
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
    Serial.print(":");
    printByte(TWE_TARGET_LID);
    printByte(0x80);
    for (int i = 0; i < CommandDataSize; i++) {
        printByte(CommandData[i]);
    }
    Serial.println("[sum][CR][LF]");
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
