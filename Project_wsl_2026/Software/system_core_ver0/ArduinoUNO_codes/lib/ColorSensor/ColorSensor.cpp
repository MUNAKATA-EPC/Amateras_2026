#include "arduino.h"
//#include "../Wire/Wire.h"
#include "Wire.h"
#include "ColorSensor.h"

// Preinstantiate Objects //////////////////////////////////////////////////////
ClassColorSensor ColorSensor = ClassColorSensor();
static TwoWire i2c = TwoWire();
////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************
 *
 */
ClassColorSensor::ClassColorSensor()
:rawClear(0), rawRed(0), rawGreen(0), rawBlue(0),
 initClear(0),
 calR(0), calG(0), calB(0),
 convR(0), convG(0), convB(0),
 convH(0), convS(0), convV(0),ledOutVal(0)
{
}


/******************************************************************************/
/* Color Sensor                                                               */
/******************************************************************************/
#define SENS_INTEGRATION_TIMEx10   6144    /* Integration Time (x10) [24-6144] (mSec x 10) */
#define SENS_WAIT_TIMEx10          24   /* Wait Time (x10) 20000  [24-73728] (mSec x 10) */
#define SENS_GAIN                  0      /* GAIN  0:1x
                                            *       1:4x
                                            *       2:16x
                                            *       3:60x
                                            */
#define SENS_ADDRESS    0x29

/*** REG ***/
#define SENS_ENABLE_REG  0x00
#define SENS_ATIME_REG   0x01
#define SENS_WTIME_REG   0x03
#define SENS_AILTL_REG   0x04
#define SENS_AILTH_REG   0x05
#define SENS_AIHTL_REG   0x06
#define SENS_AIHTH_REG   0x07
#define SENS_PERS_REG    0x0C
#define SENS_CONFIG_REG  0x0D
#define SENS_CONTROL_REG 0x0F
#define SENS_ID_REG      0x12
#define SENS_STATUS_REG  0x13
#define SENS_CDATA_REG   0x14
#define SENS_CDATAH_REG  0x15
#define SENS_RDATA_REG   0x16
#define SENS_RDATAH_REG  0x17
#define SENS_GDATA_REG   0x18
#define SENS_GDATAH_REG  0x19
#define SENS_BDATA_REG   0x1A
#define SENS_BDATAH_REG  0x1B
// command register bits
#define SENS_COMMAND_BIT  0x80
#define SENS_AUTOINCR_BIT 0x20
#define SENS_SPECIAL_BIT  0x60
#define SENS_INTCLEAR_BIT 0x06


// enable register bits
#define SENS_AIEN_BIT     0x10
#define SENS_WEN_BIT      0x08
#define SENS_AEN_BIT      0x02
#define SENS_PON_BIT      0x01


// configuration register bits
#define SENS_WLONG_BIT    0x02

// status register bits
#define SENS_AINT_BIT     0x10
#define SENS_AVALID_BIT   0x01


/*******************************************************************************
 *
 */
void ClassColorSensor::begin(word colorSensorintegratinTime_x10, byte gain)
{
    i2c.begin();
    ledInit();
    colorSensorInit(colorSensorintegratinTime_x10, gain);
}

void ClassColorSensor::begin(){
	begin(SENS_INTEGRATION_TIMEx10, SENS_GAIN);
}

void ClassColorSensor::begin(word colorSensorintegratinTime_x10){
	begin(colorSensorintegratinTime_x10, SENS_GAIN);
}


/******************************************************************************
 *
 */
