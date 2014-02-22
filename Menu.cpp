#include <avr/pgmspace.h>
#include <Arduino.h>
#include "Menu.h"
#include "LEDClock.h"

Menu::Menu()
{
  buttonPin[0] = BTN_LEFT_PIN;
  buttonPin[1] = BTN_MIDDLE_PIN;
  buttonPin[2] = BTN_RIGHT_PIN;
  
  for( int i = 0; i < 3; i ++ ) {
    buttonState[i] = 1;
  }
}

void Menu::setup()
{
  for( int i =0; i < 3; i ++ )
    pinMode( buttonPin[i], INPUT );  
}

void Menu::check( int index )
{
  // read the pushbutton input pin:
  buttonState[index] = digitalRead(buttonPin[index]);

  // compare the buttonState to its previous state
  if (buttonState[index] != lastButtonState[index]) {
    // if the state has changed, increment the counter
    if (buttonState[index] == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      Serial.println(index);
    } 
  }
  // save the current state as the last state, 
  //for next time through the loop
  lastButtonState[index] = buttonState[index];
}

void Menu::checkAll() {
  for( int i =0; i < 3; i ++ ) {
    check(i);
  }
}

