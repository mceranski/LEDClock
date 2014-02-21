/* ===========================================================================================
 *	Class to control Sure Electronics Bi-Color (RED/GREEN) 32x16 LED Matrix
 * =========================================================================================== */
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "ht1632.h"
#include "font5x7.h"
#include "LEDClock.h"
#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------------------------------
// Function Mame: HT1632
// Purpose: Construct object of display with setting pin numbers to defaults
// -------------------------------------------------------------------------------------------
HT1632::HT1632()
{
  dataPin = MATRIX_DATA_PIN;
  clockPin = MATRIX_CLK_PIN;
  writeClockPin = MATRIX_WR_PIN;
  chipSelectPin = MATRIX_CS_PIN;
  fontWidth = 5;
}

// -------------------------------------------------------------------------------------------
// Function Mame: HT1632
// Purpose: Construct object of display with custom pin numbers
// Parameters:	data	The DATA pin
//				clk		The CLK pin (clock)
//				wrclk	The WRITE CLOCK pin
//				cs		The CS pin (chip-select)
// -------------------------------------------------------------------------------------------
HT1632::HT1632(byte data, byte clk, byte wrclk, byte cs)
{
  dataPin = data;
  clockPin = clk;
  writeClockPin = wrclk;
  chipSelectPin = cs;	
  fontWidth = 5;
}

// -------------------------------------------------------------------------------------------
// Function Name: outputClockPulse
// Purpose: enable CLK_74164 pin to output a clock pulse
// -------------------------------------------------------------------------------------------
void HT1632::outputClockPulse(void)	//Output a clock pulse
{
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}

// -------------------------------------------------------------------------------------------
// Function Name: chipSelect
// Purpose: enable HT1632C
// Parameters:
//			select	HT1632C to be selected
//					If select=0, select none.
//					If s<0, select all.
// -------------------------------------------------------------------------------------------
void HT1632::chipSelect(int select)
{
  unsigned char tmp = 0;
  if (select < 0)
  { // Enable all HT1632C
    digitalWrite(chipSelectPin, LOW);
    for (tmp=0; tmp<CHIP_MAX; tmp++)  
      outputClockPulse();
  }
  else if(select==0)
  {
    //Disable all HT1632Cs
    digitalWrite(chipSelectPin, HIGH);
    for(tmp=0; tmp<CHIP_MAX; tmp++)
      outputClockPulse();
  }
  else
  {
    digitalWrite(chipSelectPin, HIGH);
    for(tmp=0; tmp<CHIP_MAX; tmp++)
      outputClockPulse();
    digitalWrite(chipSelectPin, LOW);
    outputClockPulse();
    digitalWrite(chipSelectPin, HIGH);
    tmp = 1;
    for( ; tmp<select; tmp++)
        outputClockPulse();	
  }
}
// -------------------------------------------------------------------------------------------
// Function Name: writeBits
// Purpose: Write bits (up to 8) to h1632 on pins dataPin, writeClockPin
//                  Chip is assumed to already be chip-selected
//                  Bits are shifted out from MSB to LSB, with the first bit sent
//                  being (bits & firstbit), shifted till firsbit is zero.
// Parameters:
//		bits		Bits to send
//	    firstbit	The first bit to send
// -------------------------------------------------------------------------------------------
void writeBits(byte bits, byte firstbit)
{
  while (firstbit)
  {
    digitalWrite(3, LOW);
    if (bits & firstbit)
      digitalWrite(7, HIGH);
    else
      digitalWrite(7, LOW);
    digitalWrite(3, HIGH);
    firstbit >>= 1;
  }
}

void HT1632::writeBits (byte bits, byte firstbit)
{
  while (firstbit)
  {
    digitalWrite(writeClockPin, LOW);
    if (bits & firstbit)
      digitalWrite(dataPin, HIGH);
    else
      digitalWrite(dataPin, LOW);
    digitalWrite(writeClockPin, HIGH);
    firstbit >>= 1;
  }
}

