/**
 * @file   AppUartAsciiCommand.h
 * @brief  App_Uart (Mode A) command for MWings.
 *
 * There's no need to serialize AppUartAsciiCommand.
 * Use MWings::send(const uint8_t, const uint8_t, const uint8_t* const, const int) directly.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPUARTASCIICOMMAND_H
#define APPUARTASCIICOMMAND_H

#include "MWings_Common.h"

/**
 * @struct AppUartAsciiCommand
 * @brief   command content for App_Uart (Mode A)
 */
struct AppUartAsciiCommand final : public mwings::CommandBase {
    uint8_t u8CommandId;
    uint8_t* u8Data;
    uint16_t u16DataSize;

    AppUartAsciiCommand() : u8CommandId(0x78), u8Data(nullptr), u16DataSize(0)
        {}
    // Check if the command is valid for App_Uart (Mode A)
    inline bool isValid() const override {
        return (((0x00 <= u8DestinationLogicalId and u8DestinationLogicalId <= 0x64)
                 or u8DestinationLogicalId == 0x78)
                and u8CommandId < 0x80
                and (not (u8Data == nullptr))
                and (1 <= u16DataSize and u16DataSize <= 80));
                // and (1 <= u16DataSize and u16DataSize <= 640));
    }
};

namespace appuartascii {
// There's no need to serialize AppUartAsciiCommand.
// Use MWings::send(const uint8_t, const uint8_t, const uint8_t* const, const int) directly.
}

#endif  // APPUARTASCIICOMMAND_H
