/**
 * @file   AppPalAmbPacketParser.h
 * @brief  App_PAL (AMB) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALAMBPACKETPARSER_H
#define APPPALAMBPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppPalAmbPacket
 * @brief  Packet content for App_PAL
 */
struct ParsedAppPalAmbPacket final : public mwings::ParsedPacketBase {
    uint16_t u16Ai1Voltage;
    int16_t i16Temp100x;
    uint16_t u16Humid100x;
    uint32_t u32Illuminance;
};

/**
 * @class apppalamb::PacketParser
 * @brief  Packet parser for App_PAL (AMB)
 */
namespace apppalamb {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_PAL (AMB)
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x82)
            and (barePacket.u16PayloadSize == 48)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern apppalamb::PacketParser AppPalAmbPacketParser;

#endif  // APPPALAMBPACKETPARSER_H
