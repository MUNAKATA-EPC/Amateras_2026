// Commander example for TWELITE with Arduino UNO R4: Send 0x90 detailed command to App_PAL (NOTICE)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 15;
const uint32_t TWE_APP_ID = 0x67726305;
const uint8_t TWE_RETRY_COUNT = 2;
const uint8_t TWE_TX_POWER = 3;

const uint8_t TWE_TARGET_LID = 0x01;

// Japanese traditional color: 鴇色
const AppPalNoticeRGBWColor TOKI_IRO(0x2, 0x1, 0x1, 0x1);

AppPalNoticeDetailedCommand command;

void printCommand();
void printHelp();

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Commander example for TWELITE with Arduino UNO R4: App_PAL (NOTICE), Detailed");
    Serial1.begin(115200);

    // Initialize TWELITE
    Twelite.begin(Serial1,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID,
                  TWE_RETRY_COUNT, TWE_TX_POWER);

    // Prepare initial App_PAL command
    command.u8DestinationLogicalId = TWE_TARGET_LID;
    command.sRGBWColor = TOKI_IRO;
    command.u8BlinkDutyPercentage = 50;
    command.fBlinkPeriodInSec = 1.0f;
    command.u16DurationInSec = 5;

    // Send initial App_PAL command
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
        case 'b': {
            if (command.u8BlinkDutyPercentage >= 100) {
                command.u8BlinkDutyPercentage = 0;
            } else {
                command.u8BlinkDutyPercentage += 10;
            }
            Serial.print("Set blink duty to ");
            Serial.print(command.u8BlinkDutyPercentage, DEC); Serial.println(" %"); break;
        }
        case 'p': {
            if (command.fBlinkPeriodInSec >= 5.0f) {
                command.fBlinkPeriodInSec = 0.0f;
            } else {
                command.fBlinkPeriodInSec += 0.5f;
            }
            Serial.print("Set blink period to ");
            Serial.print(command.fBlinkPeriodInSec, 2); Serial.println(" sec"); break;
        }
        case 'd': {
            switch(command.u16DurationInSec) {
            case 5: { command.u16DurationInSec = 10; break; }
            case 10: { command.u16DurationInSec = 20; break; }
            case 20: { command.u16DurationInSec = 30; break; }
            case 30: { command.u16DurationInSec = 45; break; }
            case 45: { command.u16DurationInSec = 60; break; }
            case 60: { command.u16DurationInSec = 90; break; }
            case 90: { command.u16DurationInSec = 120; break; }
            case 120: { command.u16DurationInSec = 300; break; }
            case 300: { command.u16DurationInSec = 5; break; }
            default: break;
            }
            Serial.print("Set duration to ");
            Serial.print(command.u16DurationInSec, DEC); Serial.println(" sec"); break;
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
    Serial.print("Color:        ");
    Serial.print("R"); Serial.print(command.sRGBWColor.red, DEC);
    Serial.print("G"); Serial.print(command.sRGBWColor.green, DEC);
    Serial.print("B"); Serial.print(command.sRGBWColor.blue, DEC);
    Serial.print("W"); Serial.println(command.sRGBWColor.white, DEC);
    Serial.print("Blink duty:   ");
    Serial.print(command.u8BlinkDutyPercentage, DEC); Serial.println(" %");
    Serial.print("Blink period: ");
    Serial.print(command.fBlinkPeriodInSec, 2); Serial.println(" sec");
    Serial.print("Duration:     ");
    Serial.print(command.u16DurationInSec, DEC); Serial.println(" sec");
    Serial.println("");
}

void printHelp()
{
    Serial.println("Keys:");
    Serial.println("Blink duty:   'b'");
    Serial.println("Blink period: 'p'");
    Serial.println("Duration:     'd'");
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
