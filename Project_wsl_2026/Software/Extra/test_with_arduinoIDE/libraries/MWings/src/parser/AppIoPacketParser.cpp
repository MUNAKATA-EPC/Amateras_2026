/**
 * @file   AppIoPacketParser.cpp
 * @brief  App_IO packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppIoPacketParser.h"

appio::PacketParser AppIoPacketParser;

bool appio::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppIoPacket* const parsedAppIoPacket = static_cast<ParsedAppIoPacket*>(parsedPacket);

    parsedAppIoPacket->u32SourceSerialId = barePacket.u32At(5);
    parsedAppIoPacket->u8SourceLogicalId = barePacket.u8At(9);
    parsedAppIoPacket->u16SequenceNumber = barePacket.u16At(10); // Technically, it's timestamp
    parsedAppIoPacket->u8Lqi = barePacket.u8At(4);
    parsedAppIoPacket->u16SupplyVoltage = 0; // There's no ADC

    parsedAppIoPacket->u8RelayCount = barePacket.u8At(12);
    const uint16_t u16DiState = barePacket.u16At(13);
    parsedAppIoPacket->bDiState[0] = (u16DiState & (1 << 0));
    parsedAppIoPacket->bDiState[1] = (u16DiState & (1 << 1));
    parsedAppIoPacket->bDiState[2] = (u16DiState & (1 << 2));
    parsedAppIoPacket->bDiState[3] = (u16DiState & (1 << 3));
    parsedAppIoPacket->bDiState[4] = (u16DiState & (1 << 4));
    parsedAppIoPacket->bDiState[5] = (u16DiState & (1 << 5));
    parsedAppIoPacket->bDiState[6] = (u16DiState & (1 << 6));
    parsedAppIoPacket->bDiState[7] = (u16DiState & (1 << 7));
    parsedAppIoPacket->bDiState[8] = (u16DiState & (1 << 8));
    parsedAppIoPacket->bDiState[9] = (u16DiState & (1 << 9));
    parsedAppIoPacket->bDiState[10] = (u16DiState & (1 << 10));
    parsedAppIoPacket->bDiState[11] = (u16DiState & (1 << 11));
    const uint16_t u16DiValid = barePacket.u16At(15);
    parsedAppIoPacket->bDiValid[0] = (u16DiValid & (1 << 0));
    parsedAppIoPacket->bDiValid[1] = (u16DiValid & (1 << 1));
    parsedAppIoPacket->bDiValid[2] = (u16DiValid & (1 << 2));
    parsedAppIoPacket->bDiValid[3] = (u16DiValid & (1 << 3));
    parsedAppIoPacket->bDiValid[4] = (u16DiValid & (1 << 4));
    parsedAppIoPacket->bDiValid[5] = (u16DiValid & (1 << 5));
    parsedAppIoPacket->bDiValid[6] = (u16DiValid & (1 << 6));
    parsedAppIoPacket->bDiValid[7] = (u16DiValid & (1 << 7));
    parsedAppIoPacket->bDiValid[8] = (u16DiValid & (1 << 8));
    parsedAppIoPacket->bDiValid[9] = (u16DiValid & (1 << 9));
    parsedAppIoPacket->bDiValid[10] = (u16DiValid & (1 << 10));
    parsedAppIoPacket->bDiValid[11] = (u16DiValid & (1 << 11));
    const uint16_t u16DiInterrupt = barePacket.u16At(17);
    parsedAppIoPacket->bDiInterrupt[0] = (u16DiInterrupt & (1 << 0));
    parsedAppIoPacket->bDiInterrupt[1] = (u16DiInterrupt & (1 << 1));
    parsedAppIoPacket->bDiInterrupt[2] = (u16DiInterrupt & (1 << 2));
    parsedAppIoPacket->bDiInterrupt[3] = (u16DiInterrupt & (1 << 3));
    parsedAppIoPacket->bDiInterrupt[4] = (u16DiInterrupt & (1 << 4));
    parsedAppIoPacket->bDiInterrupt[5] = (u16DiInterrupt & (1 << 5));
    parsedAppIoPacket->bDiInterrupt[6] = (u16DiInterrupt & (1 << 6));
    parsedAppIoPacket->bDiInterrupt[7] = (u16DiInterrupt & (1 << 7));
    parsedAppIoPacket->bDiInterrupt[8] = (u16DiInterrupt & (1 << 8));
    parsedAppIoPacket->bDiInterrupt[9] = (u16DiInterrupt & (1 << 9));
    parsedAppIoPacket->bDiInterrupt[10] = (u16DiInterrupt & (1 << 10));
    parsedAppIoPacket->bDiInterrupt[11] = (u16DiInterrupt & (1 << 11));

    return true;
}
