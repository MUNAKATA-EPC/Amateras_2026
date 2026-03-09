/**
 * @file   AppPalNoticeDetailedCommandSerializer.cpp
 * @brief  App_PAL (NOTICE) 0x90 detailed command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppPalNoticeDetailedCommandSerializer.h"

apppalnotice::DetailedCommandSerializer AppPalNoticeDetailedCommandSerializer;

bool apppalnotice::DetailedCommandSerializer::serialize(const mwings::CommandBase* const command,
                                                        uint8_t* const payload, const int maxPayloadSize,
                                                        uint8_t* const checksum) const
{
    constexpr int fixedPayloadSize = GetAppPalNoticeSerializedDetailedCommandPayloadSize();
    if (not (maxPayloadSize >= fixedPayloadSize)) { return false; }

    // WARNING: Note that there is NO RTTI
    const AppPalNoticeDetailedCommand* const appPalNoticeDetailedCommand = static_cast<const AppPalNoticeDetailedCommand*>(command);

    if (not appPalNoticeDetailedCommand->isValid()) { return false; }

    payload[0] = (appPalNoticeDetailedCommand->u8DestinationLogicalId <= 0x64)
        ? appPalNoticeDetailedCommand->u8DestinationLogicalId : 0x64;
    payload[1] = 0x90;
    payload[2] = 3;

    // Set LED Color in RGBW
    payload[3] = 0x03;
    payload[4] = 0xFF;
    payload[5] = (appPalNoticeDetailedCommand->sRGBWColor.u16() >> 8) & 0xFF;
    payload[6] = (appPalNoticeDetailedCommand->sRGBWColor.u16() >> 0) & 0xFF;

    // Set LED blinking behavior
    payload[7] = 0x04;
    payload[8] = 0xFF;
    payload[9] = appPalNoticeDetailedCommand->u8BlinkDutyPercentage * 0xFF / 100;
    payload[10] = appPalNoticeDetailedCommand->fBlinkPeriodInSec / 0.04f;

    // Set duration
    payload[11] = 0x02;
    payload[12] = 0xFF;
    payload[13] = (appPalNoticeDetailedCommand->u16DurationInSec >> 8) & 0xFF;
    payload[14] = (appPalNoticeDetailedCommand->u16DurationInSec >> 0) & 0xFF;

    *(checksum) = 0;
    for (int i = 0; i < fixedPayloadSize; i++) {
        *(checksum) += payload[i];
    }
    *(checksum) = ~(*(checksum)) + 1;

    return true;
}
