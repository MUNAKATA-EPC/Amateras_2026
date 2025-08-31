/*
Matrix.cpp - Max7219 LED Matrix library for Arduino & Wiring
Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// TODO: Support segment displays in api?
// TODO: Support varying vendor layouts?

/******************************************************************************
* Includes
******************************************************************************/
#include <avr/pgmspace.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "Sprite.h"
#include "Matrix.h"
#include "font.h"
//#include "font4x5.h"
#include "font3x5.h"

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

/******************************************************************************
* Definitions
******************************************************************************/

// Matrix registers
#define REG_NOOP   0x00
#define REG_DIGIT0 0x01
#define REG_DIGIT1 0x02
#define REG_DIGIT2 0x03
#define REG_DIGIT3 0x04
#define REG_DIGIT4 0x05
#define REG_DIGIT5 0x06
#define REG_DIGIT6 0x07
#define REG_DIGIT7 0x08
#define REG_DECODEMODE  0x09
#define REG_INTENSITY   0x0A
#define REG_SCANLIMIT   0x0B
#define REG_SHUTDOWN    0x0C
#define REG_DISPLAYTEST 0x0F

static uint8_t row_list[]={6,0,4,2,7,5,3,1};
//static uint8_t row_list[]={1,3,5,7,2,4,0,6};
static uint8_t col_list[]={3,6,2,1,7,5,0,4};
//static uint8_t col_list[]={4,0,5,7,1,2,6,3};

/******************************************************************************
* Constructors
******************************************************************************/

Matrix::Matrix(uint8_t data, uint8_t clock, uint8_t load, uint8_t screens /* = 1 */)
{
	// record pins for sw spi
	_pinData = data;
	_pinClock = clock;
	_pinLoad = load;
	
	// set ddr for sw spi pins
	pinMode(_pinClock, OUTPUT);
	pinMode(_pinData, OUTPUT);
	pinMode(_pinLoad, OUTPUT);
	
	// allocate screenbuffers
	_screens = screens;
	_buffer = (uint8_t*)calloc(_screens, 64);
	_maximumX = (_screens * 8);
	
	// initialize registers
	clear();             // clear display
	setScanLimit(0x07);  // use all rows/digits
	setBrightness(0x0F); // maximum brightness
	setRegister(REG_SHUTDOWN, 0x01);    // normal operation
	setRegister(REG_DECODEMODE, 0x00);  // pixels not integers
	setRegister(REG_DISPLAYTEST, 0x00); // not in test mode
}

/******************************************************************************
* MAX7219 SPI
******************************************************************************/

// sends a single byte by sw spi (no latching)
void Matrix::putByte(uint8_t data)
{
	uint8_t i = 8;
	uint8_t mask;
	while(i > 0) {
		mask = 0x01 << (i - 1);         // get bitmask
		digitalWrite(_pinClock, LOW);   // tick
		if (data & mask){               // choose bit
			digitalWrite(_pinData, HIGH); // set 1
		}else{
			digitalWrite(_pinData, LOW);  // set 0
		}
		digitalWrite(_pinClock, HIGH);  // tock
		--i;                            // move to lesser bit
	}
}

// sets register to a byte value for all screens
void Matrix::setRegister(uint8_t reg, uint8_t data)
{
	digitalWrite(_pinLoad, LOW); // begin
	for(uint8_t i = 0; i < _screens; ++i){
		putByte(reg);  // specify register
		putByte(data); // send data
	}
	digitalWrite(_pinLoad, HIGH);  // latch in data
	digitalWrite(_pinLoad, LOW); // end
}

// syncs row of display with buffer
void Matrix::syncRow(uint8_t row)
{
	if (!_buffer) return;
	
	// uint8_t's can't be negative, so don't test for negative row
	if (row >= 8) return;
	row=row_list[row];
	digitalWrite(_pinLoad, LOW); // begin
	for(uint8_t i = 0; i < _screens; ++i){
		putByte(8 - row);                // specify register
		putByte(_buffer[row + (8 * i)]); // send data
	}
	digitalWrite(_pinLoad, HIGH);  // latch in data
	digitalWrite(_pinLoad, LOW); // end
}

/******************************************************************************
* MAX7219 Configuration
******************************************************************************/

// sets how many digits are displayed
void Matrix::setScanLimit(uint8_t value)
{
	setRegister(REG_SCANLIMIT, value & 0x07);
}

// sets brightness of the display
void Matrix::setBrightness(uint8_t value)
{
	setRegister(REG_INTENSITY, value & 0x0F);
}

/******************************************************************************
* Helper Functions
******************************************************************************/

void Matrix::buffer(uint8_t x, uint8_t y, uint8_t value)
{
	if (!_buffer) return;
	
	// uint8_t's can't be negative, so don't test for negative x and y.
	if (x >= _maximumX || y >= 8) return;
	
	uint8_t offset = x; // record x
	x %= 8;             // make x relative to a single matrix
	offset -= x;        // calculate buffer offset
	x=col_list[x];
	y=row_list[y];
	
	// wrap shift relative x for nexus module layout
	if (x == 0){
		x = 8;
	}
	--x;
	
	// record value in buffer
	if(value){
		_buffer[y + offset] |= 0x01 << x;
	}else{
		_buffer[y + offset] &= ~(0x01 << x);
	}
}

