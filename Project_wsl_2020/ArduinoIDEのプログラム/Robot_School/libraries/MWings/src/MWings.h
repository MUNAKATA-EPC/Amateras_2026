/**
 * @file   MWings.h
 * @brief  Mono Wireless TWELITE Wings API for 32-bit Arduinos.
 *
 * Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
 */

#if defined(ARDUINO_ARCH_ESP32)
#elif defined(ARDUINO_ARCH_RENESAS)
// #elif defined(ARDUINO_ARCH_ESP8266)
// #elif defined(ARDUINO_ARCH_STM32)
// #elif defined(ARDUINO_ARCH_SAM)
// #elif defined(ARDUINO_ARCH_RP2040)
#else
// #error "This library is built for 32-bit MCUs like ESP32, ESP8266, RENESAS (RA4M1), STM32, SAM and RP2040."
#error "This library is built for ESP32 and RENESAS (RA4M1)."
#endif

#ifndef MWINGS_H
#define MWINGS_H

#include <Arduino.h>
#include "MWings_Common.h"
#include "MWings_Utils.h"

//// AppTwelitePacketParser for App_Twelite
#include "parser/AppTwelitePacketParser.h"
//// AppIoPacketParser for App_IO
#include "parser/AppIoPacketParser.h"
//// AppAriaPacketParser for App_ARIA (ARIA mode)
#include "parser/AppAriaPacketParser.h"
//// AppCuePacketParser for App_CUE (CUE mode)
#include "parser/AppCuePacketParser.h"
//// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
#include "parser/AppPalOpenClosePacketParser.h"
//// AppPalAmbPacketParser for App_PAL (AMB)
#include "parser/AppPalAmbPacketParser.h"
//// AppPalMotPacketParser for App_PAL (MOT)
#include "parser/AppPalMotPacketParser.h"
//// AppUartAsciiPacketParser for App_Uart (Mode A)
#include "parser/AppUartAsciiPacketParser.h"
//// AppUartAsciiExtendedPacketParser for App_Uart (Mode A, extended)
#include "parser/AppUartAsciiExtendedPacketParser.h"

//// AppTweliteCommandSerializer for App_Twelite
#include "serializer/AppTweliteCommandSerializer.h"
//// AppIoCommandSerializer for App_Io
#include "serializer/AppIoCommandSerializer.h"
//// AppUartAsciiCommand for App_Uart (Mode A)
#include "serializer/AppUartAsciiCommand.h"
//// AppPalNoticeCommandSerializer for App_PAL (NOTICE)
#include "serializer/AppPalNoticeCommandSerializer.h"
//// AppPalNoticeDetailedCommandSerializer for App_PAL (NOTICE), detailed
#include "serializer/AppPalNoticeDetailedCommandSerializer.h"
//// AppPalNoticeEventCommandSerializer for App_PAL (NOTICE), event
#include "serializer/AppPalNoticeEventCommandSerializer.h"

namespace mwings {
class MWings {
public:
    MWings() : _serial(nullptr),
               _indicatorPin(-1), _resetPin(-1), _programPin(-1),
               _isIndicatorOn(false), _indicatorTimestamp(UINT32_MAX), _indicatorDuration(0),
               _buffer(nullptr), _rxBufferSize(0), _characterCount(0), _checksum(0),
               _timeout(0), _latestTimestamp(UINT32_MAX),
               _debugSerial(nullptr),
               //// AppTwelitePacketParser for App_Twelite
               _onAppTwelitePacket(nullptr),
               //// AppIoPacketParser for App_IO
               _onAppIoPacket(nullptr),
               //// AppAriaPacketParser for App_ARIA (ARIA mode)
               _onAppAriaPacket(nullptr),
               //// AppCuePacketParser for App_CUE (CUE mode)
               _onAppCuePacket(nullptr),
               //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
               _onAppPalOpenClosePacket(nullptr),
               //// AppPalAmbPacketParser for App_PAL (AMB)
               _onAppPalAmbPacket(nullptr),
               //// AppPalMotPacketParser for App_PAL (MOT)
               _onAppPalMotPacket(nullptr),
               //// AppUartAsciiPacketParser for App_Uart (Mode A)
               _onAppUartAsciiPacket(nullptr),
               //// AppUartAsciiExtendedPacketParser for App_Uart (Mode A, extended)
               _onAppUartAsciiExtendedPacket(nullptr),
               _onBarePacket()
        {}
    ~MWings();

