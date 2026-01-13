/**
 * @file   MWings_Common.h
 * @brief  Mono Wireless TWELITE Wings API Common Definitions
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef MWINGS_COMMON_H
#define MWINGS_COMMON_H

#include <Arduino.h>

/**
 * @struct BarePacket
 * @brief  Bare packet contents
 */
struct BarePacket {
    uint8_t* u8Payload;
    uint16_t u16PayloadSize;
    uint8_t u8Checksum;

    inline uint8_t* u8From(const int index) const {
        return ((index < u16PayloadSize) ? u8Payload + index : nullptr);
    }
    inline uint8_t u8At(const int index) const {
        return ((index < u16PayloadSize) ? u8Payload[index] : 0x00);
    }
    inline int8_t i8At(const int index) const {
        return static_cast<int8_t>(u8At(index));
    }
    inline uint16_t u16At(const int index) const {
        return ((index + 1 < u16PayloadSize) ? ((u8Payload[index] << 8) | u8Payload[index+1]) : 0x0000);
    }
    inline int16_t i16At(const int index) const {
        return static_cast<int16_t>(u16At(index));
    }
    inline uint32_t u32At(const int index) const {
        return ((index + 3 < u16PayloadSize)
                ? ((u8Payload[index+0] << 24) | (u8Payload[index+1] << 16) | (u8Payload[index+2] << 8) | (u8Payload[index+3] << 0))
                : 0x00000000);
    }
    inline int32_t i32At(const int index) const {
        return static_cast<int16_t>(u16At(index));
    }
};

namespace mwings {
/**
 * @struct ParsedPacketBase
 * @brief  Contains essential packet contents
 */
struct ParsedPacketBase {
    uint32_t u32SourceSerialId;
    uint8_t u8SourceLogicalId;
    uint16_t u16SequenceNumber;
    uint8_t u8Lqi;
    uint16_t u16SupplyVoltage;
protected:
    ParsedPacketBase() {}
    virtual ~ParsedPacketBase() {}
};

/**
 * @struct CommandBase
 * @brief  Contains essential command contents
 */
struct CommandBase {
    uint8_t u8DestinationLogicalId;
protected:
    CommandBase() : u8DestinationLogicalId(0x78)
        {}
    virtual ~CommandBase() {}
    // Check if the command is valid or not
    virtual bool isValid() const = 0;
};

/**
 * @class  PacketParserBase
 * @brief  Packet parser base class
 */
class PacketParserBase {
public:
    // Check if the packet is valid or not
    virtual bool isValid(const BarePacket& barePacket) const = 0;
    // Get parsed packet from bare packet
    virtual bool parse(const BarePacket& barePacket, ParsedPacketBase* const parsedPacket) const = 0;
protected:
    PacketParserBase() {}
    virtual ~PacketParserBase() {}
};

/**
 * @class  CommandSerializerBase
 * @brief  Command serializer base class
 */
class CommandSerializerBase {
public:
    // Serialize command
    virtual bool serialize(const CommandBase* const command, uint8_t* const payload, const int maxPayloadSize, uint8_t* const checksum) const = 0;
protected:
    CommandSerializerBase() {}
    virtual ~CommandSerializerBase() {}
};
}

#endif  // MWINGS_COMMON_H
