#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include "ht1632.h"
#include "Cmd.h"

#define SPEAKER_PIN 8  
#define BAUD    (9600)
#define MAX_BUF (64)
#define MODE_CLOCK  0
#define MODE_COMMAND  1
#define MODE_MENU 2
#define BTN_LEFT 7
#define BTN_MIDDLE 6
#define BTN_RIGHT 5

RTC_DS1307 RTC;
DateTime lastDate;
HT1632 ledMatrix;

byte mode = MODE_CLOCK;

void setup()
{  
  //must set the delimiter to carriage return for this to work
  cmdInit(BAUD);
  cmdAdd("text", displayText);

  Wire.begin();   
  RTC.begin();  
  
  ledMatrix.setup();
  
  if (! RTC.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }    
  
  pinMode( SPEAKER_PIN, OUTPUT );   
}

void displayText(int arg_cnt, char **args)
{
  if( arg_cnt <= 1 ) return;
  
  char buffer[128];
  //clear out the buffer
  memset( buffer, 0, sizeof(buffer) );
  
  //concantenate the text together to get one big message
  for (int i=1; i<arg_cnt; i++) {
    strcat( buffer, args[i] );    
    strcat( buffer, " " );    
  }
    
  //display the entire string
  ledMatrix.scrollText( buffer, RED );    
  lastDate = DateTime(0); //will cause the display to update when this exits
}

void loop ()
{   
  if( mode == MODE_CLOCK ) {
    updateDisplay(); 
  }

  cmdPoll();   
}

void beep( int times = 1, int delaytime = 100 ){
  for( int i = 0; i < times; i++ ) {
    analogWrite(5, 20);  // Almost any value can be used except 0 and 255 
    delay(delaytime);    // wait for a delay
    analogWrite(5, 0);   // 0 turns it off
    delay(delaytime);    // wait for a delay   
  }
}  

void updateDisplay()
{  
  if( lastDate.minute() == RTC.now().minute() ) return;  
  lastDate = RTC.now();
        
  if( lastDate.hour() == 16 && lastDate.minute() == 45 ) {
    ledMatrix.matrixEffect();
  }    
  
  if (lastDate.minute() % 5 == 0) {
    char buffer[100];        
    switch( (int)lastDate.minute() ) {
      case 15: {
        sprintf( buffer, "The time is now quarter after %s.", lastDate.hourName() );
        break;
      }
      case 30: {
        sprintf( buffer, "The time is now %s thirty.", lastDate.hourName() );
        break;
      }
      case 45: {
        sprintf( buffer, "The time is now %s forty five.", lastDate.hourName() );
        break;
      }
      case 0: {
        sprintf( buffer, "The time is now %s o'clock.", lastDate.hourName() );
        break;
      }
      default: {
        sprintf( buffer, "Today is %s %s %d %d.", lastDate.dayName(), lastDate.monthName(), lastDate.day(), lastDate.year() );
        break;
      }      
    }
    ledMatrix.clear();
    ledMatrix.scrollText( buffer, ORANGE );
  }    
    
  ledMatrix.clear();
  char buffer[15];
  sprintf(buffer, "%.3s", lastDate.monthName());
  ledMatrix.print( buffer, RED, 1, 0 );
  sprintf(buffer, "%02d", lastDate.day());
  ledMatrix.print( buffer, ORANGE, 19, 0 );
  sprintf(buffer, "%2d:%02d", lastDate.twelveHourFormat(), lastDate.minute());
  ledMatrix.print( buffer, GREEN, 1, 8 ); 
}