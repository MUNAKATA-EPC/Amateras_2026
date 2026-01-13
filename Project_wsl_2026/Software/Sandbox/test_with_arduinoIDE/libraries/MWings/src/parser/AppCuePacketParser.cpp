/**
 * @file   AppCuePacketParser.cpp
 * @brief  App_CUE (CUE mode) parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppCuePacketParser.h"

appcue::PacketParser AppCuePacketParser;

bool appcue::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppCuePacket* const parsedAppCuePacket = static_cast<ParsedAppCuePacket*>(parsedPacket);

    parsedAppCuePacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAppCuePacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAppCuePacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAppCuePacket->u8Lqi = barePacket.u8At(4);
    parsedAppCuePacket->u16SupplyVoltage = barePacket.u16At(34);

    const uint8_t accelEventSource = barePacket.u8At(24);
    const uint8_t accelEventId = barePacket.u8At(26);
    if (accelEventSource == 0x04) {
        parsedAppCuePacket->bHasAccelEvent = true;
        parsedAppCuePacket->u8AccelEvent = accelEventId;
    } else {
        parsedAppCuePacket->bHasAccelEvent = false;
        parsedAppCuePacket->u8AccelEvent = 0xFF;
    }

    for (int index = 0; index < 10; index++) {
        parsedAppCuePacket->i16SamplesX[index] = barePacket.i16At(51 + (10 * index) + 0);
        parsedAppCuePacket->i16SamplesY[index] = barePacket.i16At(51 + (10 * index) + 2);
        parsedAppCuePacket->i16SamplesZ[index] = barePacket.i16At(51 + (10 * index) + 4);
    }
    parsedAppCuePacket->u8SampleCount = 10;

    const uint8_t rawMagnetState = barePacket.u8At(46);
    parsedAppCuePacket->u8MagnetState = rawMagnetState & 0x0F;
    parsedAppCuePacket->bMagnetStateChanged = (rawMagnetState & 0x80) ? false : true;

    return true;
}
