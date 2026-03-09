<a href="https://mono-wireless.com/jp/index.html">
    <img src="https://mono-wireless.com/common/images/logo/logo-land.svg" alt="mono wireless logo" title="MONO WIRELESS" align="right" height="60" />
</a>

# MWings

**Mono Wireless TWELITE Wings API for 32-bit Arduinos**

[![arduino-library-badge](https://www.ardu-badge.com/badge/MWings.svg?)](https://www.ardu-badge.com/MWings) [![Arduino Lint](https://github.com/monowireless/mwings_arduino/actions/workflows/arduino-lint.yml/badge.svg?branch=main&event=push)](https://github.com/monowireless/mwings_arduino/actions/workflows/arduino-lint.yml) [![MW-OSSLA](https://img.shields.io/badge/License-MW--OSSLA-e4007f)](LICENSE.md)

日本語版は[こちら](README_J.md)

## Contents

- [About](#about)
- [Installation](#installation)
- [Requirements](#requirements)
- [Features](#features)
- [Getting Started](#getting-started)
- [API reference](#api-reference)
- [License](#license)

## About

MWings is a library that communicate with TWELITE wireless modules.

This library is suitable for [TWELITE SPOT](https://mono-wireless.com/jp/products/twelite-spot/index.html) series.

You can receive packets from TWELITE child devices and send commands to TWELITE child devices through the connected TWELITE parent device.

## Installation

Open the Library Manager from the Arduino IDE, then search for `MWings` and install it.

See details for [Installing MWings Library](https://twelite.net/manuals/twelite-spot/setup-for-development/arduino-ide-1x/add-mwings-library.html).

## Requirements

Built for ESP32 modules (e.g. TWELITE SPOT) and Arduino UNO R4 series.
<!-- Built for 32-bit Arduino boards (esp32, esp8266, stm32, sam, rp2040). Tested on ESP32 series. -->

**8-bit Arduino boards (like UNO R3) are NOT supported.**

It is strongly recommended to use **App_Wings (>= v1.3.0) for the TWELITE parent device** connected with the board.

## Features

### Receive and Parse Packets from

- App_Twelite
  - Get digital and analog input states.
  - Pre-installed on [TWELITE (SMD)](https://mono-wireless.com/jp/products/TWE-LITE/index.html) and [TWELITE DIP](https://mono-wireless.com/jp/products/TWE-Lite-DIP/index.html) series.
  - Example:
    - [`monitor_spot_app_twelite.ino`](./examples/monitor_spot_app_twelite/monitor_spot_app_twelite.ino)
- App_ARIA (TWELITE ARIA mode)
  - Get air temperature, relative humidity and magnet state.
  - Pre-installed on [TWELITE ARIA](https://mono-wireless.com/jp/products/twelite-aria/index.html) series.
  - Examples:
    - [`monitor_spot_app_aria.ino`](./examples/monitor_spot_app_aria/monitor_spot_app_aria.ino)
    - [`plotter_spot_app_aria.ino`](./examples/plotter_spot_app_aria/plotter_spot_app_aria.ino)
- App_CUE (TWELITE CUE mode)
  - Get acceleration and magnet state.
  - Pre-installed on [TWELITE CUE](https://mono-wireless.com/jp/products/twelite-cue/index.html) series.
  - Example:
    - [`monitor_spot_app_cue.ino`](./examples/monitor_spot_app_cue/monitor_spot_app_cue.ino)
- App_PAL (AMBIENT)
  - Get air temperature, relative humidity and illuminance.
  - Pre-installed on [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) series (App_PAL).
  - Examples:
    - [`monitor_spot_app_pal_amb.ino`](./examples/monitor_spot_app_pal_amb/monitor_spot_app_pal_amb.ino)
    - [`plotter_spot_app_pal_amb.ino`](./examples/plotter_spot_app_pal_amb/plotter_spot_app_pal_amb.ino)
- App_PAL (MOTION) / App_CUE (MOTION PAL mode)
  - Get acceleration.
  - Pre-installed on [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) series (App_PAL) and [TWELITE CUE](https://mono-wireless.com/jp/products/twelite-cue/index.html) series (App_CUE).
  - Examples:
    - [`monitor_spot_app_pal_mot.ino`](./examples/monitor_spot_app_pal_mot/monitor_spot_app_pal_mot.ino)
    - [`plotter_spot_app_pal_mot.ino`](./examples/plotter_spot_app_pal_mot/plotter_spot_app_pal_mot.ino)
- App_PAL (OPENCLOSE) / App_CUE (OPENCLOSE PAL mode) / App_ARIA (OPENCLOSE PAL mode)
  - Get magnet state.
  - Pre-installed on [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) series (App_PAL), [TWELITE ARIA](https://mono-wireless.com/jp/products/twelite-aria/index.html) series (App_ARIA) and [TWELITE CUE](https://mono-wireless.com/jp/products/twelite-cue/index.html) series (App_CUE).
  - Example:
    - [`monitor_spot_app_pal_openclose.ino`](./examples/monitor_spot_app_pal_openclose/monitor_spot_app_pal_openclose.ino)
- App_Uart (Mode A, Simplified or Extended ASCII format)
  - Get serial data received on TWELITE child devices.
  - Pre-installed on [TWELITE UART](https://mono-wireless.com/jp/products/twelite-uart/index.html) series.
  - Example:
    - [`monitor_spot_app_uart_ascii.ino`](./examples/monitor_spot_app_uart_ascii/monitor_spot_app_uart_ascii.ino)
- App_IO
  - Get digital input states.
  - Example:
    - [`monitor_spot_app_io.ino`](./examples/monitor_spot_app_io/monitor_spot_app_io.ino)

### Receive Packets from

- Any TWELITE
  - Get binary payload with ASCII-formatted serial input (starts with `:` and ends with `CRLF`).
  - Example:
    - [`monitor_spot_any.ino`](./examples/monitor_spot_any/monitor_spot_any.ino)

### Serialize and Send Commands to

- App_Twelite
  - Set digital output states and PWM duty ratios.
  - Pre-installed on [TWELITE (SMD)](https://mono-wireless.com/jp/products/TWE-LITE/index.html) and [TWELITE DIP](https://mono-wireless.com/jp/products/TWE-Lite-DIP/index.html) series.
  - Example:
    - [`commander_spot_app_twelite.ino`](./examples/commander_spot_app_twelite/commander_spot_app_twelite.ino)
- App_IO
  - Set digital output states.
  - Example:
    - [`commander_spot_app_io.ino`](./examples/commander_spot_app_io/commander_spot_app_io.ino)
- App_Uart (Mode A, Simplified ASCII format)
  - Set serial data to send on TWELITE child devices.
  - Pre-installed on [TWELITE UART](https://mono-wireless.com/jp/products/twelite-uart/index.html) series.
  - Example:
    - [`commander_spot_app_uart_ascii.ino`](./examples/commander_spot_app_uart_ascii/commander_spot_app_uart_ascii.ino)
- App_PAL (NOTICE)
  - Set full-color LED states.
  - Pre-installed on [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) series (App_PAL).
  - Examples:
    - [`commander_spot_app_pal_notice.ino`](./examples/commander_spot_app_pal_notice/commander_spot_app_pal_notice.ino)
    - [`commander_spot_app_pal_notice_detailed.ino`](./examples/commander_spot_app_pal_notice_detailed/commander_spot_app_pal_notice_detailed.ino)
    - [`commander_spot_app_pal_notice_event.ino`](./examples/commander_spot_app_pal_notice_event/commander_spot_app_pal_notice_event.ino)

### Send Commands to

- Any TWELITE
  - Set some binary data with ASCII-formatted serial output (starts with `:` and ends with `CRLF`).
  - Examples:
    - [`commander_spot_app_twelite_manually.ino`](./examples/commander_spot_app_twelite_manually/commander_spot_app_twelite_manually.ino)
    - [`commander_spot_app_twelite_fully_manually.ino`](./examples/commander_spot_app_twelite_fully_manually/commander_spot_app_twelite_fully_manually.ino)

## Getting Started

- See [Receive data from the TWELITE child device](https://twelite.net/start-guides/twelite-spot/receive-from-cue.html).

## API reference

- [List of data types and procedures](https://twelite.net/api-references/mwings/arduino-32bit/latest/data-and-procedures.html)
- [Class mwings::MWings](https://twelite.net/api-references/mwings/arduino-32bit/latest/classes/mwings.html)

## License

``` plain
Copyright (C) 2023 Mono Wireless Inc. All Rights Reserved.
Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
```
