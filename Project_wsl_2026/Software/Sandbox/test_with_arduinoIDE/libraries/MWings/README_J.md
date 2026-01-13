<a href="https://mono-wireless.com/jp/index.html">
    <img src="https://mono-wireless.com/common/images/logo/logo-land.svg" alt="mono wireless logo" title="MONO WIRELESS" align="right" height="60" />
</a>

# MWings

**Mono Wireless TWELITE Wings API for 32-bit Arduinos**

[![arduino-library-badge](https://www.ardu-badge.com/badge/MWings.svg?)](https://www.ardu-badge.com/MWings) [![Arduino Lint](https://github.com/monowireless/mwings_arduino/actions/workflows/arduino-lint.yml/badge.svg?branch=main&event=push)](https://github.com/monowireless/mwings_arduino/actions/workflows/arduino-lint.yml) [![MW-OSSLA](https://img.shields.io/badge/License-MW--OSSLA-e4007f)](LICENSE.md)

## 目次

- [概要](#概要)
- [インストール](#インストール)
- [要件](#要件)
- [機能](#機能)
- [使い方](#使い方)
- [API リファレンス](#api-リファレンス)
- [ライセンス](#ライセンス)

## 概要

MWings は、TWELITE 無線モジュールと通信するためのライブラリです。[TWELITE SPOT](https://mono-wireless.com/jp/products/twelite-spot/index.html) シリーズに最適です。

このライブラリでは、接続した TWELITE 親機を通じて、TWELITE 子機からのパケット受信や TWELITE 子機へのコマンド送信を行うことができます。

## インストール

Arduino IDE のライブラリマネージャを開き、`MWings` と検索してからインストールボタンを押してください。

詳しい方法は [MWings ライブラリの導入](https://twelite.net/manuals/twelite-spot/setup-for-development/arduino-ide-1x/add-mwings-library.html) をご覧ください。

## 要件

<!-- 32ビット Arduino ボード（esp32, esp8266, stm32, sam, rp2040）向けに実装しています。ESP32 シリーズでテストしています。 -->
TWELITE SPOT などの ESP32 モジュール および Arduino UNO R4 シリーズ向けに実装しています。

**8ビット Arduino ボード（UNO R3など）はサポートしていません**。

ボードに接続する **TWELITE 親機には、App_Wings (>= v1.3.0)** の使用を強く推奨します。

## 機能

### パケットの受信と解釈

- App_Twelite
  - デジタル入力の状態とアナログ入力の電圧値を取得します。
  - [TWELITE (SMD)](https://mono-wireless.com/jp/products/TWE-LITE/index.html) と [TWELITE DIP](https://mono-wireless.com/jp/products/TWE-Lite-DIP/index.html) にプリインストールされています。
  - 使用例:
    - [`monitor_spot_app_twelite.ino`](./examples/monitor_spot_app_twelite/monitor_spot_app_twelite.ino)
- App_ARIA (TWELITE ARIA mode)
  - 気温、湿度と磁石の状態を取得します。
  - [TWELITE ARIA](https://mono-wireless.com/jp/products/twelite-aria/index.html) にプリインストールされています。
  - 使用例：
    - [`monitor_spot_app_aria.ino`](./examples/monitor_spot_app_aria/monitor_spot_app_aria.ino)
    - [`plotter_spot_app_aria.ino`](./examples/plotter_spot_app_aria/plotter_spot_app_aria.ino)
- App_CUE (TWELITE CUE mode)
  - 加速度と磁石の状態を取得します。
  - [TWELITE CUE](https://mono-wireless.com/jp/products/twelite-cue/index.html) にプリインストールされています。
  - 使用例：
    - [`monitor_spot_app_cue.ino`](./examples/monitor_spot_app_cue/monitor_spot_app_cue.ino)
- App_PAL (AMBIENT)
  - 気温、湿度と照度を取得します。
  - [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) シリーズにプリインストールされています (App_PAL) 。
  - 使用例：
    - [`monitor_spot_app_pal_amb.ino`](./examples/monitor_spot_app_pal_amb/monitor_spot_app_pal_amb.ino)
    - [`plotter_spot_app_pal_amb.ino`](./examples/plotter_spot_app_pal_amb/plotter_spot_app_pal_amb.ino)
- App_PAL (MOTION) / App_CUE (MOTION PAL mode)
  - 加速度を取得します。
  - [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) シリーズ (App_PAL) および [TWELITE CUE](https://mono-wireless.com/jp/products/twelite-cue/index.html) シリーズ (App_CUE) にプリインストールされています。
  - 使用例：
    - [`monitor_spot_app_pal_mot.ino`](./examples/monitor_spot_app_pal_mot/monitor_spot_app_pal_mot.ino)
    - [`plotter_spot_app_pal_mot.ino`](./examples/plotter_spot_app_pal_mot/plotter_spot_app_pal_mot.ino)
- App_PAL (OPENCLOSE) / App_CUE (OPENCLOSE PAL mode) / App_ARIA (OPENCLOSE PAL mode)
  - 磁石の状態を取得します。
  - [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) シリーズ (App_PAL) 、 [TWELITE ARIA](https://mono-wireless.com/jp/products/twelite-aria/index.html) シリーズ (App_ARIA) および [TWELITE CUE](https://mono-wireless.com/jp/products/twelite-cue/index.html) シリーズ (App_CUE) にプリインストールされています。
  - 使用例：
    - [`monitor_spot_app_pal_openclose.ino`](./examples/monitor_spot_app_pal_openclose/monitor_spot_app_pal_openclose.ino)
- App_Uart (Mode A, アスキー書式の簡易形式または拡張形式)
  - TWELTIE 子機で受信したシリアルデータを取得します。
  - [TWELITE UART](https://mono-wireless.com/jp/products/twelite-uart/index.html) にプリインストールされています。
  - 使用例：
    - [`monitor_spot_app_uart_ascii.ino`](./examples/monitor_spot_app_uart_ascii/monitor_spot_app_uart_ascii.ino)
- App_IO
  - デジタル入力の状態を取得します。
  - 使用例：
    - [`monitor_spot_app_io.ino`](./examples/monitor_spot_app_io/monitor_spot_app_io.ino)

### パケットの受信

- すべての TWELITE
  - アスキー形式のシリアル入力から、ペイロードのバイナリデータを取得します（`:`で始まり、`CRLF`で終わるもの）。
  - 使用例：
    - [`monitor_spot_any.ino`](./examples/monitor_spot_any/monitor_spot_any.ino)

### パケットの生成と送信

- App_Twelite
  - デジタル出力の状態と PWM のデューティ比を設定します。
  - [TWELITE (SMD)](https://mono-wireless.com/jp/products/TWE-LITE/index.html) および [TWELITE DIP](https://mono-wireless.com/jp/products/TWE-Lite-DIP/index.html) にプリインストールされています。
  - 使用例：
    - [`commander_spot_app_twelite.ino`](./examples/commander_spot_app_twelite/commander_spot_app_twelite.ino)
- App_IO
  - デジタル出力の状態を設定します。
  - 使用例：
    - [`commander_spot_app_io.ino`](./examples/commander_spot_app_io/commander_spot_app_io.ino)
- App_Uart (Mode A, アスキー書式の簡易形式)
  - TWELITE 子機から送信するシリアルデータを設定します。
  - [TWELITE UART](https://mono-wireless.com/jp/products/twelite-uart/index.html) にプリインストールされています。
  - 使用例：
    - [`commander_spot_app_uart_ascii.ino`](./examples/commander_spot_app_uart_ascii/commander_spot_app_uart_ascii.ino)
- App_PAL (NOTICE)
  - フルカラー LED の点灯状態を設定します。
  - [TWELITE PAL](https://mono-wireless.com/jp/products/twelite-pal/index.html) シリーズにプリインストールされています (App_PAL) 。
  - 使用例：
    - [`commander_spot_app_pal_notice.ino`](./examples/commander_spot_app_pal_notice/commander_spot_app_pal_notice.ino)
    - [`commander_spot_app_pal_notice_detailed.ino`](./examples/commander_spot_app_pal_notice_detailed/commander_spot_app_pal_notice_detailed.ino)
    - [`commander_spot_app_pal_notice_event.ino`](./examples/commander_spot_app_pal_notice_event/commander_spot_app_pal_notice_event.ino)

### パケットの送信

- すべての TWELITE
  - ペイロードのバイナリデータから、アスキー形式のシリアル出力を行います（`:`で始まり、`CRLF`で終わるもの）。
  - 使用例：
    - [`commander_spot_app_twelite_manually.ino`](./examples/commander_spot_app_twelite_manually/commander_spot_app_twelite_manually.ino)
    - [`commander_spot_app_twelite_fully_manually.ino`](./examples/commander_spot_app_twelite_fully_manually/commander_spot_app_twelite_fully_manually.ino)

## 使い方

- [TWELITE 子機からのデータ受信](https://twelite.net/start-guides/twelite-spot/receive-from-cue.html) をご覧ください。

## API リファレンス

- [データ型と手続きの一覧](https://twelite.net/api-references/mwings/arduino-32bit/latest/data-and-procedures.html)
- [mwings::MWings クラス](https://twelite.net/api-references/mwings/arduino-32bit/latest/classes/mwings.html)

## ライセンス

``` plain
Copyright (C) 2023-2024 Mono Wireless Inc. All Rights Reserved.
Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT).
```