void ClassColorSensor::colorSensorInit(word colorSensorintegratinTime_x10, byte gain)
{
    /*
     * Interrupt Disable
     */
    colorSensorWrite8(SENS_ENABLE_REG, colorSensorRead8(SENS_ENABLE_REG) & ~SENS_AIEN_BIT);

    /*
     * Set IntegrationTime
     */
    colorIntegrationTime((word)colorSensorintegratinTime_x10);

    /*
     * Set Wait Time
     */
    //colorWaitTime(false, SENS_WAIT_TIMEx10);

#if 0
#if (SENS_WAIT_TIMEx10 <= 6144)
    colorSensorWrite8(SENS_CONFIG_REG, 0x00);
    colorSensorWrite8(SENS_WTIME_REG, (byte)(256-(SENS_WAIT_TIMEx10/24)));
    colorSensorWrite8(SENS_ENABLE_REG, colorSensorRead8(SENS_ENABLE_REG) | SENS_WEN_BIT);
#else
    colorSensorWrite8(SENS_CONFIG_REG, SENS_WLONG_BIT);
    colorSensorWrite8(SENS_WTIME_REG, (byte)(256-(SENS_WAIT_TIMEx10/288)));
    colorSensorWrite8(SENS_ENABLE_REG, colorSensorRead8(SENS_ENABLE_REG) | SENS_WEN_BIT);
#endif
#endif
    /*
     * Set Gain
     */
    gain = constrain(gain, 0, 3);
    colorSensorWrite8(SENS_CONTROL_REG, gain);

    /*
     * Start
     */
    colorSensorWrite8(SENS_ENABLE_REG, colorSensorRead8(SENS_ENABLE_REG) | SENS_PON_BIT);
    delay(3);
    colorSensorWrite8(SENS_ENABLE_REG, colorSensorRead8(SENS_ENABLE_REG) | SENS_AEN_BIT);

    while (1){
    	if(colorRead()){
    		word imin(min(min(colorRawRed(), colorRawGreen()), colorRawBlue()));
    		calR = (float)imin / (float)colorRawRed();
    		calG = (float)imin / (float)colorRawGreen();
    		calB = (float)imin / (float)colorRawBlue();
    		convertHSV(colorRawRed(), colorRawGreen(), colorRawBlue());
    		initH = convertH();
    		break;
    	}
    }

}

void ClassColorSensor::colorSensorInit(word colorSensorintegratinTime_x10){
	colorSensorInit(colorSensorintegratinTime_x10, SENS_GAIN);
}


void ClassColorSensor::colorSensorInit(void){
	colorSensorInit(SENS_INTEGRATION_TIMEx10);
}


void ClassColorSensor::colorWaitTime(bool enable, word time){
    /*
     * Set Wait Time
     */
	time = constrain(time, 24, 73728);
	byte enableBit(0);
	if(enable){
		enableBit = SENS_WEN_BIT;
	}

	if( time <= 6144) {
		colorSensorWrite8(SENS_CONFIG_REG, 0x00);
		colorSensorWrite8(SENS_WTIME_REG, (byte)(256-(time/24)));
	}
	else {
		colorSensorWrite8(SENS_CONFIG_REG, SENS_WLONG_BIT);
		colorSensorWrite8(SENS_WTIME_REG, (byte)(256-(time/288)));
	}
	colorSensorWrite8(SENS_ENABLE_REG, colorSensorRead8(SENS_ENABLE_REG) | enableBit);
}

void ClassColorSensor::colorIntegrationTime(word time){
	/*
	 * Set IntegrationTime
	 */
	time = constrain(time, 24, 6144);
	colorSensorWrite8(SENS_ATIME_REG, 256 - byte(time/24));
}

#if 0
void ClassColorSensor::colorIntegrationTime(float time){
	word aTime = (word)(time * 10);
	aTime = aTime / 24;
	if (aTime > 256)
		aTime = 256;
	aTime = aTime - 256;
	colorSensorWrite8(SENS_ATIME_REG, aTime);
}
#endif

/******************************************************************************
 *
 */
byte ClassColorSensor::colorSensorRead8( byte reg )
{
    i2c.beginTransmission(SENS_ADDRESS);
    i2c.write( byte(reg | SENS_COMMAND_BIT) );
    i2c.endTransmission();

    i2c.requestFrom(SENS_ADDRESS, 1 );
    return i2c.read();
}

/******************************************************************************
 *
 */
word ClassColorSensor::colorSensorRead16( byte reg )
{
    word val;

    i2c.beginTransmission(SENS_ADDRESS);
    i2c.write( byte(reg | SENS_COMMAND_BIT | SENS_AUTOINCR_BIT) );
    i2c.endTransmission();

    i2c.requestFrom(SENS_ADDRESS, 2 );

    val = i2c.read();
    val |= i2c.read() << 8;
    return val;

}

/******************************************************************************
 *
 */
