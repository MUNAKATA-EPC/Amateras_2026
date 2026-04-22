#include "uiControllObject.hpp"

static void _drawCheckerOverlay(LGFX_Sprite &canvas, int x, int y, int w, int h)
{
    for (int row = 0; row < h; row++)
        for (int col = (row % 2); col < w; col += 2)
            canvas.drawPixel(x + col, y + row, TFT_BLACK);
}

uiImageButton::uiImageButton(int x, int y, int targetW, int targetH,
                             int imgW, int imgH, const char *jpgPathNormal)
    : _x(x), _y(y), _w(targetW), _h(targetH),
      _imgW(imgW), _imgH(imgH), _pathNormal(jpgPathNormal) {}

uiImageButton::~uiImageButton() { _spriteNormal.deleteSprite(); }

bool uiImageButton::init()
{
    if (!_pathNormal || _pathNormal[0] == '\0')
        return false;
    float scaleX = (float)_w / _imgW;
    float scaleY = (float)_h / _imgH;

    _spriteNormal.setColorDepth(8);
    if (!_spriteNormal.createSprite(_w, _h))
        return false;

    File file = SPIFFS.open(_pathNormal, "r");
    if (!file)
        return false;

    bool result = _spriteNormal.drawJpg(&file, 0, 0, 0, 0, 0, 0, scaleX, scaleY);
    file.close();
    return result;
}

void uiImageButton::update(const mouse_t &mouse)
{
    _prevPushing = _pushing;
    _pushing = _hitTest(mouse.posi) && mouse.pushing;
    _pressed_moment = (!_prevPushing && _pushing);
    _released_moment = (_prevPushing && !_pushing);
}

void uiImageButton::draw(LGFX_Sprite &canvas)
{
    _spriteNormal.pushSprite(&canvas, _x, _y);
    if (_pushing)
        _drawCheckerOverlay(canvas, _x, _y, _w, _h);
}

bool uiImageButton::_hitTest(const posi_t &posi) const
{
    return (posi.x >= _x && posi.x < _x + _w && posi.y >= _y && posi.y < _y + _h);
}

uiImageCycleButton::uiImageCycleButton(int x, int y, int targetW, int targetH,
                                       int imgW, int imgH,
                                       int cycleCount, const char *const jpgPaths[])
    : _x(x), _y(y), _w(targetW), _h(targetH), _imgW(imgW), _imgH(imgH),
      _cycleCount(cycleCount < UI_CYCLE_MAX ? cycleCount : UI_CYCLE_MAX)
{
    for (int i = 0; i < _cycleCount; i++)
        _paths[i] = jpgPaths[i];
}

uiImageCycleButton::~uiImageCycleButton()
{
    if (_sprites)
    {
        for (int i = 0; i < _cycleCount; i++)
            _sprites[i].deleteSprite();
        delete[] _sprites;
        _sprites = nullptr;
    }
}

bool uiImageCycleButton::init()
{
    float scaleX = (float)_w / _imgW;
    float scaleY = (float)_h / _imgH;

    _sprites = new LGFX_Sprite[_cycleCount];
    if (!_sprites)
        return false;

    for (int i = 0; i < _cycleCount; i++)
    {
        _sprites[i].setColorDepth(8);
        if (!_sprites[i].createSprite(_w, _h))
            return false;

        File file = SPIFFS.open(_paths[i], "r");
        if (!file)
            return false;

        bool ok = _sprites[i].drawJpg(&file, 0, 0, 0, 0, 0, 0, scaleX, scaleY);
        file.close();
        if (!ok)
            return false;
    }
    return true;
}

void uiImageCycleButton::update(const mouse_t &mouse)
{
    _prevPushing = _pushing;
    _pushing = _hitTest(mouse.posi) && mouse.pushing;
    _pressed_moment = (!_prevPushing && _pushing);
    _released_moment = (_prevPushing && !_pushing);

    if (_pressed_moment)
        _index = (_index + 1) % _cycleCount;
}

