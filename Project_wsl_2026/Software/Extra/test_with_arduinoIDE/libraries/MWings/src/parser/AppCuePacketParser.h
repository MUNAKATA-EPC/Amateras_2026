/**
 * @file   AppCuePacketParser.h
 * @brief  App_CUE (CUE mode) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPCUEPACKETPARSER_H
#define APPCUEPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppCuePacket
 * @brief  Packet content for App_CUE
 */
struct ParsedAppCuePacket final : public mwings::ParsedPacketBase {
    int16_t i16SamplesX[10];
    int16_t i16SamplesY[10];
    int16_t i16SamplesZ[10];
    uint8_t u8SampleCount;
    bool bHasAccelEvent;
    uint8_t u8AccelEvent;
    uint8_t u8MagnetState;
    bool bMagnetStateChanged;
};

/**
 * @class appcue::PacketParser
 * @brief  Packet parser for App_CUE (CUE mode)
 */
namespace appcue {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_CUE (CUE mode)
    // I think I've nearly found you
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x05)
            and (barePacket.u16PayloadSize == 148)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    // I can see clues all around me
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern appcue::PacketParser AppCuePacketParser;

#endif  // APPCUEPACKETPARSER_H
