#ifndef stepperUNO_h
#define stepperUNO_h

#include "config.h"

#include "Arduino.h"


#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define BUTTON_RST_PIN            3
// ADC readings expected for the 5 buttons on the ADC input

#if KEYPAD_VERSION == 1
	#define RIGHT_10BIT_ADC           0  // right
	#define UP_10BIT_ADC            100  // up
	#define DOWN_10BIT_ADC          256  // down
	#define LEFT_10BIT_ADC          410  // left
	#define SELECT_10BIT_ADC        640  // select
#elif KEYPAD_VERSION == 2
	#define RIGHT_10BIT_ADC           0  // right
	#define UP_10BIT_ADC            140  // up
	#define DOWN_10BIT_ADC          340  // down
	#define LEFT_10BIT_ADC          510  // left
	#define SELECT_10BIT_ADC        730  // select
#else
	#define RIGHT_10BIT_ADC           0  // right
	#define UP_10BIT_ADC            130  // up
	#define DOWN_10BIT_ADC          305  // down
	#define LEFT_10BIT_ADC          480  // left
	#define SELECT_10BIT_ADC        720  // select
#endif


#define SENSING_WINDOW         50  // button sensing window
#define DEBOUNCE_DELAY           20  // Debounce delay for SELECT/UP/DOWN/LEFT/RIGHT
//return values for ReadButtons()
#define BUTTON_NONE               0  // 
#define BUTTON_RIGHT              1  // 
#define BUTTON_UP                 2  // 
#define BUTTON_DOWN               3  // 
#define BUTTON_LEFT               4  // 
#define BUTTON_SELECT             5  // 


class keypad
{
  public:
    //keypad();
    uint8_t ReadKey();
    bool RstReadKey();
    bool buttonJustPressed  = false;    //this will be true after a ReadButtons() call if triggered
    bool buttonJustReleased = false;    //this will be true after a ReadButtons() call if triggered
    bool rstbuttonJustPressed  = false;    //this will be true after a ReadButtons() call if triggered
    bool rstbuttonJustReleased = false;    //this will be true after a ReadButtons() call if triggered
    bool rstbuttonLongPress    = false;
  private:
    uint8_t buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events
    bool rstbuttonWas      = true;
    long Dtime             = 0;
    long rstDtime          = 0;
    bool rststate          = true;
    uint8_t prev_instant_button = BUTTON_NONE;
    uint8_t apply_debounce(uint8_t BUTTON_INPUT, uint8_t button);

};

class stepMOTOR
{
  public:
    void speed(int speed_m1);
    void setDirection(bool dir);
    void enable();
    void disable();
    void step();
    void stop();
    uint8_t address;
    void begin(uint8_t _address);
    bool running;
    bool direction;

  private:

};

#endif
