/**
 * @file   AppPalOpenClosePacketParser.h
 * @brief  App_PAL (OPENCLOSE) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALOPENCLOSEPACKETPARSER_H
#define APPPALOPENCLOSEPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppPalOpenClosePacket
 * @brief  Packet content for App_PAL
 */
struct ParsedAppPalOpenClosePacket final : public mwings::ParsedPacketBase {
    uint16_t u16Ai1Voltage;
    uint8_t u8MagnetState;
    bool bMagnetStateChanged;
};

/**
 * @class apppalopenclose::PacketParser
 * @brief  Packet parser for App_PAL (OPENCLOSE)
 */
namespace apppalopenclose {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_PAL (OPENCLOSE)
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x81)
            and (barePacket.u16PayloadSize == 33)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern apppalopenclose::PacketParser AppPalOpenClosePacketParser;

#endif  // APPPALOPENCLOSEPACKETPARSER_H
