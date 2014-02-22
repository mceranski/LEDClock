  #include <avr/pgmspace.h>
#include <Arduino.h>
#include "Menu.h"
#include "LEDClock.h"

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 1;         // current state of the button
int lastButtonState = 1; 

void menuInit()
{
  pinMode( BTN_MIDDLE_PIN, INPUT );  
}

void readButton( int index )
{
  // read the pushbutton input pin:
  buttonState = digitalRead(BTN_MIDDLE_PIN);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonPushCounter++;      
      Serial.println(buttonPushCounter);
    } 
  }
  // save the current state as the last state, 
  //for next time through the loop
  lastButtonState = buttonState;

}

void menuRead() {
    readButton(0);
}

