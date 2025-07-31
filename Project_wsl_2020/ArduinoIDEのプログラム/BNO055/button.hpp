#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#include <Arduino.h>

typedef enum {
  BUTTON_PULLDOWN = 0,
  BUTTON_PULLUP = 1
} Button_Type;

class Button {
public:
  Button(int pin, Button_Type bt) {
    pinMode(pin, bt == Button_Type::BUTTON_PULLDOWN ? INPUT_PULLDOWN : INPUT_PULLUP);
    _pin = pin;
    _bt = bt;
    _push_time = 0;
  }

  void loop() {
    _value = digitalRead(_pin);

    if (_value == !(int)_bt) {
      _push_time += 1;
    } else {
      if (_push_time <= 0) {
        _push_time = 0;
      } else {
        _push_time = -1;
      }
    }
  }

  bool isPushed() {
    return _push_time == 1;
  }

  bool isPushing() {
    return _push_time > 1;
  }

  bool isReleased() {
    return _push_time == -1;
  }

private:
  int _value;
  int _pin;
  int _push_time;
  Button_Type _bt;
};

#endif