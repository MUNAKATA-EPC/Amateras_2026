

#ifndef INC_ColorSensor_H
#define INC_ColorSensor_H

#include <arduino.h>

#define COLOR_SENSOR_DS_B   0
#define COLOR_SENSOR_DS_G   1
#define COLOR_SENSOR_DS_R   2

#define CLED1   4
#define CLED2   5
#define CLED3   7
#define CLED4   6
#define CLED5   3

#define CLED_ON 100;
#define CLED_OFF  0;

class ClassColorSensor
{
public:
    ClassColorSensor();  // Constractor
    void begin( void );
    void begin(word colorSensorintegratinTime_x10);
    void begin(word colorSensorintegratinTime_x10, byte gain);

    /*******************************************************/
    /* for ColorSensor                                     */
    /*******************************************************/
private:
    word rawClear;
    word rawRed;
    word rawGreen;
    word rawBlue;

    word initClear;

    word initH;

    float calR;
    float calG;
    float calB;

    byte convR;
    byte convG;
    byte convB;

    word convH;
    word convS;
    word convV;

    void colorSensorInit( void );
    void colorSensorInit(word colorSensorintegratinTime_x10);
    void colorSensorInit(word colorSensorintegratinTime_x10, byte gain);

    byte colorSensorRead8( byte reg );
    word colorSensorRead16( byte reg );
    void colorSensorWrite8( byte reg, byte val );

public:
    void colorWaitTime(bool enable, word time);
    void colorIntegrationTime(word time);

    boolean colorRead( void );
    word colorRawClear(void);
    word colorRawRed(void);
    word colorRawGreen(void);
    word colorRawBlue(void);

    boolean colorRawCRGB( word &c, word &r, word &g, word &b );

    word colorClear(void);
    word colorRed(void);
    word colorGreen(void);
    word colorBlue(void);

    boolean colorBlack();
    boolean colorIsBlack(word th);

    /*******************************************************/
    /* for LED control                                     */
    /*******************************************************/
private:
    unsigned short ledOutVal;
    void ledInit( void );
    void led( void );

public:
    void ledIref( byte cm, byte hc, byte cc );

    void ledBrightness( int no, byte brightness );
    void ledOnOff( int no, boolean bOn );

    void ledOff( int no );
    void ledOn( int no );
    void led( int no ); // toggle on/off

    void led( int no, boolean bOn, byte brightness );
    void led( int no, byte brightness );

    void ledRGB( byte r, byte g, byte b );
    void ledRGB( boolean bOn );
    void ledRGB( boolean bOn, byte r, byte g, byte b );

    void hue2rgb( const word h, byte &r, byte &g, byte &b);

    void convertRGB(const word h);
    word convertRed();
    word convertGreen();
    word convertBlue();

    void rgb2hsv(const word r, const word g, const word b, word &h, word &s, word &v);
    void convertHSV(const word r, const word g, const word b);
    word convertH();
    word convertS();
    word convertV();

};

extern ClassColorSensor ColorSensor;

#endif /* INC_ColorSensor_H */

