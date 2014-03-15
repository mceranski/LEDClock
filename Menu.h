#ifndef MENU_H
#define MENU_H

#include "Menu.h"

class Menu
{  
  private:
    void (*_buttonCallback)(int);
    
  public:   
    char text[10];
    
    void setup();
    void setCallback(void (*buttonCallback)(int));
    void read();
};

#endif //MENU_H
