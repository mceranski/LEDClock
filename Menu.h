#ifndef MENU_H
#define MENU_H

class Menu {
private:
  int buttonPin[3];
  int buttonState[3];        //current state of the button
  int lastButtonState[3];    //last state of the button 
public:
  Menu();
  void setup();
  void check( int index );
  void checkAll();
};

#endif
