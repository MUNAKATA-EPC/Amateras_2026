/**
 * @file   AppUartAsciiExtendedPacketParser.cpp
 * @brief  App_Uart (Mode A) extended packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppUartAsciiExtendedPacketParser.h"

appuartascii::ExtendedPacketParser AppUartAsciiExtendedPacketParser;

bool appuartascii::ExtendedPacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppUartAsciiExtendedPacket* const parsedAppUartAsciiExtendedPacket = static_cast<ParsedAppUartAsciiExtendedPacket*>(parsedPacket);

    parsedAppUartAsciiExtendedPacket->u32SourceSerialId = barePacket.u32At(3);
    parsedAppUartAsciiExtendedPacket->u8SourceLogicalId = barePacket.u8At(0);
    parsedAppUartAsciiExtendedPacket->u16SequenceNumber = 0x0000;
    parsedAppUartAsciiExtendedPacket->u8Lqi = barePacket.u8At(11);
    parsedAppUartAsciiExtendedPacket->u16SupplyVoltage = 0x0000;

    parsedAppUartAsciiExtendedPacket->u32DestinationSerialId = barePacket.u32At(7);
    parsedAppUartAsciiExtendedPacket->u8CommandId = barePacket.u8At(2);
    parsedAppUartAsciiExtendedPacket->u8Data = barePacket.u8From(14);
    parsedAppUartAsciiExtendedPacket->u16DataSize = barePacket.u16At(12);

    return true;
}
