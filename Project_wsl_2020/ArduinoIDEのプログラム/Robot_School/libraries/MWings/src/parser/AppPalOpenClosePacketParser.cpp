/**
 * @file   AppPalOpenClosePacketParser.cpp
 * @brief  App_PAL (OPENCLOSE) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppPalOpenClosePacketParser.h"

apppalopenclose::PacketParser AppPalOpenClosePacketParser;

bool apppalopenclose::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppPalOpenClosePacket* const parsedAppPalOpenClosePacket = static_cast<ParsedAppPalOpenClosePacket*>(parsedPacket);

    parsedAppPalOpenClosePacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAppPalOpenClosePacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAppPalOpenClosePacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAppPalOpenClosePacket->u8Lqi = barePacket.u8At(4);
    parsedAppPalOpenClosePacket->u16SupplyVoltage = barePacket.u16At(19);

    parsedAppPalOpenClosePacket->u16Ai1Voltage = barePacket.u16At(25);

    const uint8_t rawMagnetState = barePacket.u8At(31);
    parsedAppPalOpenClosePacket->u8MagnetState = rawMagnetState & 0x0F;
    parsedAppPalOpenClosePacket->bMagnetStateChanged = (rawMagnetState & 0x80) ? false : true;

    return true;
}