    bool begin(HardwareSerial& serial,
               const int indicatorPin = -1, const int resetPin = -1, const int programPin = -1,
               const uint8_t channel = 18, const uint32_t appId = 0x67720102,
               const uint8_t retryCount = 2, const uint8_t txPower = 3,
               const int rxBufferSize = 1024, const int timeout = 100,
               HardwareSerial* debugSerial = nullptr);

    inline void end() {
        _serial = nullptr;
        _indicatorPin = -1, _resetPin = -1; _programPin = -1;
        _isIndicatorOn = false; _indicatorTimestamp = UINT32_MAX; _indicatorDuration = 0;
        if (_buffer) { delete[] _buffer; }
        _rxBufferSize = 0; _characterCount = 0; _checksum = 0;
        _timeout = 0; _latestTimestamp = UINT32_MAX;
        _debugSerial = nullptr;
        //// AppTwelitePacketParser for App_Twelite
        _onAppTwelitePacket = nullptr;
        //// AppIoPacketParser for App_IO
        _onAppIoPacket = nullptr;
        //// AppAriaPacketParser for App_ARIA (ARIA mode)
        _onAppAriaPacket = nullptr;
        //// AppCuePacketParser for App_CUE (CUE mode)
        _onAppCuePacket = nullptr;
        //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
        _onAppPalOpenClosePacket = nullptr;
        //// AppPalAmbPacketParser for App_PAL (AMB)
        _onAppPalAmbPacket = nullptr;
        //// AppPalMotPacketParser for App_PAL (MOT)
        _onAppPalMotPacket = nullptr;
        //// AppUartAsciiPacketParser for App_Uart (Mode A)
        _onAppUartAsciiPacket = nullptr;
        //// AppUartAsciiExtendedPacketParser for App_Uart (Mode A, extended)
        _onAppUartAsciiExtendedPacket = nullptr;
        _onBarePacket = nullptr;
    }

    void update();

private:
    enum class State {
        WAITING_FOR_HEADER,
        RETRIEVING_PAYLOAD,
        WAITING_FOR_FOOTER,
        COMPLETED,
        UNKNOWN_ERROR,
        CHECKSUM_ERROR,
        TIMEOUT_ERROR
    };

    enum class Command : uint8_t {
        ACK = 0xD0,
        MODULE_ADDRESS = 0xD1,
        SET_PARAMETER = 0xD2,
        GET_PARAMETER = 0xD3,
        CONTROL = 0xD8,
        DISABLE_SILENT_MODE = 0xD9,
        CLEAR = 0xDD,
        SAVE = 0xDE,
        RESET = 0xDF
    };

    enum class Parameter : uint8_t {
        APP_ID = 0x01,
        CH_MASK = 0x02,
        RETRY_TX = 0x03,
        ROUTING_LAYER = 0x04,
        AP_ADDRESS = 0x05,
        UART_BAUDRATE = 0x06,
        ENCRYPTION = 0x07,
        OPTION_BITS = 0x08
    };

    enum class SavingStatus : uint8_t {
        SUCCEEDED = 0x01,
        FAILED = 0x00,
        SUCCEEDED_NO_MODIFICATIONS = 0x81,
        FAILED_NO_MODIFICATIONS = 0x80
    };

    inline void turnOnIndicator() {
        if (not (_indicatorPin < 0)) {
#if defined(ARDUINO_ARCH_RENESAS)
            digitalWrite(_indicatorPin, HIGH);
#else
            digitalWrite(_indicatorPin, LOW);
#endif
            _isIndicatorOn = true;
        }
    }

    inline void turnOffIndicator() {
        if (not (_indicatorPin < 0)) {
#if defined(ARDUINO_ARCH_RENESAS)
            digitalWrite(_indicatorPin, LOW);
#else
            digitalWrite(_indicatorPin, HIGH);
#endif
            _isIndicatorOn = false;
        }
    }

    inline void turnOnIndicatorFor(const uint16_t duration)  {
        _indicatorTimestamp = millis();
        _indicatorDuration = duration;
        turnOnIndicator();
    }

