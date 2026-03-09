/**
 * @file   AppUartAsciiPacketParser.h
 * @brief  App_Uart (Mode A) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPUARTASCIIPACKETPARSER_H
#define APPUARTASCIIPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppUartAsciiPacket
 * @brief  Packet content for App_Uart (Mode A)
 */
struct ParsedAppUartAsciiPacket final : public mwings::ParsedPacketBase {
    uint8_t u8CommandId;
    uint8_t* u8Data;
    uint16_t u16DataSize;
};

/**
 * @class appuartascii::PacketParser
 * @brief  Packet parser for App_Uart (Mode A)
 */
namespace appuartascii {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_Uart (Mode A)
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((0x00 <= barePacket.u8At(0) and barePacket.u8At(0) <= 0x64)
             or barePacket.u8At(0) == 0x78)
            and barePacket.u8At(1) < 0x80
            and (3 <= barePacket.u16PayloadSize and barePacket.u16PayloadSize <= 82)) {
            // and (3 <= barePacket.u16PayloadSize and barePacket.u16PayloadSize <= 642)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern appuartascii::PacketParser AppUartAsciiPacketParser;

#endif  // APPUARTASCIIPACKETPARSER_H
