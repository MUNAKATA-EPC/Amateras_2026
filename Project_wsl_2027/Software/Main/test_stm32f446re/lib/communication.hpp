#include <Arduino.h>


class communication
{
    private:
        HardwareSerial& _serial;
    public:
        communication(HardwareSerial& serial);
        void begin(unsigned long baud);
};