void ClassColorSensor::colorSensorWrite8( byte reg, byte val )
{
    i2c.beginTransmission(SENS_ADDRESS);
    i2c.write( byte(reg | SENS_COMMAND_BIT) );
    i2c.write( val );
    i2c.endTransmission();
}

/******************************************************************************
 *
 */
boolean ClassColorSensor::colorRead( void )
{

    if( (colorSensorRead8(SENS_STATUS_REG) & SENS_AVALID_BIT) == SENS_AVALID_BIT ) {
        rawClear = colorSensorRead16(SENS_CDATA_REG);
        rawRed = colorSensorRead16(SENS_RDATA_REG);
        rawGreen = colorSensorRead16(SENS_GDATA_REG);
        rawBlue = colorSensorRead16(SENS_BDATA_REG);
        return true;
    }

    return false;
}
/******************************************************************************
 *
 */
word ClassColorSensor::colorRawClear(void)
{
    return rawClear;
}
/******************************************************************************
 *
 */
word ClassColorSensor::colorRawRed(void)
{
    return rawRed;
}
/******************************************************************************
 *
 */
word ClassColorSensor::colorRawGreen(void)
{
    return rawGreen;
}
/******************************************************************************
 *
 */
word ClassColorSensor::colorRawBlue(void)
{
    return rawBlue;
}
/******************************************************************************
 *
 */
boolean ClassColorSensor::colorRawCRGB( word &c, word &r, word &g, word &b )
{
    boolean retVal = colorRead();
    c = rawClear;
    r = rawRed;
    g = rawGreen;
    b = rawBlue;

    return retVal;
}

word ClassColorSensor::colorClear(void){
	return rawClear;
}

word ClassColorSensor::colorRed(void){
	//return (word)((float)rawRed * calR);
	return rawRed;
}

word ClassColorSensor::colorGreen(void){
	//return (word)((float)rawGreen * calG);
	return rawGreen;
}

word ClassColorSensor::colorBlue(void){
	//return (word)((float)rawBlue * calB);
	return rawBlue;
}

/******************************************************************************/
/* LED                                                                        */
/******************************************************************************/
#define ledNum   8
#define ledNo_b  0
#define ledNo_g  1
#define ledNo_r  2
#define def_brightness  0xff

#define def_iref_cm 1   /* 0 or 1 */
#define def_iref_hc 1   /* 0 or 1 */
#define def_iref_cc 4   /* 0 to 63 */

#if 1
    /* 8port */
#define LED_PORT_NUM 8
#define LED_ADDRESS                        (byte)(0b1000000)  /* Assumes A0 to A3 are connected to ground */
//#define LED_ALLCALL_ADDRESS                0b1001000
//#define LED_RESET_ADDRESS                  0b1001011
/* TLC59116 registers */
//#define TLC59116_GRPPWM                0x0a
#define LED_LEDOUT0            0x0c
#define LED_PWM0               0x02
#define LED_IREF               0x12

#else
    /* 16 port */
#define LED_PORT_NUM 16
#define LED_ADDRESS                        0b1100000  /* Assumes A0 to A3 are connected to ground */
//#define LED_ALLCALL_ADDRESS                0b1101000
//#define LED_RESET_ADDRESS                  0b1101011

/* TLC59116 registers */
//#define TLC59116_GRPPWM                0x12
#define LED_LEDOUT0               0x14

#endif


/* Control register (three MSB control auto-increment) */
#define LED_NO_AUTO_INCREMENT              0b00000000
#define LED_AUTO_INCREMENT_ALL_REGISTERS   0b10000000
//#define AUTO_INCREMENT_BRIGHTNESS      0b10100000
//#define AUTO_INCREMENT_CONTROL         0b11000000
//#define AUTO_INCREMENT_BRIGHT_CONTROL  0b11100000

//////////////////
#if 0
#if 1
#else
#endif

/* I2C bus addresses (excludes the R/W bit) */
#if 1
#else
#endif


/* I2C R/W */
#define I2C_READ                       1
#define I2C_WRITE                      0





/* LED output state for LEDOUT0 to LEDOUT3 */
#define LED_OUTPUT_OFF                 0b00
#define LED_OUTPUT_GROUP               0b11
#endif

/*******************************************************************************
 *
 */
