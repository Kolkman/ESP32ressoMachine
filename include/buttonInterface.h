
#pragma once
#ifndef ButtonInterface_h
#define ButtonInterface_h

#include "ESPressoMachineDefaults.h"
#ifdef ENABLE_BUTTON

#ifndef BLACK_BUTTON
#define BLACK_BUTTON 18
#endif

#ifndef BLUE_BUTTON
#define BLUE_BUTTON 16
#endif

#ifndef RED_BUTTON
#define RED_BUTTON 17
#endif


#ifndef SINGLEPRESS_T
#define SINGLEPRESS_T 200 // miliseconds hold will be seen as a single button press
#endif

#ifndef LONGPRESS_T
#define LONGPRESS_T 3000 // miliseconds hold will be seen as a long button press
#endif




#define LCD_PAGE ((LCD_COLUMS) * (LCD_ROWS))
class ButtonInterface
{
public:
    ButtonInterface();
    void setupButton();
    void loopButton(ESPressoMachine *);

private:
    unsigned long BlueStartPress;
    unsigned long RedStartPress;
    unsigned long  BlackStartPress;
      unsigned long BlueLastPress;
    unsigned long RedLastPress;
    unsigned long  BlackLastPress;
};

#endif
#endif