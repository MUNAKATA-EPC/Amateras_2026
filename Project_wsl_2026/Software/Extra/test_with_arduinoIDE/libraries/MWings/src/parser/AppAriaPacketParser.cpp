/**
 * @file   AppAriaPacketParser.cpp
 * @brief  App_ARIA (ARIA mode) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppAriaPacketParser.h"

apparia::PacketParser AppAriaPacketParser;

bool apparia::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppAriaPacket* const parsedAppAriaPacket = static_cast<ParsedAppAriaPacket*>(parsedPacket);

    parsedAppAriaPacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAppAriaPacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAppAriaPacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAppAriaPacket->u8Lqi = barePacket.u8At(4);
    parsedAppAriaPacket->u16SupplyVoltage = barePacket.u16At(34);

    parsedAppAriaPacket->i16Temp100x = barePacket.i16At(51);
    parsedAppAriaPacket->u16Humid100x = barePacket.u16At(57);

    const uint8_t rawMagnetState = barePacket.u8At(46);
    parsedAppAriaPacket->u8MagnetState = rawMagnetState & 0x0F;
    parsedAppAriaPacket->bMagnetStateChanged = (rawMagnetState & 0x80) ? false : true;

    return true;
}
