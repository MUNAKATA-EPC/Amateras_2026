#include <Arduino.h>
#include "USE_L3GD20H.h"

String StrL3GD20H_Data = "";

void L3GD20Hbegin(int Serial_speed)
{
    Serial5.begin(Serial_speed);
}

int L3GD20Hread()
{
    if (Serial5.available())
    {
        StrL3GD20H_Data = Serial5.readStringUntil('\n');
    }

    int L3GDdata = StrL3GD20H_Data.toInt();

    if (L3GDdata > 0 && L3GDdata <= 21)
    {
        L3GDdata = map(L3GDdata, 0, 21, 0, 90);
    }
    else if (L3GDdata > 0 && L3GDdata <= 36)
    {
       L3GDdata = map(L3GDdata, 21, 36, 90, 180);
    }
    else if (L3GDdata > 0 && L3GDdata <= 52)
    {
        L3GDdata = map(L3GDdata, 36, 52, 180, 270);
    }
    else if (L3GDdata > 0 && L3GDdata <= 73)
    {
        L3GDdata = map(L3GDdata, 52, 73, 270, 360);
    }
    else
    {
        //L3GDdata = map(L3GDdata, 52, 360, 270, 360);
    }

    return (L3GDdata);
}