/******************************************************************************
* User API
******************************************************************************/

// buffers and writes to screen
void Matrix::write(uint8_t x, uint8_t y, uint8_t value)
{
	buffer(x, y, value);
	
	// update affected row
	syncRow(y);
}

void Matrix::write(uint8_t x, uint8_t y, Sprite sprite)
{
	for (uint8_t i = 0; i < sprite.height(); i++){
		for (uint8_t j = 0; j < sprite.width(); j++)
		buffer(x + j, y + i, sprite.read(j, i));
		
		syncRow(y + i);
	}
}

// clears screens and buffers
void Matrix::clear(void)
{
	if (!_buffer) return;
	
	// clear buffer
	for(uint8_t i = 0; i < 8; ++i){
		for(uint8_t j = 0; j < _screens; ++j){
			_buffer[i + (8 * j)] = 0x00;
		}
	}
	
	// clear registers
	for(uint8_t i = 0; i < 8; ++i){
		syncRow(i);
	}
}

//*******************************************
// fuRo Orignal function
//*******************************************
// 8x8フォントを文字を表示する。引数は文字コード
/*
void Matrix::putch(uint8_t x, uint8_t y, uint8_t ft)
{
unsigned char buf;

for (uint8_t i = 0; i < 8; i++){
for (uint8_t j = 0; j < 8; j++)
{
buf =  pgm_read_byte_near(fontb + (8*ft + i));
buffer(-x + j, y + i, (buf>>j)&0x01);
}
syncRow(y + i);
}
}
*/
void Matrix::putch(uint8_t x, uint8_t y, uint8_t ft, uint8_t pos)
{
	unsigned char buf;
	
	for (uint8_t i = 0; i < 8; i++){
		for (uint8_t j = 0; j < 8; j++)
		{
			buf =  pgm_read_byte_near(fontb + (8*ft + i));
			switch(pos)
			{
			  case UP://correct
				buffer(x + j, y + i, (buf>>j)&0x01);
				break;
			  case RIGHT://turn right
				buffer(x + 7-i, y +  j, (buf>>j)&0x01);
				break;
			  case DOWN://upside down
				buffer(x +7 - j, y  + 7 - i, (buf>>j)&0x01);
				break;
			  case LEFT://turn left
				buffer(x  +  i, y + 7-j, (buf>>j)&0x01);
				break;
			  default: //default
				buffer(x + j, y + i, (buf>>j)&0x01);
				break;
			}
		}
	}
	for (uint8_t i = 0; i < 8; i++)syncRow(i);// sync
}

//-----------------------------
// 8x8フォントで10進数を表示する。引数は十進数
/*
void Matrix::putd(uint8_t x, uint8_t y, uint8_t n)
{
if(n<0||n>9)return;
n = n + '0';
for (uint8_t i = 0; i < 8; i++){
for (uint8_t j = 0; j < 8; j++)
buffer(x + j, y + i, (fontb[n][i]>>j)&0x01);

syncRow(y + i);
}
}
*/
void Matrix::putd(uint8_t x, uint8_t y, uint8_t n)
{
	unsigned char buf;	
	if(n<0||n>9)return;
	n = n + '0';
	for (uint8_t i = 0; i < 8; i++){
		for (uint8_t j = 0; j < 8; j++)
		{
			buf =  pgm_read_byte_near(font35 + (8*n+i));
			buffer(x + j, y + i, (buf>>j)&0x01); 
		}
		syncRow(y + i);
	}
}


//-----------------------------

