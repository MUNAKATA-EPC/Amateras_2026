#include <Arduino.h>
#include "MyLibrary.h"

void setup()
{
    DSR1202begin();
    IRXIAObegin();
    LINEXIAObegin();
    SSD1306begin();
    SSD1306clear();
}

void loop()
{
    IRXIAOget();
    LINEXIAOget();
    
    SSD1306clear();
    SSD1306write(1, 0, 0, "IREdge:" + String(int(IRedge)) + ",Dist:" + String(int(IRdist)), false);
    SSD1306write(1, 0, 10, "LINEEdge:" + String(int(LINEedge)), false);
    SSD1306show();

    DSR1202move(BNO055deg, BNO055deg, -BNO055deg, -BNO055deg);

    delay(100);
}