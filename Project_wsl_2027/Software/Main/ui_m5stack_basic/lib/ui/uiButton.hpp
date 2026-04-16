#pragma once

#include "ui.hpp"

class uiButton
{
public:
    // コンストラクタ
    // x, y      : ボタン左上の座標
    // w, h      : ボタンのサイズ
    // pngPath   : SPIFFSのPNGファイルパス（例: "/button.png"）、空文字でフォールバック描画
    uiButton(int x, int y, int w, int h, const char *pngPath = "");

    // 毎フレーム呼ぶ更新関数（uiUpdate内で呼ぶ）
    void update(const mouse_t &mouse);

    // 毎フレーム呼ぶ描画関数（update後に呼ぶ）
    void draw(LGFX_Sprite &canvas);

    // ゲッター
    bool isPushing();            // 押されている間 true
    bool isPressedMoment();      // 押された瞬間だけ true
    bool isReleasedMoment();     // 離された瞬間だけ true
    unsigned long pushingTime(); // 押されている時間をミリ秒で返す

private:
    int _x, _y, _w, _h;
    const char *_pngPath;

    bool _pushing;
    bool _prevPushing;
    bool _pressedMoment;
    bool _releasedMoment;
    unsigned long _pushStartMs;
    unsigned long _pushingTime;

    bool _hitTest(const posi_t &posi) const;
};
