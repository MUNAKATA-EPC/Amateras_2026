/**
 * @file   AppTwelitePacketParser.cpp
 * @brief  App_Twelite packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppTwelitePacketParser.h"

apptwelite::PacketParser AppTwelitePacketParser;

bool apptwelite::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppTwelitePacket* const parsedAppTwelitePacket = static_cast<ParsedAppTwelitePacket*>(parsedPacket);

    parsedAppTwelitePacket->u32SourceSerialId = barePacket.u32At(5);
    parsedAppTwelitePacket->u8SourceLogicalId = barePacket.u8At(0);
    parsedAppTwelitePacket->u16SequenceNumber = barePacket.u16At(10); // Technically, it's timestamp
    parsedAppTwelitePacket->u8Lqi = barePacket.u8At(4);
    parsedAppTwelitePacket->u16SupplyVoltage = barePacket.u16At(13);

    parsedAppTwelitePacket->u8DestinationLogicalId = barePacket.u8At(9);
    parsedAppTwelitePacket->u8RelayCount = barePacket.u8At(12);
    const uint8_t u8DIState = barePacket.u8At(16);
    parsedAppTwelitePacket->bPeriodic = ((u8DIState & 0x80) == 0x80);
    for (int i = 0; i < 4; i++) {
        parsedAppTwelitePacket->bDiState[i] = (u8DIState & (1 << i));
    }
    const uint8_t u8DIChanged = barePacket.u8At(17);
    for (int i = 0; i < 4; i++) {
        parsedAppTwelitePacket->bDiChanged[i] = (u8DIChanged & (1 << i));
    }
    uint8_t u8AiValue[4];
    for (int i = 0; i < 4; i++) {
        u8AiValue[i] = barePacket.u8At(18+i);
    }
    const uint8_t u8AiFurtherValue = barePacket.u8At(22);
    for (int i = 0; i < 4; i++) {
        parsedAppTwelitePacket->u16AiVoltage[i] = (u8AiValue[i] * 4 + ((u8AiFurtherValue >> (i*2)) & 0x03)) * 4;
    }

    return true;
}