void uiImageCycleButton::draw(LGFX_Sprite &canvas)
{
    if (!_sprites)
        return;
    _sprites[_index].pushSprite(&canvas, _x, _y);
    if (_pushing)
        _drawCheckerOverlay(canvas, _x, _y, _w, _h);
}

bool uiImageCycleButton::_hitTest(const posi_t &posi) const
{
    return (posi.x >= _x && posi.x < _x + _w && posi.y >= _y && posi.y < _y + _h);
}

uiButton::uiButton(int x, int y, int w, int h,
                   uint32_t bgColor, const char *label, uint32_t textColor)
    : _x(x), _y(y), _w(w), _h(h),
      _bgColor(bgColor), _label(label), _textColor(textColor) {}

void uiButton::update(const mouse_t &mouse)
{
    _prevPushing = _pushing;
    _pushing = _hitTest(mouse.posi) && mouse.pushing;
    _pressed_moment = (!_prevPushing && _pushing);
    _released_moment = (_prevPushing && !_pushing);
}

void uiButton::draw(LGFX_Sprite &canvas)
{
    // 指定色での背景塗り（外形線のdrawRectを削除）
    canvas.fillRect(_x, _y, _w, _h, _bgColor);

    // 中央テキスト
    if (_label && _label[0] != '\0')
    {
        canvas.setTextColor(_textColor);
        canvas.setTextDatum(textdatum_t::middle_center);
        canvas.drawString(_label, _x + _w / 2, _y + _h / 2);
    }

    // 押下エフェクト（最後に描画）
    if (_pushing)
        _drawCheckerOverlay(canvas, _x, _y, _w, _h);
}

bool uiButton::_hitTest(const posi_t &posi) const
{
    return (posi.x >= _x && posi.x < _x + _w && posi.y >= _y && posi.y < _y + _h);
}

uiBitmapButton::uiBitmapButton(int x, int y, int w, int h,
                               const uint8_t *bitmap, int bmpW, int bmpH,
                               uint32_t bitmapColor, uint32_t bgColor)
    : _x(x), _y(y), _w(w), _h(h),
      _bitmap(bitmap), _bmpW(bmpW), _bmpH(bmpH),
      _bitmapColor(bitmapColor), _bgColor(bgColor) {}

void uiBitmapButton::update(const mouse_t &mouse)
{
    _prevPushing     = _pushing;
    _pushing         = _hitTest(mouse.posi) && mouse.pushing;
    _pressed_moment  = (!_prevPushing && _pushing);
    _released_moment = (_prevPushing  && !_pushing);
}

void uiBitmapButton::draw(LGFX_Sprite &canvas)
{
    // ① ボタン背景を塗る
    canvas.fillRect(_x, _y, _w, _h, _bgColor);

    // ② ビットマップをボタン中央に配置して描画
    //    ビットマップがボタンより小さい場合はセンタリング、大きければクリッピング
    int offsetX = _x + (_w - _bmpW) / 2;
    int offsetY = _y + (_h - _bmpH) / 2;

    canvas.drawBitmap(offsetX, offsetY, _bitmap, _bmpW, _bmpH,
                      _bitmapColor, _bgColor);

    // ③ 押下エフェクト（最後に重ねる）
    if (_pushing)
        _drawCheckerOverlay(canvas, _x, _y, _w, _h);
}

bool uiBitmapButton::_hitTest(const posi_t &posi) const
{
    return (posi.x >= _x && posi.x < _x + _w &&
            posi.y >= _y && posi.y < _y + _h);
}

uiCycleButton::uiCycleButton(int x, int y, int w, int h, int numStates,
                             const uint32_t *bgColors, const char **labels, const uint32_t *textColors)
    : _x(x), _y(y), _w(w), _h(h), _numStates(numStates)
{
    if (_numStates > UI_CYCLE_MAX)
        _numStates = UI_CYCLE_MAX;
    for (int i = 0; i < _numStates; i++)
    {
        _bgColors[i] = bgColors[i];
        _labels[i] = labels[i];
        _textColors[i] = textColors[i];
    }
}

