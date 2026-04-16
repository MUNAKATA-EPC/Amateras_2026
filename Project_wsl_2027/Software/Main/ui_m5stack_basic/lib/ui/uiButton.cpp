#include "uiButton.hpp"

uiButton::uiButton(int x, int y, int w, int h, const char* pngPath)
    : _x(x), _y(y), _w(w), _h(h), _pngPath(pngPath),
      _pushing(false), _prevPushing(false),
      _pressedMoment(false), _releasedMoment(false),
      _pushStartMs(0), _pushingTime(0)
{
}

void uiButton::update(const mouse_t& mouse)
{
    _prevPushing = _pushing;

    // ヒットテスト：ボタン矩形内かつ押されている
    bool hit = _hitTest(mouse.posi);
    _pushing = hit && mouse.pushing;

    // 瞬間検出
    _pressedMoment  = (!_prevPushing && _pushing);
    _releasedMoment = (_prevPushing && !_pushing);

    // 押下時間の計算
    if (_pressedMoment)
    {
        _pushStartMs = millis();
    }

    if (_pushing)
    {
        _pushingTime = millis() - _pushStartMs;
    }
    else
    {
        _pushingTime = 0;
    }
}

void uiButton::draw(LGFX_Sprite& canvas)
{
    if (_pngPath && _pngPath[0] != '\0')
    {
        // PNG画像をボタンサイズにぴったり描画
        // 押下中は少し暗くするためにアルファブレンド的な処理をシミュレート
        canvas.drawPngFile(SPIFFS, _pngPath, _x, _y, _w, _h);

        // 押下中のビジュアルフィードバック：半透明の暗いオーバーレイ
        if (_pushing)
        {
            // 黒を薄く重ねて押下感を表現
            canvas.fillRect(_x, _y, _w, _h, canvas.color565(0, 0, 0));
            // ※ colorDepth 8bit環境ではアルファが使えないため、
            //   枠線で代替フィードバックも追加
            canvas.drawRect(_x, _y, _w, _h, TFT_WHITE);
        }
    }
    else
    {
        // PNGなし時のフォールバック描画
        uint16_t bgColor = _pushing
            ? canvas.color565(80, 80, 80)   // 押下中：濃いグレー
            : canvas.color565(50, 50, 180); // 通常：青

        canvas.fillRoundRect(_x, _y, _w, _h, 6, bgColor);
        canvas.drawRoundRect(_x, _y, _w, _h, 6, TFT_WHITE);
    }
}

bool uiButton::isPushing()
{
    return _pushing;
}

bool uiButton::isPressedMoment()
{
    return _pressedMoment;
}

bool uiButton::isReleasedMoment()
{
    return _releasedMoment;
}

unsigned long uiButton::pushingTime()
{
    return _pushingTime;
}

bool uiButton::_hitTest(const posi_t& posi) const
{
    return (posi.x >= _x && posi.x < _x + _w &&
            posi.y >= _y && posi.y < _y + _h);
}