// -------------------------------------------------------------------------------------------
// Function Name: sendCommand
// Purpose: Send a command to the ht1632 chip.
//                  Select 1 0 0 c7 c6 c5 c4 c3 c2 c1 c0 xx Free
// Parameters:
//		chipNo	The chip you want to send data
//      command	Consists of a 3-bit "CMD" ID, an 8bit command, and one "don't care bit".
// -------------------------------------------------------------------------------------------
void HT1632::sendCommand (byte chipNo, byte command)
{
  chipSelect(chipNo);
  writeBits(HT1632_ID_CMD, 1<<2);  // send 3 bits of id: COMMMAND
  writeBits(command, 1<<7);  // send the actual command
  writeBits(0, 1); 	/* one extra dont-care bit in commands. */
  chipSelect(0);
}

// -------------------------------------------------------------------------------------------
// Function Name: sendData
// Purpose: send a nibble (4 bits) of data to a particular memory location of the
//                  ht1632.  The command has 3 bit ID, 7 bits of address, and 4 bits of data.
//                  Select 1 0 1 A6 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3 Free
//                  Note that the address is sent MSB first, while the data is sent LSB first!
//                  This means that somewhere a bit reversal will have to be done to get
//                  zero-based addressing of words and dots within words.
// Parameters:
//		chipNo		The chip you want to send data
//		address		Chip address to write to
//		data		Data to write to chip memory
// -------------------------------------------------------------------------------------------
void HT1632::sendData(byte chipNo, byte address, byte data)
{
  chipSelect(chipNo);
  writeBits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
  writeBits(address, 1<<6); // Send address
  writeBits(data, 1<<3); // send 4 bits of data
  chipSelect(0);
}

// -------------------------------------------------------------------------------------------
// Function Name: clear
// Purpose: clear display including the shadow memory for region of chips c0 to c1
// Parameters:
//		c0	The first chip (1 is the first one)
//		c1	The last chip to clear
// -------------------------------------------------------------------------------------------
void HT1632::clear(int c0, int c1)
{
  byte i; 
  for (int j = c0; j <= c1; j++)
  {
    chipSelect(j);
    writeBits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
    writeBits(0, 1<<6);				// Send address
    for (i=0; i<64/2; i++)			// Clear entire display
      writeBits(0, 1<<7);			// send 8 bits of data
    chipSelect(0);
    for (i=0; i<64; i++)
      shadowRam[i][j-1] = 0;
  }
}

// -------------------------------------------------------------------------------------------
// Function Name: clear
// Purpose: clear entire display including the shadow memory
// -------------------------------------------------------------------------------------------
void HT1632::clear()
{
	clear(1, CHIP_MAX);
}

// -------------------------------------------------------------------------------------------
// Function Name: clearLine
// Purpose: clear one line in the display including the shadow memory
// -------------------------------------------------------------------------------------------
void HT1632::clearLine(int line)
{
  int startChip, endChip;
	
  if (line == 1)
  {
    startChip = 1;
    endChip = 2;
  }
  else
  {
    startChip = 3;
    endChip = 4;
  }	
  clear(startChip, endChip);
}

// -------------------------------------------------------------------------------------------
// Function Name: setIntensity
// Purpose: Sets the intensity of any of the quadrant of the display
// Parameters:
//		chip		The chip responsible for the quadrant [1..4]
//		intensity	The intensity value [0..15]
// -------------------------------------------------------------------------------------------
void HT1632::setIntensity(int chip, int intensity)
{
  byte i = intensity & 0x0F;
  i|= 0xA0;
  sendCommand(chip, i); 
}

// -------------------------------------------------------------------------------------------
// Function Name: setIntensity
// Purpose: Sets the intensity of any of the quadrant of the display
// Parameters:
//		intensity	The intensity value [0..15]
// -------------------------------------------------------------------------------------------
void HT1632::setIntensity(int intensity)
{
  byte i;
  for (i = 1; i <= CHIP_MAX; i++)
    setIntensity(i, intensity);
}

// -------------------------------------------------------------------------------------------
// Function Name: xyToIndex
// Purpose: get the value of x,y
// Parameters:
//		x		X coordinate
//		y		Y coordinate
//Return Code: address of xy pixel
// -------------------------------------------------------------------------------------------
byte HT1632::xyToIndex(byte x, byte y)
{
  byte nChip, addr;
  
  if (x>=32)
    nChip = 3 + x/16 + (y>7?2:0);
  else
    nChip = 1 + x/16 + (y>7?2:0);
	
  x = x % 16;
  y = y % 8;
  addr = (x<<1) + (y>>2);	
  return addr;
}