    inline void updateIndicator() {
        if (_isIndicatorOn and (millis() - _indicatorTimestamp > _indicatorDuration)) {
            turnOffIndicator();
            _indicatorTimestamp = UINT32_MAX;
            _indicatorDuration = 0;
        }
    }

    State processAscii(const uint8_t character, BarePacket& barePacket);

    inline void debugPrint(const char* const str) const {
        if (not Utils::IsWritable(_debugSerial)) { return; }
        _debugSerial->write(static_cast<uint8_t>('\n'));
        _debugSerial->print("[Debug] ");
        _debugSerial->print(str);
        _debugSerial->write(static_cast<uint8_t>('\n'));
    }

    inline void beginCommand() const {
        if (not Utils::IsWritable(_serial)) { return; }
        Utils::WriteBinary(_serial, static_cast<uint8_t>(':'));
        Utils::WriteInAscii(_serial, static_cast<uint8_t>(0xDB));
    }

    inline void endCommand() const {
        if (not Utils::IsWritable(_serial)) { return; }
        Utils::WriteBinary(_serial, static_cast<uint8_t>('X'));
        Utils::FlushTxBuffer(_serial);
    }

    inline bool checkForAck(const uint32_t timeout) const {
        if (not Utils::IsInitialized(_serial)) { return false; }
        return Utils::FindAscii(_serial, ":DBD001", timeout);
    }

    inline bool ensureParameterSet(const uint32_t timeout) const {
        if (not Utils::IsInitialized(_serial)) { return false; }
        return Utils::FindAscii(_serial, ":DBD3", timeout);
    }

    inline bool ensureParametersSaved(const uint32_t timeout, bool* const modified = nullptr) const {
        if (not Utils::IsInitialized(_serial)) { return false; }
        if (not Utils::FindAscii(_serial, ":DBDE", timeout)) { return false; }
        uint8_t status = _serial->read();
        if (not (modified == nullptr)) {
            if (status == static_cast<uint8_t>(SavingStatus::SUCCEEDED_NO_MODIFICATIONS)
                or status == static_cast<uint8_t>(SavingStatus::FAILED_NO_MODIFICATIONS)) {
                *(modified) = false;
            } else {
                *(modified) = true;
            }
        }
        if (status == static_cast<uint8_t>(SavingStatus::FAILED)
            or status == static_cast<uint8_t>(SavingStatus::FAILED_NO_MODIFICATIONS)) {
            return false;
        }
        return true;
    }

    inline bool ensureDeviceReset(const uint32_t timeout) const {
        if (not Utils::IsInitialized(_serial)) { return false; }
        return Utils::FindAscii(_serial, "!INF MW APP_WINGS(Parent)", timeout);
    }

    inline bool ensureSilentModeDisabled(const uint32_t timeout) const {
        if (not Utils::IsInitialized(_serial)) { return false; }
        return Utils::FindAscii(_serial, ":DBD91101", timeout);
    }

    HardwareSerial* _serial;

    int _indicatorPin;
    int _resetPin;
    int _programPin;

    bool _isIndicatorOn;
    uint32_t _indicatorTimestamp;
    uint16_t _indicatorDuration;

    uint8_t* _buffer;
    int _rxBufferSize;
    int _characterCount;
    uint16_t _checksum;
    uint16_t _timeout;
    uint32_t _latestTimestamp;

    HardwareSerial* _debugSerial;   // optional

public:
    //// AppTwelitePacketParser for App_Twelite
    inline void on(void (*callback)(const ParsedAppTwelitePacket& packet)) { _onAppTwelitePacket = callback; }
    //// AppIoPacketParser for App_IO
    inline void on(void (*callback)(const ParsedAppIoPacket& packet)) { _onAppIoPacket = callback; }
    //// AppAriaPacketParser for App_ARIA (ARIA mode)
    inline void on(void (*callback)(const ParsedAppAriaPacket& packet)) { _onAppAriaPacket = callback; }
    //// AppCuePacketParser for App_CUE (CUE mode)
    inline void on(void (*callback)(const ParsedAppCuePacket& packet)) { _onAppCuePacket = callback; }
    //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
    inline void on(void (*callback)(const ParsedAppPalOpenClosePacket& packet)) { _onAppPalOpenClosePacket = callback; }
    //// AppPalAmbPacketParser for App_PAL (AMB)
    inline void on(void (*callback)(const ParsedAppPalAmbPacket& packet)) { _onAppPalAmbPacket = callback; }
    //// AppPalMotPacketParser for App_PAL (MOT)
    inline void on(void (*callback)(const ParsedAppPalMotPacket& packet)) { _onAppPalMotPacket = callback; }
    //// AppUartAsciiPacketParser for App_Uart (Mode A)
    inline void on(void (*callback)(const ParsedAppUartAsciiPacket& packet)) { _onAppUartAsciiPacket = callback; }
    //// AppUartAsciiExtendedPacketParser for App_Uart (Mode A, extended)
    inline void on(void (*callback)(const ParsedAppUartAsciiExtendedPacket& packet)) { _onAppUartAsciiExtendedPacket = callback; }

