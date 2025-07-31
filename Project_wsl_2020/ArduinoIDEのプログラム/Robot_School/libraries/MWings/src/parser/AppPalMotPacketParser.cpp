/**
 * @file   AppPalMotPacketParser.cpp
 * @brief  App_PAL (MOT) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppPalMotPacketParser.h"

apppalmot::PacketParser AppPalMotPacketParser;

bool apppalmot::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppPalMotPacket* const parsedAppPalMotPacket = static_cast<ParsedAppPalMotPacket*>(parsedPacket);

    parsedAppPalMotPacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAppPalMotPacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAppPalMotPacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAppPalMotPacket->u8Lqi = barePacket.u8At(4);
    parsedAppPalMotPacket->u16SupplyVoltage = barePacket.u16At(19);

    parsedAppPalMotPacket->u16Ai1Voltage = barePacket.u16At(25);

    const uint8_t extensionByte = barePacket.u8At(29);
    const uint16_t frequencies[8] = {25, 50, 100, 190, 380, 750, 1100, 1300};
    parsedAppPalMotPacket->u16SamplingFrequency = frequencies[(extensionByte >> 5)];
    for (int index = 0; index < 16; index++) {
        parsedAppPalMotPacket->i16SamplesX[index] = barePacket.i16At(31 + (10 * index) + 0);
        parsedAppPalMotPacket->i16SamplesY[index] = barePacket.i16At(31 + (10 * index) + 2);
        parsedAppPalMotPacket->i16SamplesZ[index] = barePacket.i16At(31 + (10 * index) + 4);
    }
    parsedAppPalMotPacket->u8SampleCount = 16;

    return true;
}
