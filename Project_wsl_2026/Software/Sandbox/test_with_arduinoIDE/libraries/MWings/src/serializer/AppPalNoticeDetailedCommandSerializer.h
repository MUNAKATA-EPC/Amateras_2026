/**
 * @file   AppPalNoticeDetailedCommandSerializer.h
 * @brief  App_PAL (NOTICE) 0x90 detailed command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALNOTICEDETAILEDCOMMANDSERIALIZER_H
#define APPPALNOTICEDETAILEDCOMMANDSERIALIZER_H

#include "MWings_Common.h"

struct AppPalNoticeRGBWColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;

    AppPalNoticeRGBWColor() : red(0), green(0), blue(0), white(15) {}
    AppPalNoticeRGBWColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) : red(r), green(g), blue(b), white(w) {}
    inline bool isValid() const {
        return (red <= 0xF and green <= 0xF and blue <= 0xF and white <= 0xF);
    }
    inline uint16_t u16() const {
        return (((white & 0xF) << 12)
                | ((blue & 0xF) << 8)
                | ((green & 0xF) << 4)
                | ((red & 0xF) << 0));
    }
};

/**
 * @struct AppPalNoticeDetailedCommand
 * @brief  Detailed command content for App_PAL (NOTICE)
 */
struct AppPalNoticeDetailedCommand final : public mwings::CommandBase {
    AppPalNoticeRGBWColor sRGBWColor;
    uint8_t u8BlinkDutyPercentage;
    float fBlinkPeriodInSec;
    uint16_t u16DurationInSec;

    AppPalNoticeDetailedCommand() : sRGBWColor(AppPalNoticeRGBWColor()),
                                    u8BlinkDutyPercentage(100),
                                    fBlinkPeriodInSec(1.0f),
                                    u16DurationInSec(1)
        {}
    // Check if the command is valid for App_PAL (NOTICE)
    inline bool isValid() const override {
        return (0x00 <= u8DestinationLogicalId and u8DestinationLogicalId <= 0x64
                and sRGBWColor.isValid()
                and u8BlinkDutyPercentage <= 100
                and fBlinkPeriodInSec <= 10.2f);
    }
};

/**
 * @brief   Returns payload size for serialized App_PAL (NOTICE) detailed command
 * @see     constexpr
 * @return  Payload size in binary count
 */
constexpr int GetAppPalNoticeSerializedDetailedCommandPayloadSize() { return 15; }

/**
 * @class apppalnotice::DetailedCommandSerializer
 * @brief  Detailed command serializer for App_PAL (NOTICE)
 */
namespace apppalnotice {
class DetailedCommandSerializer final : public mwings::CommandSerializerBase {
public:
    // Serialize command
    bool serialize(const mwings::CommandBase* const command,
                   uint8_t* const payload, const int maxPayloadSize,
                   uint8_t* const checksum) const override;
};
}

extern apppalnotice::DetailedCommandSerializer AppPalNoticeDetailedCommandSerializer;

#endif  // APPPALNOTICEDETAILEDCOMMANDSERIALIZER_H
