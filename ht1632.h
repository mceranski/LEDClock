/* ===================================================================
 *  Interface functions for the Sure Electronics HT1632 based BiColor
 *  32x16 LED Matrix display
 * ===================================================================
 */
#ifndef __ht1632_h
#define __ht1632_h

#include <Arduino.h>
 
/*
  commands written to the chip consist of a 3 bit "ID", followed by
  either 9 bits of "Command code" or 7 bits of address + 4 bits of data.
*/

#define HT1632_ID_CMD 4		/* ID = 100 - Commands */
#define HT1632_ID_RD  6		/* ID = 110 - Read RAM */
#define HT1632_ID_WR  5		/* ID = 101 - Write RAM */

#define HT1632_CMD_SYSDIS 0x00	/* CMD= 0000-0000-x Turn off oscil */
#define HT1632_CMD_SYSON  0x01	/* CMD= 0000-0001-x Enable system oscil */
#define HT1632_CMD_LEDOFF 0x02	/* CMD= 0000-0010-x LED duty cycle gen off */
#define HT1632_CMD_LEDON  0x03	/* CMD= 0000-0011-x LEDs ON */
#define HT1632_CMD_BLOFF  0x08	/* CMD= 0000-1000-x Blink ON */
#define HT1632_CMD_BLON   0x09	/* CMD= 0000-1001-x Blink Off */
#define HT1632_CMD_SLVMD  0x10	/* CMD= 0001-0xxx-x Slave Mode */
#define HT1632_CMD_MSTMD  0x18	/* CMD= 0001-10xx-x Use on-chip clock */
#define HT1632_CMD_EXTCLK 0x1C	/* CMD= 0001-11xx-x Use external clock */
#define HT1632_CMD_COMS00 0x20	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS01 0x24	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS10 0x28	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS11 0x2C	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_PWM    0xA0	/* CMD= 101x-PPPP-x PWM duty cycle */

// For dual LED Matrix use 63 and 15
// For single LED Matrix set these to:
// X_MAX=31
// Y_MAX=15
// CHIP_MAX=4
#define X_MAX 31	// 0 based X
#define Y_MAX 15	// 0 based Y
#define CHIP_MAX 4	// Number of HT1632C Chips

// Colors supported by the display
#define BLACK  0
#define GREEN  1
#define RED    2
#define ORANGE 3
#define MULTICOLOR 4

class HT1632
{
  private:
    byte shadowRam[64][CHIP_MAX];
    /* 
    we keep a copy of the display controller contents so that we can know which bits are on without having to (slowly) read the device.
    Note that we only use the low four bits of the shadow ram, since we're shadowing 4-bit memory.  This makes things faster, but we
    COULD do something with the other half of our bytes !
    */

    byte dataPin;	// Data pin (pin 7)
    byte clockPin; 	// Clock pin (pin 2)
    byte writeClockPin;	// Write clock pin (pin 5)
    byte chipSelectPin;		// Chip Select pin (pin 4)
	
    byte fontCode[8];		// Largest font is 8x8
    byte fontWidth;		// Font character width of current font

    void outputClockPulse(void);	//Output a clock pulse
    void chipSelect(int select); // Does chip select to chips in the display
    void writeBits (byte bits, byte firstbit);
    void sendCommand (byte chipNo, byte command);
    void sendData (byte chipNo, byte address, byte data);
    byte xyToIndex(byte x, byte y);
    void clear(int c0, int c1); // Clear region controlled by chips [c0..c1]

    void fillFontCodeBuffer(char chr);
    void clearFontCodeBuffer();
    int getMaxChars(int x);
  public:
      // Two ctors, one with defaults and one with custom pins
    HT1632();
    HT1632(byte data, byte clk, byte wrclk, byte cs);
	
    void setup();
    int getPixel(byte x, byte y);
    void plot(byte x, byte y, byte color);
    void clear(); // Clear the entire display
    void clearLine(int line); // Clears either upper or lower part of the display
    void setIntensity(int chip, int intensity);
    void setIntensity(int intensity);
    void scrollText( const char *s, byte color, int y = 4, int times = 1, int delaytime = 125);
    void matrixEffect( int times = 100, int delaytime = 5, byte color = GREEN );
    void print( const char *s, byte color = GREEN, int x = 0, int y = 0 );
};

#endif
