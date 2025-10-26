/**
 * @file   AppPalNoticeEventCommandSerializer.cpp
 * @brief  App_PAL (NOTICE) 0x90 event command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppPalNoticeEventCommandSerializer.h"

apppalnotice::EventCommandSerializer AppPalNoticeEventCommandSerializer;

bool apppalnotice::EventCommandSerializer::serialize(const mwings::CommandBase* const command,
                                                     uint8_t* const payload, const int maxPayloadSize,
                                                     uint8_t* const checksum) const
{
    constexpr int fixedPayloadSize = GetAppPalNoticeSerializedEventCommandPayloadSize();
    if (not (maxPayloadSize >= fixedPayloadSize)) { return false; }

    // WARNING: Note that there is NO RTTI
    const AppPalNoticeEventCommand* const appPalNoticeEventCommand = static_cast<const AppPalNoticeEventCommand*>(command);

    if (not appPalNoticeEventCommand->isValid()) { return false; }

    payload[0] = (appPalNoticeEventCommand->u8DestinationLogicalId <= 0x64)
        ? appPalNoticeEventCommand->u8DestinationLogicalId : 0x64;
    payload[1] = 0x90;
    payload[2] = 1;

    // Tell an event id
    payload[3] = 0x00;
    payload[4] = 0x04;
    payload[5] = 0x00;
    payload[6] = appPalNoticeEventCommand->u8EventId;

    *(checksum) = 0;
    for (int i = 0; i < fixedPayloadSize; i++) {
        *(checksum) += payload[i];
    }
    *(checksum) = ~(*(checksum)) + 1;

    return true;
}