void uiCycleButton::update(const mouse_t &mouse)
{
    _pushing = _hitTest(mouse.posi) && mouse.pushing;
    if (_prevPushing && !_pushing && _hitTest(mouse.posi))
    {
        _currentIndex = (_currentIndex + 1) % _numStates;
    }
    _prevPushing = _pushing;
}

void uiCycleButton::draw(LGFX_Sprite &canvas)
{
    canvas.fillRect(_x, _y, _w, _h, _bgColors[_currentIndex]);

    canvas.setTextDatum(textdatum_t::middle_center);
    canvas.setTextColor(_textColors[_currentIndex]);
    canvas.drawString(_labels[_currentIndex], _x + _w / 2, _y + _h / 2);

    if (_pushing)
        _drawCheckerOverlay(canvas, _x, _y, _w, _h);
}

bool uiCycleButton::_hitTest(const posi_t &posi) const
{
    return (posi.x >= _x && posi.x < _x + _w && posi.y >= _y && posi.y < _y + _h);
}

uiToggle::uiToggle(int x, int y, int w, int h,
                   uint32_t borderColor,
                   uint32_t innerBgColor,
                   uint32_t onColor,  const char *onLabel,  uint32_t onTextColor,
                   uint32_t offColor, const char *offLabel, uint32_t offTextColor,
                   bool initialState,
                   int borderWidth)
    : _x(x), _y(y), _w(w), _h(h), _borderWidth(borderWidth), // 初期化リストの順番も合わせる
      _borderColor(borderColor), _innerBgColor(innerBgColor),
      _onColor(onColor), _offColor(offColor),
      _onLabel(onLabel), _offLabel(offLabel),
      _onTextColor(onTextColor), _offTextColor(offTextColor),
      _isOn(initialState) {}

void uiToggle::update(const mouse_t &mouse)
{
    _pushing = _hitTest(mouse.posi) && mouse.pushing;
    _pressed_moment = (!_prevPushing && _pushing);
    _released_moment = (_prevPushing && !_pushing);
    _toggled_moment = false;

    if (_released_moment && _hitTest(mouse.posi))
    {
        _isOn = !_isOn;
        _toggled_moment = true;
    }
    _prevPushing = _pushing;
}

void uiToggle::draw(LGFX_Sprite &canvas)
{
    int bw = _borderWidth;
    canvas.fillRect(_x, _y, _w, _h, _borderColor);

    int innerX = _x + bw;
    int innerY = _y + bw;
    int innerW = _w - (bw * 2);
    int innerH = _h - (bw * 2);
    canvas.fillRect(innerX, innerY, innerW, innerH, _innerBgColor);

    int topH = innerH / 2;
    int bottomH = innerH - topH;

    int centerX = _x + (_w / 2);
    int centerY_ON  = innerY + (topH / 2);
    int centerY_OFF = innerY + topH + (bottomH / 2);

    if (_isOn)
    {
        canvas.fillRect(innerX, innerY, innerW, topH, _onColor);
        _drawTextCentered(canvas, _onLabel, centerX, centerY_ON, _onTextColor);

        _drawTextCentered(canvas, _offLabel, centerX, centerY_OFF, _borderColor);
    }
    else
    {
        canvas.fillRect(innerX, innerY + topH, innerW, bottomH, _offColor);
        _drawTextCentered(canvas, _offLabel, centerX, centerY_OFF, _offTextColor);

        _drawTextCentered(canvas, _onLabel, centerX, centerY_ON, _borderColor);
    }

    if (_pushing)
        _drawCheckerOverlay(canvas, _x, _y, _w, _h);
}


void uiToggle::_drawTextCentered(LGFX_Sprite &canvas, const char *text, int cx, int cy, uint32_t textColor)
{
    canvas.setTextDatum(textdatum_t::middle_center);
    canvas.setTextColor(textColor);
    canvas.drawString(text, cx, cy);
}

bool uiToggle::_hitTest(const posi_t &posi) const
{
    return (posi.x >= _x && posi.x < _x + _w && posi.y >= _y && posi.y < _y + _h);
}