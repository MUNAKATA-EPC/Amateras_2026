/**
 * @file   AppPalNoticeCommandSerializer.h
 * @brief  App_PAL (NOTICE) 0x90 command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALNOTICECOMMANDSERIALIZER_H
#define APPPALNOTICECOMMANDSERIALIZER_H

#include "MWings_Common.h"

enum class AppPalNoticeColor : uint8_t {
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    YELLOW = 3,
    PURPLE = 4,
    LIGHT_BLUE = 5,
    WHITE = 6,
    WARM_WHITE = 7
};

enum class AppPalNoticeBlinkSpeed : uint8_t {
    ALWAYS_ON = 0,
    SLOW = 1,
    MEDIUM = 2,
    FAST = 3
};

/**
 * @struct AppPalNoticeCommand
 * @brief  Command content for App_PAL (NOTICE)
 */
struct AppPalNoticeCommand final : public mwings::CommandBase {
    AppPalNoticeColor eColor;
    AppPalNoticeBlinkSpeed eBlinkSpeed;
    uint8_t u8Brightness;
    uint16_t u16DurationInSec;

    AppPalNoticeCommand() : eColor(AppPalNoticeColor::WHITE),
                            eBlinkSpeed(AppPalNoticeBlinkSpeed::MEDIUM),
                            u8Brightness(0x8),
                            u16DurationInSec(5)
        {}
    // Check if the command is valid for App_PAL (NOTICE)
    inline bool isValid() const override {
        return (0x00 <= u8DestinationLogicalId and u8DestinationLogicalId <= 0x64
                and u8Brightness <= 0xF);
    }
};

/**
 * @brief   Returns payload size for serialized App_PAL (NOTICE) command
 * @see     constexpr
 * @return  Payload size in binary count
 */
constexpr int GetAppPalNoticeSerializedCommandPayloadSize() { return 11; }

/**
 * @class apppalnotice::CommandSerializer
 * @brief  Command serializer for App_PAL (NOTICE)
 */
namespace apppalnotice {
class CommandSerializer final : public mwings::CommandSerializerBase {
public:
    // Serialize command
    bool serialize(const mwings::CommandBase* const command,
                   uint8_t* const payload, const int maxPayloadSize,
                   uint8_t* const checksum) const override;
};
}

extern apppalnotice::CommandSerializer AppPalNoticeCommandSerializer;

#endif  // APPPALNOTICECOMMANDSERIALIZER_H
