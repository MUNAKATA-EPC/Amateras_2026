/**
 * @file   AppIoPacketParser.h
 * @brief  App_IO packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPIOPACKETPARSER_H
#define APPIOPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppIoPacket
 * @brief  Packet content for App_IO
 */
struct ParsedAppIoPacket final : public mwings::ParsedPacketBase {
    uint8_t u8RelayCount;
    bool bDiState[12];
    bool bDiValid[12];
    bool bDiInterrupt[12];
};

/**
 * @class appio::PacketParser
 * @brief  Packet parser for App_IO
 */
namespace appio {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_IO
    inline bool isValid(const BarePacket& barePacket) const override {
        if ((barePacket.u8At(1) == 0x81)
            and (barePacket.u8At(3) == 0x02)
            and ((barePacket.u8At(5) & 0x80) == 0x80)
            and (barePacket.u16PayloadSize == 20)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern appio::PacketParser AppIoPacketParser;

#endif  // APPIOPACKETPARSER_H
