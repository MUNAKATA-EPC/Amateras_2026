/**
 * @file   AppIoCommandSerializer.cpp
 * @brief  App_Io 0x80 command serializer for MWings.
 *
 * Copyright (C) 2024 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#include "AppIoCommandSerializer.h"

appio::CommandSerializer AppIoCommandSerializer;

bool appio::CommandSerializer::serialize(const mwings::CommandBase* const command,
                                              uint8_t* const payload, const int maxPayloadSize,
                                              uint8_t* const checksum) const
{
    constexpr int fixedPayloadSize = GetAppIoSerializedCommandPayloadSize();
    if (not (maxPayloadSize >= fixedPayloadSize)) { return false; }

    // WARNING: Note that there is NO RTTI
    const AppIoCommand* const appIoCommand = static_cast<const AppIoCommand*>(command);

    if (not appIoCommand->isValid()) { return false; }

    payload[0] = (appIoCommand->u8DestinationLogicalId <= 0x78)
                 ? appIoCommand->u8DestinationLogicalId : 0x78;
    payload[1] = 0x80;
    payload[2] = 0x01;
    uint16_t u16DiState = 0;
    uint16_t u16DiToChange = 0;
    for (int i = 0; i < 12; i++) {
        if (appIoCommand->bDiState[i]) { u16DiState |= (1 << i); }
        if (appIoCommand->bDiToChange[i]) { u16DiToChange |= (1 << i); }
    }
    payload[3] = (u16DiState >> 8) & 0xFF;
    payload[4] = (u16DiState >> 0) & 0xFF;
    payload[5] = (u16DiToChange >> 8) & 0xFF;
    payload[6] = (u16DiToChange >> 0) & 0xFF;
    for (int i = 7; i < fixedPayloadSize; i++) {
        payload[i] = 0;
    }
    *(checksum) = 0;
    for (int i = 0; i < fixedPayloadSize; i++) {
        *(checksum) += payload[i];
    }
    *(checksum) = ~(*(checksum)) + 1;

    return true;
}
