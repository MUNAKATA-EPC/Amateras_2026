/**
 * @file   AppPalNoticeCommandSerializer.cpp
 * @brief  App_PAL (NOTICE) 0x90 command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppPalNoticeCommandSerializer.h"

apppalnotice::CommandSerializer AppPalNoticeCommandSerializer;

bool apppalnotice::CommandSerializer::serialize(const mwings::CommandBase* const command,
                                                uint8_t* const payload, const int maxPayloadSize,
                                                uint8_t* const checksum) const
{
    constexpr int fixedPayloadSize = GetAppPalNoticeSerializedCommandPayloadSize();
    if (not (maxPayloadSize >= fixedPayloadSize)) { return false; }

    // WARNING: Note that there is NO RTTI
    const AppPalNoticeCommand* const appPalNoticeCommand = static_cast<const AppPalNoticeCommand*>(command);

    if (not appPalNoticeCommand->isValid()) { return false; }

    payload[0] = (appPalNoticeCommand->u8DestinationLogicalId <= 0x64)
        ? appPalNoticeCommand->u8DestinationLogicalId : 0x64;
    payload[1] = 0x90;
    payload[2] = 2;

    // Set LED Color, Blink speed and Brightness
    payload[3] = 0x01;
    payload[4] = static_cast<uint8_t>(appPalNoticeCommand->eColor);
    payload[5] = static_cast<uint8_t>(appPalNoticeCommand->eBlinkSpeed);
    payload[6] = appPalNoticeCommand->u8Brightness;

    // Set duration
    payload[7] = 0x02;
    payload[8] = 0xFF;
    payload[9] = (appPalNoticeCommand->u16DurationInSec >> 8) & 0xFF;
    payload[10] = (appPalNoticeCommand->u16DurationInSec >> 0) & 0xFF;

    *(checksum) = 0;
    for (int i = 0; i < fixedPayloadSize; i++) {
        *(checksum) += payload[i];
    }
    *(checksum) = ~(*(checksum)) + 1;

    return true;
}
