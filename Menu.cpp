#include <avr/pgmspace.h>
#include <Arduino.h>
#include "Menu.h"
#include "LEDClock.h"

int buttonStates[3] = {0,0,0};
int lastButtonStates[3] = {0,0,0};
int buttonPins[3] = { BTN_LEFT_PIN, BTN_MIDDLE_PIN, BTN_RIGHT_PIN };

void menuInit()
{
  for( int i = 0; i < 3; i ++ ) {
    pinMode( buttonPins[i], INPUT );
  }
}

void readButton( int index )
{
  // read the pushbutton input pin:
  buttonStates[index] = digitalRead(buttonPins[index]);

  // compare the buttonState to its previous state
  if (buttonStates[index] != lastButtonStates[index]) {
    // if the state has changed, increment the counter
    if (buttonStates[index] == HIGH) {
      Serial.println(index);
    }
  }
  // save the current state as the last state, 
  //for next time through the loop
  lastButtonStates[index] = buttonStates[index];
}

void menuRead() {
  for( int i = 0; i < 3; i ++ ) {
    readButton(i);
  }
}

