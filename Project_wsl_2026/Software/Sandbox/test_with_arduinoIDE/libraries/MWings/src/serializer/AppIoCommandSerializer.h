/**
 * @file   AppIoCommandSerializer.h
 * @brief  App_Io 0x80 command serializer for MWings.
 *
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPIOCOMMANDSERIALIZER_H
#define APPIOCOMMANDSERIALIZER_H

#include "MWings_Common.h"

/**
 * @struct AppIoCommand
 * @brief  Command content for App_Io
 */
struct AppIoCommand final : public mwings::CommandBase {
    bool bDiToChange[12];
    bool bDiState[12];
    AppIoCommand() : bDiToChange{true, true, true, true, true, true, true, true, true, true, true, true},
                     bDiState{false, false, false, false, false, false, false, false, false, false, false, false}
        {}
    // Check if the command is valid for App_Io
    inline bool isValid() const override {
        return (0x00 <= u8DestinationLogicalId and u8DestinationLogicalId <= 0x78);
    }
};

/**
 * @brief   Returns payload size for serialized App_Io command
 * @see     constexpr
 * @return  Payload size in binary count
 */
constexpr int GetAppIoSerializedCommandPayloadSize() { return 15; }

/**
 * @class appio::CommandSerializer
 * @brief  Command serializer for App_Io
 */
namespace appio {
class CommandSerializer final : public mwings::CommandSerializerBase {
public:
    // Serialize command
    bool serialize(const mwings::CommandBase* const command,
                   uint8_t* const payload, const int maxPayloadSize,
                   uint8_t* const checksum) const override;
};
}

extern appio::CommandSerializer AppIoCommandSerializer;

#endif  // APPIOCOMMANDSERIALIZER_H
