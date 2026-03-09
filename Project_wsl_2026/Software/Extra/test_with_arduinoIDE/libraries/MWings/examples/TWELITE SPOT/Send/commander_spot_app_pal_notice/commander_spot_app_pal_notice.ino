// Commander example for TWELITE SPOT: Send 0x90 command to App_PAL (NOTICE)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 15;
const uint32_t TWE_APP_ID = 0x67726305;
const uint8_t TWE_RETRY_COUNT = 2;
const uint8_t TWE_TX_POWER = 3;

const uint8_t TWE_TARGET_LID = 0x01;

AppPalNoticeCommand command;

void printCommand();
void printHelp();

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Commander example for TWELITE SPOT: App_PAL (NOTICE)");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID,
                  TWE_RETRY_COUNT, TWE_TX_POWER);

    // Prepare initial App_PAL command
    command.u8DestinationLogicalId = TWE_TARGET_LID;
    command.eColor = AppPalNoticeColor::WHITE;
    command.eBlinkSpeed = AppPalNoticeBlinkSpeed::MEDIUM;
    command.u8Brightness = 0x8;
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
        case 'c': {
            switch(command.eColor) {
            case AppPalNoticeColor::RED: {
                command.eColor = AppPalNoticeColor::GREEN;
                Serial.println("Set color to Green"); break;
            }
            case AppPalNoticeColor::GREEN: {
                command.eColor = AppPalNoticeColor::BLUE;
                Serial.println("Set color to Blue"); break;
            }
            case AppPalNoticeColor::BLUE: {
                command.eColor = AppPalNoticeColor::YELLOW;
                Serial.println("Set color to Yellow"); break;
            }
            case AppPalNoticeColor::YELLOW: {
                command.eColor = AppPalNoticeColor::PURPLE;
                Serial.println("Set color to Purple"); break;
            }
            case AppPalNoticeColor::PURPLE: {
                command.eColor = AppPalNoticeColor::LIGHT_BLUE;
                Serial.println("Set color to Light Blue"); break;
            }
            case AppPalNoticeColor::LIGHT_BLUE: {
                command.eColor = AppPalNoticeColor::WHITE;
                Serial.println("Set color to White"); break;
            }
            case AppPalNoticeColor::WHITE: {
                command.eColor = AppPalNoticeColor::WARM_WHITE;
                Serial.println("Set color to Warm White"); break;
            }
            case AppPalNoticeColor::WARM_WHITE: {
                command.eColor = AppPalNoticeColor::RED;
                Serial.println("Set color to Red"); break;
            }
            default: break;
            } break;
        }
        case 'b': {
            if (command.u8Brightness >= 15) {
                command.u8Brightness = 0;
            } else {
                command.u8Brightness += 1;
            }
            Serial.print("Set brightness to ");
            Serial.println(command.u8Brightness, DEC); break;
        }
        case 'p': {
            switch(command.eBlinkSpeed) {
            case AppPalNoticeBlinkSpeed::ALWAYS_ON: {
                command.eBlinkSpeed = AppPalNoticeBlinkSpeed::SLOW;
                Serial.println("Set blink speed to Slow"); break;
            }
            case AppPalNoticeBlinkSpeed::SLOW: {
                command.eBlinkSpeed = AppPalNoticeBlinkSpeed::MEDIUM;
                Serial.println("Set blink speed to Medium"); break;
            }
            case AppPalNoticeBlinkSpeed::MEDIUM: {
                command.eBlinkSpeed = AppPalNoticeBlinkSpeed::FAST;
                Serial.println("Set blink speed to Fast"); break;
            }
            case AppPalNoticeBlinkSpeed::FAST: {
                command.eBlinkSpeed = AppPalNoticeBlinkSpeed::ALWAYS_ON;
                Serial.println("Set not to blink"); break;
            }
            default: break;
            } break;
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
    Serial.print("Color:      ");
    switch (command.eColor) {
    case AppPalNoticeColor::RED: { Serial.println("Red"); break; }
    case AppPalNoticeColor::GREEN: { Serial.println("Green"); break; }
    case AppPalNoticeColor::BLUE: { Serial.println("Blue"); break; }
    case AppPalNoticeColor::YELLOW: { Serial.println("Yellow"); break; }
    case AppPalNoticeColor::PURPLE: { Serial.println("Purple"); break; }
    case AppPalNoticeColor::LIGHT_BLUE: { Serial.println("Light Blue"); break; }
    case AppPalNoticeColor::WHITE: { Serial.println("White"); break; }
    case AppPalNoticeColor::WARM_WHITE: { Serial.println("Warm White"); break; }
    default: { Serial.println("Unknown"); break; }
    }
    Serial.print("Brightness: ");
    Serial.print(command.u8Brightness, DEC); Serial.println("/15");
    Serial.print("Blinking:   ");
    switch (command.eBlinkSpeed) {
    case AppPalNoticeBlinkSpeed::ALWAYS_ON: { Serial.println("No blinking"); break; }
    case AppPalNoticeBlinkSpeed::SLOW: { Serial.println("Slow"); break; }
    case AppPalNoticeBlinkSpeed::MEDIUM: { Serial.println("Medium"); break; }
    case AppPalNoticeBlinkSpeed::FAST: { Serial.println("Fast"); break; }
    default: { Serial.println("Unknown"); break; }
    }
    Serial.print("Duration:   ");
    Serial.print(command.u16DurationInSec, DEC); Serial.println(" sec");
    Serial.println("");
}

void printHelp()
{
    Serial.println("Keys:");
    Serial.println("Color:      'c'");
    Serial.println("Brightness: 'b'");
    Serial.println("Blinking:   'p'");
    Serial.println("Duration:   'd'");
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