void ClassColorSensor::ledInit( void )
{
    /*
     * Initialize Device
     */
    i2c.beginTransmission(LED_ADDRESS);                  /* Transmit to the TLC59116 */
    i2c.write(byte(LED_AUTO_INCREMENT_ALL_REGISTERS));   /* Send the control register.
                                                          * All registers will be written to, starting at register 0
                                                          */
    i2c.write(byte(0));                                 /* 00:Set MODE1: no sub-addressing */
    i2c.write(byte(0));                                 /* 01:Set MODE2: dimming */
    for (int i=0; i< LED_PORT_NUM; i++) {               /* 02-09 :Set individual brightness control to maximum */
        i2c.write(byte(def_brightness));
    }
    i2c.write(byte(0xFF));                              /* 0A :Set GRPPWM: Full brightness */
    i2c.write(byte(0));                                 /* 0B :Set GRPFREQ: Not blinking, must be 0 */
    for (int i=0; i<(LED_PORT_NUM/4); i++) {            /* 0C,0D :Set LEDs off for now */
        i2c.write(byte(0));
    }
    i2c.endTransmission();                              /* Set the I2C all-call and sub addresses (if needed) */

    ledIref( def_iref_cm, def_iref_hc, def_iref_cc );

}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledIref( byte cm,   /* 0 or 1 */
                             byte hc,   /* 0 or 1 */
                             byte cc    /* 0 to 63 */
                           )
{
    cm &= 1;
    hc &= 1;
    cc &= 63;

    i2c.beginTransmission(LED_ADDRESS);
    i2c.write(byte(LED_NO_AUTO_INCREMENT + LED_IREF));
    i2c.write( (byte)((cm<<7)|(hc<<6)|cc) );
    i2c.endTransmission();

}

/*******************************************************************************
 *
 */
void ClassColorSensor::led( void )
{
    i2c.beginTransmission(LED_ADDRESS);
    i2c.write(byte(LED_AUTO_INCREMENT_ALL_REGISTERS + LED_LEDOUT0)); /* Write to consecutive registers, starting with LEDOUT0 */
    i2c.write( lowByte(ledOutVal) );
    i2c.write( highByte(ledOutVal) );
    i2c.endTransmission();
}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledBrightness( int no, byte brightness )
{
    if( no >= LED_PORT_NUM ) return;

    brightness = constrain(brightness, 0, 100);
//    brightness = (byte)map(brightness, 0, 100, 0, 255);
    brightness = (byte)map(brightness, 0, 100, 0, 10);

    i2c.beginTransmission(LED_ADDRESS);
    i2c.write(byte(LED_NO_AUTO_INCREMENT + LED_PWM0 + no)); /* Write to the PWMx register */
    i2c.write(byte(brightness));
    i2c.endTransmission();
}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledOnOff( int no, boolean bOn )
{
    if( no >= LED_PORT_NUM ) return;

    int shift = no * 2;
    int val = 0x02<<shift;

    if( bOn ) {
        ledOutVal |= val;
    }
    else {
        ledOutVal &= ~val;
    }
    led();
}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledOff( int no )
{
    ledOnOff( no, false );
}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledOn( int no )
{
    ledOnOff( no, true );
}

/*******************************************************************************
 *
 */
void ClassColorSensor::led( int no )    // toggle On/Off
{
    int shift = no * 2;
    int val = 0x02<<shift;

    if( no >= LED_PORT_NUM ) return;

    ledOutVal ^=  val;

    led();

}

/*******************************************************************************
 *
 */
void ClassColorSensor::led( int no, boolean bOn, byte brightness )
{
    ledBrightness( no, brightness );
    ledOnOff( no, bOn );
}


