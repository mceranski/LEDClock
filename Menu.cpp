#include "Menu.h" 
#include "LEDClock.h" 
#include <Arduino.h>
#include <avr/pgmspace.h>

const int buttonPin[3]= {BTN_LEFT_PIN, BTN_MIDDLE_PIN, BTN_RIGHT_PIN};
int buttonState[3] = {0, 0, 0};
int lastButtonState[3] = {0, 0, 0}; 

prog_char miDemo[] PROGMEM = "DEMO";
PROGMEM const char *menu_items[] = {   
  miDemo 
};

void Menu::setup()
{  
   pinMode( BTN_LEFT_PIN, INPUT );  
   pinMode( BTN_MIDDLE_PIN, INPUT );  
   pinMode( BTN_RIGHT_PIN, INPUT );
}

void Menu::read()
{  
  for( int i =0; i < 3; i++ ) {
    // read the pushbutton input pin:
    buttonState[i] = digitalRead(buttonPin[i]);
  
    // compare the buttonState to its previous state
    if (buttonState[i] != lastButtonState[i]) {
      // if the current state is HIGH then the button went from off to on:
      if (buttonState[i] == HIGH) {
        //copy the selected menu items text to the text property
        strcpy_P(text, (char*)pgm_read_word(&(menu_items[0])));
        //save the index of the button that was pressed
        _buttonCallback(i);
      } 
    }
    // save the current state as the last state, 
    //for next time through the loop
    lastButtonState[i] = buttonState[i];
  }
}

void Menu::setCallback(void (*buttonCallback)(int))
{
  _buttonCallback = buttonCallback;
}
