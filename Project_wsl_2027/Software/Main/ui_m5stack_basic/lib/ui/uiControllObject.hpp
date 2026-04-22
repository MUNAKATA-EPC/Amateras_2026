#pragma once

#include <Arduino.h>
#include <SPIFFS.h>
#include <LovyanGFX.hpp>
#include "uiMouse.hpp"

static constexpr int UI_CYCLE_MAX = 8;

class uiImageButton
{
public:
    uiImageButton(int x, int y, int targetW, int targetH,
                  int imgW, int imgH, const char *jpgPathNormal);
    ~uiImageButton();

    bool init();
    void update(const mouse_t &mouse);
    void draw(LGFX_Sprite &canvas);

    bool isPushing()        const { return _pushing; }
    bool isPressedMoment()  const { return _pressed_moment; }
    bool isReleasedMoment() const { return _released_moment; }

private:
    int _x, _y, _w, _h, _imgW, _imgH;
    const char *_pathNormal;
    LGFX_Sprite _spriteNormal;
    bool _pushing         = false;
    bool _prevPushing     = false;
    bool _pressed_moment  = false;
    bool _released_moment = false;
    bool _hitTest(const posi_t &posi) const;
};

class uiImageCycleButton
{
public:
    uiImageCycleButton(int x, int y, int targetW, int targetH,
                       int imgW, int imgH,
                       int cycleCount, const char *const jpgPaths[]);
    ~uiImageCycleButton();

    bool init();
    void update(const mouse_t &mouse);
    void draw(LGFX_Sprite &canvas);

    bool isPushing()        const { return _pushing; }
    bool isPressedMoment()  const { return _pressed_moment; }
    bool isReleasedMoment() const { return _released_moment; }
    int  currentIndex()     const { return _index; }

private:
    int _x, _y, _w, _h, _imgW, _imgH;
    int _cycleCount;
    const char *_paths[UI_CYCLE_MAX];
    LGFX_Sprite *_sprites = nullptr;
    int  _index           = 0;
    bool _pushing         = false;
    bool _prevPushing     = false;
    bool _pressed_moment  = false;
    bool _released_moment = false;
    bool _hitTest(const posi_t &posi) const;
};

class uiButton
{
public:
    uiButton(int x, int y, int w, int h,
             uint32_t bgColor, const char *label, uint32_t textColor);

    void update(const mouse_t &mouse);
    void draw(LGFX_Sprite &canvas);

    bool isPushing()        const { return _pushing; }
    bool isPressedMoment()  const { return _pressed_moment; }
    bool isReleasedMoment() const { return _released_moment; }

private:
    int _x, _y, _w, _h;
    uint32_t    _bgColor;
    const char *_label;
    uint32_t    _textColor;
    bool _pushing         = false;
    bool _prevPushing     = false;
    bool _pressed_moment  = false;
    bool _released_moment = false;
    bool _hitTest(const posi_t &posi) const;
};

class uiBitmapButton
{
public:
    uiBitmapButton(int x, int y, int w, int h,
                   const uint8_t *bitmap, int bmpW, int bmpH,
                   uint32_t bitmapColor, uint32_t bgColor);

    void update(const mouse_t &mouse);
    void draw(LGFX_Sprite &canvas);

    bool isPushing()        const { return _pushing; }
    bool isPressedMoment()  const { return _pressed_moment; }
    bool isReleasedMoment() const { return _released_moment; }

private:
    int _x, _y, _w, _h;
    const uint8_t *_bitmap;
    int _bmpW, _bmpH;
    uint32_t _bitmapColor;
    uint32_t _bgColor;
    bool _pushing         = false;
    bool _prevPushing     = false;
    bool _pressed_moment  = false;
    bool _released_moment = false;
    bool _hitTest(const posi_t &posi) const;
};

class uiCycleButton
{
public:
    uiCycleButton(int x, int y, int w, int h, int numStates,
                  const uint32_t* bgColors, const char** labels, const uint32_t* textColors);

    void update(const mouse_t &mouse);
    void draw(LGFX_Sprite &canvas);

    int getIndex() const { return _currentIndex; }

private:
    int _x, _y, _w, _h;
    int _numStates;
    int _currentIndex    = 0;
    bool _pushing        = false;
    bool _prevPushing    = false;

    // 状態ごとのデータを保持（最大 UI_CYCLE_MAX = 8）
    uint32_t _bgColors[UI_CYCLE_MAX];
    const char* _labels[UI_CYCLE_MAX];
    uint32_t _textColors[UI_CYCLE_MAX];

    bool _hitTest(const posi_t &posi) const;
};

class uiToggle
{
public:
    uiToggle(int x, int y, int w, int h,
             uint32_t borderColor,
             uint32_t innerBgColor,
             uint32_t onColor,  const char *onLabel,  uint32_t onTextColor,
             uint32_t offColor, const char *offLabel, uint32_t offTextColor,
             bool initialState = false,
             int borderWidth = 2);

    void update(const mouse_t &mouse);
    void draw(LGFX_Sprite &canvas);

    bool isOn()             const { return _isOn; }
    bool isPushing()        const { return _pushing; }
    bool isPressedMoment()  const { return _pressed_moment; }
    bool isReleasedMoment() const { return _released_moment; }
    bool isToggledMoment()  const { return _toggled_moment; }

private:
    int _x, _y, _w, _h;
    int _borderWidth;
    uint32_t    _borderColor, _innerBgColor;
    uint32_t    _onColor,  _offColor;
    const char *_onLabel,  *_offLabel;
    uint32_t    _onTextColor, _offTextColor;
    bool _isOn            = false;
    bool _pushing         = false;
    bool _prevPushing     = false;
    bool _pressed_moment  = false;
    bool _released_moment = false;
    bool _toggled_moment  = false;

    bool _hitTest(const posi_t &posi) const;
    void _drawTextCentered(LGFX_Sprite &canvas, const char *text, int cx, int cy, uint32_t textColor);
};