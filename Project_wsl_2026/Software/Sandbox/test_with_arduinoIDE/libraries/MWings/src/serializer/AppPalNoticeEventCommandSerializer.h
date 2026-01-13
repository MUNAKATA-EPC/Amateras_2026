/**
 * @file   AppPalNoticeEventCommandSerializer.h
 * @brief  App_PAL (NOTICE) 0x90 event command serializer for MWings.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef APPPALNOTICEEVENTCOMMANDSERIALIZER_H
#define APPPALNOTICEEVENTCOMMANDSERIALIZER_H

#include "MWings_Common.h"

/**
 * @struct AppPalNoticeEventCommand
 * @brief  Event command content for App_PAL (NOTICE)
 */
struct AppPalNoticeEventCommand final : public mwings::CommandBase {
    uint8_t u8EventId;

    AppPalNoticeEventCommand() : u8EventId(0)
        {}
    // Check if the command is valid for App_PAL (NOTICE)
    inline bool isValid() const override {
        return (0x00 <= u8DestinationLogicalId and u8DestinationLogicalId <= 0x64
                and u8EventId <= 0x10);
    }
};

/**
 * @brief   Returns payload size for serialized App_PAL (NOTICE) event command
 * @see     constexpr
 * @return  Payload size in binary count
 */
constexpr int GetAppPalNoticeSerializedEventCommandPayloadSize() { return 7; }

/**
 * @class apppalnotice::EventCommandSerializer
 * @brief  Event command serializer for App_PAL (NOTICE)
 */
namespace apppalnotice {
class EventCommandSerializer final : public mwings::CommandSerializerBase {
public:
    // Serialize command
    bool serialize(const mwings::CommandBase* const command,
                   uint8_t* const payload, const int maxPayloadSize,
                   uint8_t* const checksum) const override;
};
}

extern apppalnotice::EventCommandSerializer AppPalNoticeEventCommandSerializer;

#endif  // APPPALNOTICEEVENTCOMMANDSERIALIZER_H