//4x5フォントを表示する。引数は表示したい数字値
void Matrix::putch45(uint8_t x, uint8_t y, uint8_t ft)
{
	unsigned char buf;	
	ft=ft-0x20;	
	for (uint8_t i = 0; i < 5; i++){
		for (uint8_t j = 4; j < 8 ; j++)  {
			buf =  pgm_read_byte_near(font35 + (5*ft+i));
			buffer(x +  7 - j, y + i, (buf>>j)&0x01);
		}
		syncRow(y + i);
	}
}
//------------------------------
//4x5フォント文字列を表示する。引数は表示したい数字値
void Matrix::putch45(uint8_t x, uint8_t y, const char *ft)
{
	unsigned char buf;	
	for (uint8_t i = 0; i < 5; i++){
		for (uint8_t j = 4; j < 8 ; j++)  {
			buf =  pgm_read_byte_near(font35 + (5*(ft[0]-0x20)+i));
			buffer(x +  7 - j, y + i, (buf>>j)&0x01);	
			//       buffer(x +  7 - j, y + i, (font35[ft[0]-0x20][i]>>j)&0x01);
		}
		syncRow(y + i);
	}
	for (uint8_t i = 0; i < 5; i++){
		for (uint8_t j = 4; j < 8 ; j++)  {
			buf =  pgm_read_byte_near(font35 + (5*(ft[1]-0x20)+i));
			buffer(x +  7 - j, y + i, (buf>>j)&0x01);	
			//        buffer(x +  7 - j + 4, y + i, (font35[ft[1]-0x20][i]>>j)&0x01);
		}
		syncRow(y + i);
	}
	
}
//-----------------------------
//4x5フォントで二桁数字を表示する0-99
void Matrix::putd2(uint8_t x, uint8_t y, uint8_t n)
{
	int u,d;
	
	if(n<=0||n<=100)
	{
		d=n%10;
		u=n/10;
	}
	else
	{
		u=10;d=10;
	}	
	putch45(x,y, u+0x30);
	putch45(x+4,y, d+0x30);
	
}
//-----------------------------
//指定した8x8キャラを表示する。引数は表示したいポインタ
void Matrix::putm(uint8_t x, uint8_t y, uint8_t ft[])
{
	for (uint8_t i = 0; i < 8; i++){
		for (uint8_t j = 0; j < 8; j++)
		buffer(x + j, y + i, (ft[i]>>j)&0x01);
		syncRow(y + i);
	}
}
//--------------------------------
//文字をスクロールさせる8x8
//---------------------------------
void Matrix::scrollText(const char text[], uint8_t dtime, uint8_t screen)
{
	
	//---------- スタートアップスクロール
	for(uint8_t i=0;i<8*screen;i++)
	{
		for(uint8_t k=0;k<=screen+1;k++)
		{
			putch(8*(k+screen)-i, 0, text[k]);//マトリクスに表示をおこなう
		}
		delay(dtime);
	}
	//---------- 通常スクロール
	for(uint8_t j=0;j<strlen(text)-screen;j++)
	{
		for(uint8_t i=0;i<8;i++)
		{
			for(uint8_t k=0;k<=screen+1;k++)
			{
				putch(8*k-i, 0, text[j+k]);//マトリクスに表示をおこなう
			}
			delay(dtime);
		}
	}
	
	//ラストスクロール
	for(uint8_t i=0;i<8;i++)
	{
		putch(-i, 0, text[strlen(text)-screen]);//マトリクスに表示をおこなう
		for(uint8_t k=1;k<=screen+1;k++){
			putch(8*k-i, 0, ' ');//マトリクスに表示をおこなう
		}
		delay(dtime);
	}
	
}

//--------------------------------
//文字をスクロールさせる4x5
//--------------------------------
void Matrix::scrollText45(const char text[], uint8_t dtime, uint8_t screen)
{
	//---------- スタートアップスクロール
	for(uint8_t i=0;i<8*screen;i++)
	{
		for(uint8_t k=0;k<=2*screen+1;k++)
		{
			putch45(4*(k+2*screen)-i, 0, text[k]);//マトリクスに二桁数字表示をおこなう
		}
		delay(dtime);
	}
	
	//---------- 通常スクロール
	for(uint8_t j=0;j<strlen(text);j++)
	{
		for(uint8_t i=0;i<4;i++)
		{
			for(uint8_t k=0;k<=2*screen+1;k++)
			{
				putch45(4*k-i, 0, text[j+k]);//マトリクスに二桁数字表示をおこなう
			}
			delay(dtime);
		}
	}
}
//-----------------------------
// line 
//-----------------------------
void Matrix::line(
			uint8_t x1, uint8_t y1,
			uint8_t x2, uint8_t y2,
			uint8_t b){
	uint8_t dx, dy;
	int8_t sx, sy;
	uint8_t chg;
	int16_t e;
	uint8_t i;
	uint8_t tmp;
	
	if(x1 > x2){
		sx = -1; 
		dx = x1 - x2;
	} 
	else {
		sx =  1; 
		dx = x2 - x1;
	}
	if(y1 > y2){
		sy = -1; 
		dy = y1 - y2;
	} 
	else {
		sy =  1; 
		dy = y2 - y1;
	}
	
	if(dy > dx){
		tmp = dx; 
		dx = dy; 
		dy = tmp;
		chg = 1;
	} 
	else 
	chg = 0;
	
	e = dy * 2 - dx;
	for(i = 0; i <= dx; i++){
		write(x1, y1, b);
		if(e >= 0){
			if(chg)
			x1 += sx;
			else
			y1 += sy;
			e -= dx * 2;
		}
		if(chg)
		y1 += sy;
		else
		x1 += sx;
		e += dy * 2;
	}
}

//---------------------------
//円形を描く
//---------------------------
void Matrix::circle(float x, float y, float r)
{
	int xx,yy;
	float th;
	for(th=0;th<=360;th+=15)
	{
		xx	= r*cos(th*PI/180);
		yy	= r*sin(th*PI/180);
		buffer(x+xx,y+yy,HIGH);
	}
		for (uint8_t i = 0; i < 8; i++)syncRow(i);// sync
}

//----------------------------
//	四角を描く
//----------------------------
void Matrix::rectangle(uint8_t x, uint8_t y, uint8_t dx,uint8_t dy)
{
	line(x , y, x+ dx, y);
	line(x +dx , y, x+ dx, y +dy);
	line(x +dx, y+dy, x , y + dy);
	line(x , y, x , y + dy);	
}

