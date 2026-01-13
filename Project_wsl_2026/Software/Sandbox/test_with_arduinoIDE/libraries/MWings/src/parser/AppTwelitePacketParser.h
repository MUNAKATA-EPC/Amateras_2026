/**
 * @file   AppTwelitePacketParser.h
 * @brief  App_Twelite packet parser for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPTWELITEPACKETPARSER_H
#define APPTWELITEPACKETPARSER_H

#include "MWings_Common.h"

/**
 * @struct ParsedAppTwelitePacket
 * @brief  Packet content for App_Twelite
 */
struct ParsedAppTwelitePacket final : public mwings::ParsedPacketBase {
    uint8_t u8DestinationLogicalId;
    uint8_t u8RelayCount;
    bool bPeriodic;
    bool bDiChanged[4];
    bool bDiState[4];
    uint16_t u16AiVoltage[4];
};

/**
 * @class apptwelite::PacketParser
 * @brief  Packet parser for App_Twelite
 */
namespace apptwelite {
class PacketParser final : public mwings::PacketParserBase {
public:
    // Check if the packet is from App_Twelite
    inline bool isValid(const BarePacket& barePacket) const override {
        if ((barePacket.u8At(1) == 0x81)
            and (barePacket.u8At(3) == 0x01)
            and ((barePacket.u8At(5) & 0x80) == 0x80)
            and (barePacket.u16PayloadSize == 23)) {
            return true;
        }
        return false;
    }

    // Parse from bare packet
    bool parse(const BarePacket& barePacket, mwings::ParsedPacketBase* const parsedPacket) const override;
};
}

extern apptwelite::PacketParser AppTwelitePacketParser;

#endif  // APPTWELITEPACKETPARSER_H
