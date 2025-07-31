/**
 * @file   AppUartAsciiPacketParser.cpp
 * @brief  App_Uart (Mode A) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppUartAsciiPacketParser.h"

appuartascii::PacketParser AppUartAsciiPacketParser;

bool appuartascii::PacketParser::parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const
{
    // WARNING: Note that there is NO RTTI
    ParsedAppUartAsciiPacket* const parsedAppUartAsciiPacket = static_cast<ParsedAppUartAsciiPacket*>(parsedPacket);

    parsedAppUartAsciiPacket->u32SourceSerialId = 0x00000000;
    parsedAppUartAsciiPacket->u8SourceLogicalId = barePacket.u8At(0);
    parsedAppUartAsciiPacket->u16SequenceNumber = 0x0000;
    parsedAppUartAsciiPacket->u8Lqi = 0x00;
    parsedAppUartAsciiPacket->u16SupplyVoltage = 0x0000;

    parsedAppUartAsciiPacket->u8CommandId = barePacket.u8At(1);
    parsedAppUartAsciiPacket->u8Data = barePacket.u8From(2);
    parsedAppUartAsciiPacket->u16DataSize = barePacket.u16PayloadSize - 2;

    return true;
}