// -------------------------------------------------------------------------------------------
// Function Name: calcBit
// Purpose: calculate the bitval of y
// Parameters:
//		y		Y coordinate
// Return Code: bitval
// -------------------------------------------------------------------------------------------
#define calcBit(y) (8>>(y&3))

// -------------------------------------------------------------------------------------------
// Function Name: getPixel
// Purpose: get the value (color) of pixel in x,y. Function is using the shadow memory and does
//		not read the actual value from the display, though possible
// Parameters:
//		x		X coordinate
//      y		Y coordinate
// Return Code: color setted on x,y coordinates
// -------------------------------------------------------------------------------------------
int HT1632::getPixel(byte x, byte y)
{
  byte addr, bitval, nChip;
	
  if (x>=32)
    nChip = 3 + x/16 + (y>7?2:0);
  else
    nChip = 1 + x/16 + (y>7?2:0);
	
  addr = xyToIndex(x,y);
  bitval = calcBit(y);
	
  if ((shadowRam[addr][nChip-1] & bitval) && (shadowRam[addr+32][nChip-1] & bitval))
    return ORANGE;
  else if (shadowRam[addr][nChip-1] & bitval)
    return GREEN;
  else if (shadowRam[addr+32][nChip-1] & bitval)
    return RED;
  else
    return BLACK;
}

// -------------------------------------------------------------------------------------------
// Function Name: plot
// Purpose: plot a dot/pixel on x,y with given color
// Parameters:
//		x		X coordinate
//      y		Y coordinate
//      color	The color to show, BLACK(clean), GREEN, RED, ORANGE
// -------------------------------------------------------------------------------------------
void HT1632::plot (byte x, byte y, byte color)
{
  byte nChip, addr, bitval;
	
  if (x<0 || x>X_MAX || y<0 || y>Y_MAX) return;
  	
  if (color != BLACK && color != GREEN && color != RED && color != ORANGE) return;
	
  if (x>=32)
    nChip = 3 + x/16 + (y>7?2:0);
  else
    nChip = 1 + x/16 + (y>7?2:0);
	
  addr = xyToIndex(x,y);
  bitval = calcBit(y);

  switch (color)
  {
    case BLACK:
      if (getPixel(x,y) != BLACK)
      {
        // compare with memory to only set if pixel is other color
	// clear the bit in both planes;
	shadowRam[addr][nChip-1] &= ~bitval;
	sendData(nChip, addr, shadowRam[addr][nChip-1]);
	addr = addr + 32;
	shadowRam[addr][nChip-1] &= ~bitval;
	sendData(nChip, addr, shadowRam[addr][nChip-1]);
      }
      break;
    case GREEN:
        if (getPixel(x,y) != GREEN)
	{
	  // compare with memory to only set if pixel is other color
	  // set the bit in the green plane and clear the bit in the red plane;
	  shadowRam[addr][nChip-1] |= bitval;
	  sendData(nChip, addr, shadowRam[addr][nChip-1]);
	  addr = addr + 32;
	  shadowRam[addr][nChip-1] &= ~bitval;
	  sendData(nChip, addr, shadowRam[addr][nChip-1]);
	}
	break;
    case RED:
      if (getPixel(x,y) != RED)
      {
        // compare with memory to only set if pixel is other color
	// clear the bit in green plane and set the bit in the red plane;
	shadowRam[addr][nChip-1] &= ~bitval;
	sendData(nChip, addr, shadowRam[addr][nChip-1]);
	addr = addr + 32;
	shadowRam[addr][nChip-1] |= bitval;
	sendData(nChip, addr, shadowRam[addr][nChip-1]);
      }
      break;
    case ORANGE:
      if (getPixel(x,y) != ORANGE)
      {
        // compare with memory to only set if pixel is other color
        // set the bit in both the green and red planes;
        shadowRam[addr][nChip-1] |= bitval;
	sendData(nChip, addr, shadowRam[addr][nChip-1]);
	addr = addr + 32;
	shadowRam[addr][nChip-1] |= bitval;
	sendData(nChip, addr, shadowRam[addr][nChip-1]);
      }
      break;
    }
}

