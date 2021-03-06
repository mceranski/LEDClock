#include <Arduino.h>
#include <Wire.h>
#include "LEDClock.h"
#include "RTClib.h"
#include "ht1632.h"
#include "Menu.h"

RTC_DS1307 RTC;
Menu menu;
DateTime lastDate;
HT1632 ledMatrix;
byte mode = MODE_CLOCK;

void setup()
{    
  Wire.begin();   
  RTC.begin();    
  ledMatrix.setup();
  
  menu.setup();
  menu.setCallback( onButtonPressed ); 
  
  if (! RTC.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }    

  pinMode( SPEAKER_PIN, OUTPUT );
}

void onButtonPressed(int index) 
{      
  ledMatrix.clear();  
  
  switch( index ) {
      case 0: { //toggle mode
        mode = (mode == MODE_MENU ? MODE_CLOCK : MODE_MENU);
        ledMatrix.print( menu.text, RED, 1, 0 );  
        break;
      } 
      case 1: { //up        
        DateTime future (RTC.now().unixtime() + 60); //move clock forward 1 minute
        RTC.adjust(future);    
        updateClock(future);               
        break;
      }
      case 2: { //down        
        DateTime past (RTC.now().unixtime() - 60); //move clock backward 1 minute
        RTC.adjust(past);    
        updateClock(past);
        break;
      }
  }
}

void updateClock( DateTime value )
{  
  lastDate = value;
  ledMatrix.clear();
  char buffer[15];
  sprintf(buffer, "%.3s", value.monthName());
  ledMatrix.print( buffer, RED, 1, 0 );
  sprintf(buffer, "%02d", value.day());
  ledMatrix.print( buffer, ORANGE, 19, 0 );
  sprintf(buffer, "%2d:%02d", value.twelveHourFormat(), value.minute());
  ledMatrix.print( buffer, GREEN, 1, 8 );  
}

void displayText(int arg_cnt, char **args)
{
  if( arg_cnt <= 1 ) return;
  
  char buffer[100];
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
  if( mode == MODE_CLOCK )
    displayClock(); 
  
  menu.read(); 
}

const char fifteen[] PROGMEM = "The time is now quarter after %s.";
const char thirty[] PROGMEM = "The time is now %s thirty.";
const char fortyFive[] PROGMEM = "The time is now %s forty five.";
const char oClock[] PROGMEM = "The time is now %s o'clock.";
const char today[] PROGMEM = "Today is %s %s %d %d.";

void displayClock()
{  
  if( lastDate.minute() == RTC.now().minute() ) return;  
  lastDate = RTC.now();
        
  if( lastDate.hour() == 16 && lastDate.minute() == 45 ) {
    ledMatrix.matrixEffect();
  }    
  
  if (lastDate.minute() % 5 == 0) {
    char buffer[75];        
    switch( (int)lastDate.minute() ) {
      case 15: {
        sprintf_P( buffer, fifteen, lastDate.hourName() );
        break;
      }
      case 30: {
        sprintf_P( buffer, thirty, lastDate.hourName() );
        break;
      }
      case 45: {
        sprintf_P( buffer, fortyFive, lastDate.hourName() );
        break;
      }
      case 0: {
        sprintf_P( buffer, oClock, lastDate.hourName() );
        break;
      }
      default: {
        sprintf_P( buffer, today, lastDate.dayName(), lastDate.monthName(), lastDate.day(), lastDate.year() );
        break;
      }      
    }
    ledMatrix.clear();
    ledMatrix.scrollText( buffer, ORANGE );
  }    

  updateClock( lastDate );
}