    inline void on(void (*callback)(const BarePacket& packet)) { _onBarePacket = callback; }

    inline bool send(const uint8_t* const payload, const int payloadSize, const uint8_t checksum) {
        if (not Utils::IsWritable(_serial)) { return false; }
        turnOnIndicatorFor(10);
        Utils::WriteBinary(_serial, static_cast<uint8_t>(':'));
        Utils::WriteInAscii(_serial, payload, payloadSize);
        Utils::WriteInAscii(_serial, checksum);
        Utils::WriteBinary(_serial, static_cast<uint8_t>('\r'));
        Utils::WriteBinary(_serial, static_cast<uint8_t>('\n'));
        if (Utils::IsWritable(_debugSerial)) {
            Utils::WriteBinary(_debugSerial, static_cast<uint8_t>(':'));
            Utils::WriteInAscii(_debugSerial, payload, payloadSize);
            Utils::WriteInAscii(_debugSerial, checksum);
            Utils::WriteBinary(_debugSerial, static_cast<uint8_t>('\r'));
            Utils::WriteBinary(_debugSerial, static_cast<uint8_t>('\n'));
        }
        return true;
    }

    inline bool send(const uint8_t* const payload, const int payloadSize) {
        uint8_t checksum = 0;
        for (int i = 0; i < payloadSize; i++) {
            checksum += payload[i];
        }
        checksum = ~(checksum) + 1;
        return send(payload, payloadSize, checksum);
    }

    inline bool send(const uint8_t logicalId, const uint8_t commandId,
                     const uint8_t* const payload, const int payloadSize, const uint8_t checksum) {
        if (not Utils::IsWritable(_serial)) { return false; }
        Utils::WriteBinary(_serial, static_cast<uint8_t>(':'));
        Utils::WriteInAscii(_serial, logicalId);
        Utils::WriteInAscii(_serial, commandId);
        Utils::WriteInAscii(_serial, payload, payloadSize);
        Utils::WriteInAscii(_serial, checksum);
        Utils::WriteBinary(_serial, static_cast<uint8_t>('\r'));
        Utils::WriteBinary(_serial, static_cast<uint8_t>('\n'));
        if (Utils::IsWritable(_debugSerial)) {
            Utils::WriteBinary(_debugSerial, static_cast<uint8_t>(':'));
            Utils::WriteInAscii(_debugSerial, logicalId);
            Utils::WriteInAscii(_debugSerial, commandId);
            Utils::WriteInAscii(_debugSerial, payload, payloadSize);
            Utils::WriteInAscii(_debugSerial, checksum);
            Utils::WriteBinary(_debugSerial, static_cast<uint8_t>('\r'));
            Utils::WriteBinary(_debugSerial, static_cast<uint8_t>('\n'));
        }
        return true;
    }

    inline bool send(const uint8_t logicalId, const uint8_t commandId,
                     const uint8_t* const payload, const int payloadSize) {
        uint8_t checksum = 0;
        checksum += logicalId;
        checksum += commandId;
        for (int i = 0; i < payloadSize; i++) {
            checksum += payload[i];
        }
        checksum = ~(checksum) + 1;
        return send(logicalId, commandId, payload, payloadSize, checksum);
    }

