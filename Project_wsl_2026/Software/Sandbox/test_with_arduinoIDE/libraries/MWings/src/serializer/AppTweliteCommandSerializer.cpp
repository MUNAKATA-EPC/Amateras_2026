/**
 * @file   AppTweliteCommandSerializer.cpp
 * @brief  App_Twelite 0x80 command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppTweliteCommandSerializer.h"

apptwelite::CommandSerializer AppTweliteCommandSerializer;

bool apptwelite::CommandSerializer::serialize(const mwings::CommandBase* const command,
                                              uint8_t* const payload, const int maxPayloadSize,
                                              uint8_t* const checksum) const
{
    constexpr int fixedPayloadSize = GetAppTweliteSerializedCommandPayloadSize();
    if (not (maxPayloadSize >= fixedPayloadSize)) { return false; }

    // WARNING: Note that there is NO RTTI
    const AppTweliteCommand* const appTweliteCommand = static_cast<const AppTweliteCommand*>(command);

    if (not appTweliteCommand->isValid()) { return false; }

    payload[0] = (appTweliteCommand->u8DestinationLogicalId <= 0x78)
        ? appTweliteCommand->u8DestinationLogicalId : 0x78;
    payload[1] = 0x80;
    payload[2] = 0x01;
    payload[3] = 0; payload[4] = 0;
    for (int i = 0; i < 4; i++) {
        if (appTweliteCommand->bDiState[i]) { payload[3] |= (1 << i); }
        if (appTweliteCommand->bDiToChange[i]) { payload[4] |= (1 << i); }
        if ((not appTweliteCommand->bPwmToChange[i]) or appTweliteCommand->u16PwmDuty[i] == 0xFFFF) {
            payload[5+2*i+0] = 0xFF;
            payload[5+2*i+1] = 0xFF;
        } else {
            if (appTweliteCommand->u16PwmDuty[i] > 1024) {
                payload[5+2*i+0] = static_cast<uint8_t>(1024 >> 8);
                payload[5+2*i+1] = static_cast<uint8_t>(1024 & 0xFF);
            } else {
                payload[5+2*i+0] = static_cast<uint8_t>(appTweliteCommand->u16PwmDuty[i] >> 8);
                payload[5+2*i+1] = static_cast<uint8_t>(appTweliteCommand->u16PwmDuty[i] & 0xFF);
            }
        }
    }

    *(checksum) = 0;
    for (int i = 0; i < fixedPayloadSize; i++) {
        *(checksum) += payload[i];
    }
    *(checksum) = ~(*(checksum)) + 1;

    return true;
}
