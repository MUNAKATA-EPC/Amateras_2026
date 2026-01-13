// Monitor example for TWELITE SPOT: Receive data from App_PAL (MOTION) or App_CUE (MOTION PAL Mode)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = 5;
const int PRG_PIN = 4;
const int LED_PIN = 18;

const uint8_t TWE_CHANNEL = 15;
const uint32_t TWE_APP_ID = 0x67726305;

void setup()
{
    // Initialize serial ports
    Serial.begin(115200);
    Serial.println("Monitor example for TWELITE SPOT: App_PAL (MOTION) or App_CUE (MOTION PAL Mode)");
    Serial2.begin(115200, SERIAL_8N1);

    // Initialize TWELITE
    Twelite.begin(Serial2,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_PAL (MOT)
    Twelite.on([](const ParsedAppPalMotPacket& packet) {
        Serial.println("");
        Serial.print("Packet Number:     #");
        Serial.println(packet.u16SequenceNumber, DEC);
        Serial.print("Source Logical ID: 0x");
        Serial.println(packet.u8SourceLogicalId, HEX);
        Serial.print("LQI:               ");
        Serial.println(packet.u8Lqi, DEC);
        Serial.print("Supply Voltage:    ");
        Serial.print(packet.u16SupplyVoltage, DEC); Serial.println(" mV");
        int16_t xAverage = 0;
        for (int i = 0; i < packet.u8SampleCount; i++) {
            xAverage = (i * xAverage + packet.i16SamplesX[i]) / (i + 1);
        }
        Serial.print("Accel X (Average): ");
        Serial.print(xAverage, DEC); Serial.println(" mG");
        int16_t yAverage = 0;
        for (int i = 0; i < packet.u8SampleCount; i++) {
            yAverage = (i * yAverage + packet.i16SamplesY[i]) / (i + 1);
        }
        Serial.print("Accel Y (Average): ");
        Serial.print(yAverage, DEC); Serial.println(" mG");
        int16_t zAverage = 0;
        for (int i = 0; i < packet.u8SampleCount; i++) {
            zAverage = (i * zAverage + packet.i16SamplesZ[i]) / (i + 1);
        }
        Serial.print("Accel Z (Average): ");
        Serial.print(zAverage, DEC); Serial.println(" mG");
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