// -------------------------------------------------------------------------------------------
// Function Name: init
// Purpose: initialize the display by assigning the pin ports and setting up parameters to the
//	display itself. On return the display should be clear
// -------------------------------------------------------------------------------------------
void HT1632::setup()
{
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH);
  pinMode(writeClockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  for (int i=1; i<=CHIP_MAX; i++)
  {
    sendCommand(i, HT1632_CMD_SYSDIS); // Disable system
    sendCommand(i, HT1632_CMD_COMS00); // 16*32, PMOS drivers
    sendCommand(i, HT1632_CMD_MSTMD);  // MASTER MODE
    sendCommand(i, HT1632_CMD_SYSON);  // System on
    sendCommand(i, HT1632_CMD_LEDON);  // LEDs on
    setIntensity(i, 0x0F);
  }
  clear(); // Clear the display
}

void HT1632::print( const char *s, byte color, int x, int y )
{
    int fontWidth = 5;
    char c;
    int i, j;
    int mask;
    int xOffset = x;
    
    for( x=0; s[x]; x++)
    {
        c = s[x];
        clearFontCodeBuffer();
        fillFontCodeBuffer(c);
        for(i=0; i <8;i++)
        {
            mask = 1 << i;
            for(j=0; j < fontWidth;j++)
            {
                if (fontCode[j] & mask) {
                  plot(j+((fontWidth+1)*x) + xOffset, (7-i)+y, color == MULTICOLOR ? random(1, 4) : color );
                }
                else
                  plot(j+((fontWidth+1)*x) + xOffset, (7-i)+y, BLACK);
            }
        }
    }
}

int HT1632::getMaxChars(int x)
{
   return ( (X_MAX+1) - x ) / (fontWidth);
}

void HT1632::matrixEffect( int times, int delaytime, byte color )
{
  clear();
  
  //first fill the screen with random green dots
  for( int x = 1; x < X_MAX+1; x = x +2  ) {
    for( int y = 1; y < Y_MAX+1; y++ ) {
      int value = rand() % 100; 
      plot( x, y, (value % 2 ) == 0 ? GREEN : BLACK );
    }
  }
  
  for( int i = 0; i < times; i++ ) {
    for( int x = 1; x < X_MAX+1; x = x +2  ) {
      for( int y = Y_MAX+1; y >= 0; y-- ) {
        if( y == 1 ) {
          int value = rand() % 100; 
          plot( x, y, (value % 2 ) == 0 ? GREEN : BLACK );
        }
        else {
          plot( x, y, getPixel(x, y-1) );
        }
      }
    }
    delay( delaytime );
  }
  
  clear();
}

void HT1632::scrollText(const char *s, byte color, int y, int times, int delaytime)
{    
  int maxChars = getMaxChars(1);
  clear();
  for( int j = 0; j < times; j ++ ) {         
    for( int k = 0; k < strlen(s); k++ ) {
      char buffer[maxChars];  
      memcpy(buffer, s + k, maxChars);  
      int charsLeft = (strlen(s) - k)+1;
      if( charsLeft < maxChars ) {
        for( int x = charsLeft-1; x < maxChars; x++ )
          buffer[x] = ' ';
      }     
      print( buffer, color, 1, y );
      delay(delaytime);
    }
    clear();
  }    
}

// -------------------------------------------------------------------------------------------
// Function Name: fillFontCodeBuffer
// Purpose: loads a charcter font code int local buffer
// Parameters:
//		chr		The desired character
// -------------------------------------------------------------------------------------------
void HT1632::fillFontCodeBuffer(char chr)
{
  char chars[CHAR_BUFFER];
  strcpy_P(chars, (char*)fontMap);  
  
  for(int i=0; chars[i]; i++)
  {
      if(chars[i] == chr)
      {
          memcpy(fontCode,fontChars[i], 5);
          return;
      }
  }
}

// -------------------------------------------------------------------------------------------
// Function Name: clearFontCodeBuffer
// Purpose: Clears the font code in local buffer
// -------------------------------------------------------------------------------------------
void HT1632::clearFontCodeBuffer()
{
  memset(fontCode, 0, sizeof(fontCode));
}