void ClassColorSensor::led(int no, byte brightness) {
	ledBrightness( no, brightness );
	ledOnOff( no, true );
}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledRGB( byte r, byte g, byte b )
{
    /*
     * Blue  : 2180mcd           = 100% <= (base)
     * Red   : 2180mcd / 3000mcd = 0.72 => 72%   72/100=36/50=18/25
     * Green : 2180mcd / 4200mcd = 0.52 => 52%   52/100=26/50=13/25
     */

    /*** R ***/
#if 0
	unsigned long val;
	if( r > 100 ) {
        r = 100;
    }

    val = (unsigned long)r * 2; //1が入力されるとvalが0になってしまうため最初に2をかける
    val *= 576; /* x576 = x18 x32 */
    val /= 800; /* /800 = /25 /32 */
    r = (byte)val;
#endif
    byte r255(constrain(r, 0, 100));
    r255 = (byte)map(r255, 0, 100, 0, 184); // 255*0.72=183.6

    /*** G ***/
#if 0
    if( g > 100 ) {
        g = 100;
    }

    val = (unsigned long)g * 2;
    val *= 416; /* x416 = x13 x32 */
    val /= 800; /* /800 = /25 /32 */
    g = (byte)val * 2;
#endif
    byte g255(constrain(g, 0, 100));
    g255 = (byte)map(g255, 0, 100, 0, 133); // 255*0.52=132.6

    /*** B ***/
#if 0
    if( b > 100 ) {
        b = 100;
    }
    b *= 2;
#endif
    byte b255(constrain(b, 0, 100));
    b255 = (byte)map(b255, 0, 100, 0, 255);

    i2c.beginTransmission(LED_ADDRESS);
    i2c.write(byte(LED_AUTO_INCREMENT_ALL_REGISTERS + LED_PWM0 + ledNo_b)); /* Write to the PWMx register */
    i2c.write( b255 );
    i2c.write( g255 );
    i2c.write( r255 );
    i2c.endTransmission();

    ledRGB( true );
}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledRGB( boolean bOn )
{
    unsigned short val = 0x2a << ledNo_b;
    if( bOn ) {
        ledOutVal |= val;
    }
    else {
        ledOutVal &= ~val;
    }

    led();
}

/*******************************************************************************
 *
 */
void ClassColorSensor::ledRGB( boolean bOn, byte r, byte g, byte b )
{
    ledRGB( r, g, b );
    ledRGB( bOn );
}


void ClassColorSensor::hue2rgb( const word h, byte &r, byte &g, byte &b){

	/* HSVのH値を各rgbの正規化値(0-100)に変換する処理 */
	if (h <= 120) {
		/* H値(0-120) 赤-黄-緑     */
		r = map(h, 0, 120, 100, 0); // 赤LED R←→G
		g = map(h, 0, 120, 0, 100); // 緑LED G←→R
		b = 0;
	} else if (h <= 240) {
		/* H値(120-240) 緑-水色-青 */
		g = map(h, 120, 240, 100, 0); // 緑LED G←→B
		b = map(h, 120, 240, 0, 100); // 青LED B←→G
		r = 0;
	} else {
		/* H値(240-360) 青-紫-赤   */
		b = map(h, 240, 360, 100, 0); // 青LED B←→R
		r = map(h, 240, 360, 0, 100); // 青LED R←→B
		g = 0;
	}
}

void ClassColorSensor::rgb2hsv(const word r, const word g, const word b, word &h, word &s, word &v){
	word imax(max(max(r, g), b));
	word imin(min(min(r, g), b));
	v = imax;
	s = (float)(imax - imin)/(float)imax * 100;

	float hr = (float)(imax - r)/(float)(imax - imin);
	float hg = (float)(imax - g)/(float)(imax - imin);
	float hb = (float)(imax - b)/(float)(imax - imin);


	if(imax == r){
		h = 60 * (hb - hg);
	}
	else if(imax == g){
		h = 60 * (2 + hr - hb);
	}
	else if(imax == b){
		h = 60 * (4 + hg - hr);
	}

	if ((int)h < 0){
		h = h + 360;
	}
}

void ClassColorSensor::convertRGB(const word h) {
	hue2rgb(h,convR, convG, convB);
}

word ClassColorSensor::convertRed() {
	return convR;
}

word ClassColorSensor::convertGreen() {
	return convG;
}

word ClassColorSensor::convertBlue() {
	return convB;
}

void ClassColorSensor::convertHSV(const word r, const word g, const word b) {
	rgb2hsv(r, g, b, convH, convS, convV);
}

word ClassColorSensor::convertH() {
	return convH;
}

word ClassColorSensor::convertS() {
	return convS;
}

word ClassColorSensor::convertV() {
	return convV;
}

boolean ClassColorSensor::colorBlack() {
	return colorIsBlack(initClear/2);
}

boolean ClassColorSensor::colorIsBlack(word th) {
	if(th > rawClear){
		return true;
	}
	return false;
}
