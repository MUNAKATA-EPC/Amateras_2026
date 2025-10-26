// Arduino plotter example for TWELITE with Arduino UNO R4: Receive data from App_PAL (MOTION) or App_CUE (MOTION PAL Mode)

#include <Arduino.h>
#include "MWings.h"

const int RST_PIN = D11;
const int PRG_PIN = D12;
const int LED_PIN = D13;        // Use on-board LED as indicator

const uint8_t TWE_CHANNEL = 15;
const uint32_t TWE_APP_ID = 0x67726305;

const int CUTOFF_FREQ = 4;                              // LPF cutoff frequency
const float Tau = 1.0f / (2.0f * 3.14f * CUTOFF_FREQ);  // LPF time constant

void setup()
{
    // Initialize serial ports
    while (!Serial && millis() < 5000); // Wait for internal USB-UART
    Serial.begin(115200);
    Serial.println("Plotter example for TWELITE with Arduino UNO R4: App_PAL (MOTION) or App_CUE (MOTION PAL Mode)");
    Serial1.begin(115200);

    // Initialize TWELITE
    Twelite.begin(Serial1,
                  LED_PIN, RST_PIN, PRG_PIN,
                  TWE_CHANNEL, TWE_APP_ID);

    // Attach an event handler to process packets from App_PAL (MOT)
    Twelite.on([](const ParsedAppPalMotPacket& packet) {
        // Plot only the first device
        static uint32_t firstSourceSerialId = packet.u32SourceSerialId;
        if (not (packet.u32SourceSerialId == firstSourceSerialId)) { return; }
        // LPF (Digital RC Filter)
        static int16_t prevPacketFilteredLatestSampleX = 0;
        static int16_t prevPacketFilteredLatestSampleY = 0;
        static int16_t prevPacketFilteredLatestSampleZ = 0;
        const float a = packet.u16SamplingFrequency * Tau / (1.0f + packet.u16SamplingFrequency * Tau); // LPF coefficient
        int16_t filteredSamplesX[16];
        int16_t filteredSamplesY[16];
        int16_t filteredSamplesZ[16];
        filteredSamplesX[0] = a * prevPacketFilteredLatestSampleX + (1 - a) * packet.i16SamplesX[0];
        filteredSamplesY[0] = a * prevPacketFilteredLatestSampleY + (1 - a) * packet.i16SamplesY[0];
        filteredSamplesZ[0] = a * prevPacketFilteredLatestSampleZ + (1 - a) * packet.i16SamplesZ[0];
        for (int i = 1; i < packet.u8SampleCount; i++) {
            filteredSamplesX[i] = a * filteredSamplesX[i-1] + (1 - a) * packet.i16SamplesX[i];
            filteredSamplesY[i] = a * filteredSamplesY[i-1] + (1 - a) * packet.i16SamplesY[i];
            filteredSamplesZ[i] = a * filteredSamplesZ[i-1] + (1 - a) * packet.i16SamplesZ[i];
        }
        prevPacketFilteredLatestSampleX = filteredSamplesX[15];
        prevPacketFilteredLatestSampleY = filteredSamplesY[15];
        prevPacketFilteredLatestSampleZ = filteredSamplesZ[15];
        // Print to plot values
        for (int i = 0; i < packet.u8SampleCount; i++) {
            Serial.print("Xf:"); // Filtered X
            Serial.print(filteredSamplesX[i], DEC); Serial.print(",");
            Serial.print("Yf:"); // Filtered Y
            Serial.print(filteredSamplesY[i], DEC); Serial.print(",");
            Serial.print("Zf:"); // Filtered Z
            Serial.print(filteredSamplesZ[i], DEC); Serial.print(",");
            Serial.print("X:");  // Raw X
            Serial.print(packet.i16SamplesX[i], DEC); Serial.print(",");
            Serial.print("Y:");  // Raw Y
            Serial.print(packet.i16SamplesY[i], DEC); Serial.print(",");
            Serial.print("Z:");  // Raw Z
            Serial.print(packet.i16SamplesZ[i], DEC); Serial.println("");
        }
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
