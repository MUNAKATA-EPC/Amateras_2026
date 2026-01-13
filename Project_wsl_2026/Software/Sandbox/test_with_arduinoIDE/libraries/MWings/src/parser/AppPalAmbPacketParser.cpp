/**
 * @file   AppPalAmbPacketParser.cpp
 * @brief  App_PAL (AMB) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppPalAmbPacketParser.h"

apppalamb::PacketParser AppPalAmbPacketParser;

bool apppalamb::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppPalAmbPacket* const parsedAppPalAmbPacket = static_cast<ParsedAppPalAmbPacket*>(parsedPacket);

    parsedAppPalAmbPacket->u32SourceSerialId = barePacket.u32At(7);
    parsedAppPalAmbPacket->u8SourceLogicalId = barePacket.u8At(11);
    parsedAppPalAmbPacket->u16SequenceNumber = barePacket.u16At(5);
    parsedAppPalAmbPacket->u8Lqi = barePacket.u8At(4);
    parsedAppPalAmbPacket->u16SupplyVoltage = barePacket.u16At(19);

    parsedAppPalAmbPacket->u16Ai1Voltage = barePacket.u16At(25);

    parsedAppPalAmbPacket->i16Temp100x = barePacket.i16At(31);
    parsedAppPalAmbPacket->u16Humid100x = barePacket.u16At(37);
    parsedAppPalAmbPacket->u32Illuminance = barePacket.u32At(43);

    return true;
}
