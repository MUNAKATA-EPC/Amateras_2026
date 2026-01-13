/**
 * @file   AppPalMotPacketParser.h
 * @brief  App_PAL (MOT) packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALMOTPACKETPARSER_H
#define APPPALMOTPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppPalMotPacket
 * @brief  Packet content for App_PAL
 */
struct ParsedAppPalMotPacket final : public mwings::ParsedPacketBase {
    uint16_t u16Ai1Voltage;
    int16_t i16SamplesX[16];
    int16_t i16SamplesY[16];
    int16_t i16SamplesZ[16];
    uint8_t u8SampleCount;
    uint16_t u16SamplingFrequency;
};

/**
 * @class apppalmot::PacketParser
 * @brief  Packet parser for App_PAL (MOT)
 */
namespace apppalmot {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_PAL (MOT)
    inline bool isValid(const BarePacket& barePacket) const override {
        if (((barePacket.u8At(0) & 0x80) == 0x80)
            and ((barePacket.u8At(7) & 0x80) == 0x80)
            and (barePacket.u8At(12) == 0x80)
            and (barePacket.u8At(13) == 0x83)
            and (barePacket.u16PayloadSize == 188)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern apppalmot::PacketParser AppPalMotPacketParser;

#endif  // APPPALMOTPACKETPARSER_H
