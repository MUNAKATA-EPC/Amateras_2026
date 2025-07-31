/**
 * @file   AppAriaPacketParser.h
 * @brief  App_ARIA (ARIA mode) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPARIAPACKETPARSER_H
#define APPARIAPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppAriaPacket
 * @brief  Packet content for App_ARIA
 */
struct ParsedAppAriaPacket final : public mwings::ParsedPacketBase {
    int16_t i16Temp100x;
    uint16_t u16Humid100x;
    uint8_t u8MagnetState;
    bool bMagnetStateChanged;
};

/**
 * @class apparia::PacketParser
 * @brief  Packet parser for App_ARIA (ARIA mode)
 */
namespace apparia {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_ARIA (ARIA mode)
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x06)
            and (barePacket.u16PayloadSize == 60)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern apparia::PacketParser AppAriaPacketParser;

#endif  // APPARIAPACKETPARSER_H
