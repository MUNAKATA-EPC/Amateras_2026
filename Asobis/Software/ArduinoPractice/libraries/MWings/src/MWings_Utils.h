/**
 * @file   MWings_Utils.h
 * @brief  Mono Wireless TWELITE Wings API Utils
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#ifndef MWINGS_UTILS_H
#define MWINGS_UTILS_H

#include <Arduino.h>
#include "MWings_Common.h"

namespace mwings {
class Utils {
public:
    static constexpr uint8_t HexFrom(const uint8_t character) {
        return (character < 'A') ? (character - '0') : (character - 'A' + 0xA);
    }

    static constexpr uint8_t CharacterFrom(const uint8_t hex) {
        return (hex < 0xA) ? ('0' + hex) : ('A' + hex - 0xA);
    }

    static constexpr int ByteCountFrom(const int characterCount) {
        return characterCount / 2;
    }

    static constexpr int CharacterCountFrom(const int byteCount) {
        return byteCount * 2;
    }

    static inline bool IsInitialized(HardwareSerial* serial) {
        return (serial ? true : false);
    }

    static inline bool IsReadable(HardwareSerial* serial) {
        return (serial and serial->available());
    }

    static inline bool IsWritable(HardwareSerial* serial) {
#if defined(ARDUINO_ARCH_RENESAS)
        return (serial ? true : false);
#else
        return (serial and serial->availableForWrite());
#endif
    }

    static inline void WriteBinary(HardwareSerial* serial, const uint8_t data) {
        if (not IsWritable(serial)) { return; }
        serial->write(data);
    }

    static inline void WriteBinary(HardwareSerial* serial, const uint16_t data) {
        WriteBinary(serial, static_cast<uint8_t>((data >> 8) & 0xFF));
        WriteBinary(serial, static_cast<uint8_t>((data >> 0) & 0xFF));
    }

    static inline void WriteBinary(HardwareSerial* serial, const uint32_t data) {
        WriteBinary(serial, static_cast<uint16_t>((data >> 16) & 0xFFFF));
        WriteBinary(serial, static_cast<uint16_t>((data >> 0) & 0xFFFF));
    }

    static inline void WriteBinary(HardwareSerial* serial, const uint8_t* const data, const int size) {
        for (int i = 0; i < size; i++) { WriteBinary(serial, data[i]); }
    }

    static inline void WriteInAscii(HardwareSerial* serial, const uint8_t data) {
        if (not IsWritable(serial)) { return; }
        serial->write(CharacterFrom((data >> 4) & 0x0F));
        serial->write(CharacterFrom((data >> 0) & 0x0F));
    }

    static inline void WriteInAscii(HardwareSerial* serial, const uint16_t data) {
        WriteInAscii(serial, static_cast<uint8_t>((data >> 8) & 0xFF));
        WriteInAscii(serial, static_cast<uint8_t>((data >> 0) & 0xFF));
    }

    static inline void WriteInAscii(HardwareSerial* serial, const uint32_t data) {
        WriteInAscii(serial, static_cast<uint16_t>((data >> 16) & 0xFFFF));
        WriteInAscii(serial, static_cast<uint16_t>((data >> 0) & 0xFFFF));
    }

    static inline void WriteInAscii(HardwareSerial* serial, const uint8_t* const data, const int size) {
        for (int i = 0; i < size; i++) { WriteInAscii(serial, data[i]); }
    }

    static inline void FlushRxBuffer(HardwareSerial* serial) {
        if (not Utils::IsInitialized(serial)) { return; }
        while (serial->available()) { serial->read(); }
    }

    static inline void FlushTxBuffer(HardwareSerial* serial) {
        if (not Utils::IsInitialized(serial)) { return; }
        serial->flush();
    }

    static inline bool FindBinary(HardwareSerial* serial,
                                  const uint8_t* const data, const int size, const uint16_t timeout,
                                  const bool withTerminal = false, const uint8_t terminal = 0,
                                  HardwareSerial* debugSerial = nullptr) {
        if (not IsInitialized(serial)) { return false; }
        if (not (size > 0)) { return false; }
        uint32_t timestamp = millis();
        while (true) {
            if (millis() - timestamp > timeout) { return false; }
            if (serial->available()) {
                if (debugSerial) { debugSerial->write(serial->peek()); }
                if (serial->read() == data[0]) {
                    break;
                }
            }
        }
        for (int i = 1; i < size; i++) {
            if (withTerminal and terminal == data[i]) { break; }
            while (true) {
                if (millis() - timestamp > timeout) { return false; }
                if (serial->available()) {
                    if (debugSerial) { debugSerial->write(serial->peek()); }
                    if (serial->read() == data[i]) {
                        break;
                    } else {
                        return FindBinary(serial,
                                          data, size, timeout - millis() + timestamp,
                                          withTerminal, terminal, debugSerial); // Recurse
                    }
                }
            }
        }
        return true;
    }

    static inline bool FindAscii(HardwareSerial* serial,
                                 const char* const data, const uint16_t timeout,
                                 HardwareSerial* debugSerial = nullptr) {
        return FindBinary(serial,
                          reinterpret_cast<const uint8_t*>(data), 1024, timeout,
                          true, static_cast<uint8_t>('\0'), debugSerial);
    }

private:
    Utils() {}
    virtual ~Utils() {}
};
}

#endif  // MWINGS_UTILS_H
