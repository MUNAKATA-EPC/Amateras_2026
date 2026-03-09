/**
 * @file   AppTweliteCommandSerializer.h
 * @brief  App_Twelite 0x80 command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPTWELITECOMMANDSERIALIZER_H
#define APPTWELITECOMMANDSERIALIZER_H

#include "MWings_Common.h"

/**
 * @struct AppTweliteCommand
 * @brief  Command content for App_Twelite
 */
struct AppTweliteCommand final : public mwings::CommandBase {
    bool bDiToChange[4];
    bool bDiState[4];
    bool bPwmToChange[4];
    uint16_t u16PwmDuty[4];
    AppTweliteCommand() : bDiToChange{true, true, true, true},
                          bDiState{false, false, false, false},
                          bPwmToChange{true, true, true, true},
                          u16PwmDuty{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}
        {}
    // Check if the command is valid for App_Twelite
    inline bool isValid() const override {
        return ((0x00 <= u8DestinationLogicalId and u8DestinationLogicalId <= 0x78)
                and (u16PwmDuty[0] <= 1024 or u16PwmDuty[0] == 0xFFFF)
                and (u16PwmDuty[1] <= 1024 or u16PwmDuty[1] == 0xFFFF)
                and (u16PwmDuty[2] <= 1024 or u16PwmDuty[2] == 0xFFFF)
                and (u16PwmDuty[3] <= 1024 or u16PwmDuty[3] == 0xFFFF));
    }
};

/**
 * @brief   Returns payload size for serialized App_Twelite command
 * @see     constexpr
 * @return  Payload size in binary count
 */
constexpr int GetAppTweliteSerializedCommandPayloadSize() { return 13; }

/**
 * @class apptwelite::CommandSerializer
 * @brief  Command serializer for App_Twelite
 */
namespace apptwelite {
class CommandSerializer final : public mwings::CommandSerializerBase {
public:
    // Serialize command
    bool serialize(const mwings::CommandBase* const command,
                   uint8_t* const payload, const int maxPayloadSize,
                   uint8_t* const checksum) const override;
};
}

extern apptwelite::CommandSerializer AppTweliteCommandSerializer;

#endif  // APPTWELITECOMMANDSERIALIZER_H