    //// AppTweliteCommandSerializer for App_Twelite
    inline bool send(AppTweliteCommand& command) {
        constexpr int fixedSize = GetAppTweliteSerializedCommandPayloadSize();
        uint8_t payload[fixedSize];
        uint8_t checksum;
        if (AppTweliteCommandSerializer.serialize(&command, payload, fixedSize, &checksum)) {
            return send(payload, fixedSize, checksum);
        }
        return false;
    }
    //// AppIoCommandSerializer for App_Io
    inline bool send(AppIoCommand& command) {
        constexpr int fixedSize = GetAppIoSerializedCommandPayloadSize();
        uint8_t payload[fixedSize];
        uint8_t checksum;
        if (AppIoCommandSerializer.serialize(&command, payload, fixedSize, &checksum)) {
            return send(payload, fixedSize, checksum);
        }
        return false;
    }
    //// AppUartAsciiCommand for App_Uart (Mode A)
    inline bool send(AppUartAsciiCommand& command, uint32_t timeout = 0) {
        // There's no need to serialize AppUartAsciiCommand
        if (not send(command.u8DestinationLogicalId, command.u8CommandId,
                     command.u8Data, command.u16DataSize)) { return false; }
        if (timeout > 0) {
            bool foundOrNot = Utils::FindAscii(_serial, ":DBA1", timeout);
            Utils::FlushRxBuffer(_serial);
            return foundOrNot;
        } else {
            return true;
        }
        return false;
    }
    //// AppPalNoticeCommandSerializer for App_PalNotice
    inline bool send(AppPalNoticeCommand& command) {
        constexpr int fixedSize = GetAppPalNoticeSerializedCommandPayloadSize();
        uint8_t payload[fixedSize];
        uint8_t checksum;
        if (AppPalNoticeCommandSerializer.serialize(&command, payload, fixedSize, &checksum)) {
            return send(payload, fixedSize, checksum);
        }
        return false;
    }
    //// AppPalNoticeDetailedCommandSerializer for App_PalNotice
    inline bool send(AppPalNoticeDetailedCommand& command) {
        constexpr int fixedSize = GetAppPalNoticeSerializedDetailedCommandPayloadSize();
        uint8_t payload[fixedSize];
        uint8_t checksum;
        if (AppPalNoticeDetailedCommandSerializer.serialize(&command, payload, fixedSize, &checksum)) {
            return send(payload, fixedSize, checksum);
        }
        return false;
    }
    //// AppPalNoticeEventCommandSerializer for App_PalNotice
    inline bool send(AppPalNoticeEventCommand& command) {
        constexpr int fixedSize = GetAppPalNoticeSerializedEventCommandPayloadSize();
        uint8_t payload[fixedSize];
        uint8_t checksum;
        if (AppPalNoticeEventCommandSerializer.serialize(&command, payload, fixedSize, &checksum)) {
            return send(payload, fixedSize, checksum);
        }
        return false;
    }

private:
    //// AppTwelitePacketParser for App_Twelite
    void (*_onAppTwelitePacket)(const ParsedAppTwelitePacket& packet);
    //// AppIoPacketParser for App_IO
    void (*_onAppIoPacket)(const ParsedAppIoPacket& packet);
    //// AppAriaPacketParser for App_ARIA (ARIA mode)
    void (*_onAppAriaPacket)(const ParsedAppAriaPacket& packet);
    //// AppCuePacketParser for App_CUE (CUE mode)
    void (*_onAppCuePacket)(const ParsedAppCuePacket& packet);
    //// AppPalOpenClosePacketParser for App_PAL (OPENCLOSE)
    void (*_onAppPalOpenClosePacket)(const ParsedAppPalOpenClosePacket& packet);
    //// AppPalAmbPacketParser for App_PAL (AMB)
    void (*_onAppPalAmbPacket)(const ParsedAppPalAmbPacket& packet);
    //// AppPalMotPacketParser for App_PAL (MOT)
    void (*_onAppPalMotPacket)(const ParsedAppPalMotPacket& packet);
    //// AppUartAsciiPacketParser for App_Uart (Mode A)
    void (*_onAppUartAsciiPacket)(const ParsedAppUartAsciiPacket& packet);
    //// AppUartAsciiExtendedPacketParser for App_Uart (Mode A, extended)
    void (*_onAppUartAsciiExtendedPacket)(const ParsedAppUartAsciiExtendedPacket& packet);

    void (*_onBarePacket)(const BarePacket& packet);
};
}

extern mwings::MWings Twelite;

#endif  // MWINGS_H
