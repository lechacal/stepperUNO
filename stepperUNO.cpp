//StepperUNO library
//Version 1.3


#include "Arduino.h"
#include "stepperUNO.h"
#include "Wire.h"

uint8_t keypad::apply_debounce(uint8_t BUTTON_INPUT, uint8_t button){
   	if (prev_instant_button == BUTTON_INPUT){
		if ((millis()-Dtime) > DEBOUNCE_DELAY)  {
			button = BUTTON_INPUT;
		}
	}
	else {
		Dtime = millis();
	}
   
	prev_instant_button = BUTTON_INPUT;
	return button;
}


uint8_t keypad::ReadKey()
{
   unsigned int buttonVoltage;
   uint8_t button = BUTTON_NONE;   // return no button pressed if the below checks don't write to btn
 
   //read the button ADC pin voltage
   buttonVoltage = analogRead( BUTTON_ADC_PIN );
   //sense if the voltage falls within valid voltage windows
   if( buttonVoltage < ( RIGHT_10BIT_ADC + SENSING_WINDOW ) )
   {
	button = apply_debounce(BUTTON_RIGHT, button);
   }
   else if(   buttonVoltage >= ( UP_10BIT_ADC - SENSING_WINDOW )
           && buttonVoltage <= ( UP_10BIT_ADC + SENSING_WINDOW ) )
   {
      button = apply_debounce(BUTTON_UP, button);
   }
   else if(   buttonVoltage >= ( DOWN_10BIT_ADC - SENSING_WINDOW )
           && buttonVoltage <= ( DOWN_10BIT_ADC + SENSING_WINDOW ) )
   {
      button = apply_debounce(BUTTON_DOWN, button);
   }
   else if(   buttonVoltage >= ( LEFT_10BIT_ADC - SENSING_WINDOW )
           && buttonVoltage <= ( LEFT_10BIT_ADC + SENSING_WINDOW ) )
   {
      button = apply_debounce(BUTTON_LEFT, button);
   }
   else if(   buttonVoltage >= ( SELECT_10BIT_ADC - SENSING_WINDOW )
           && buttonVoltage <= ( SELECT_10BIT_ADC + SENSING_WINDOW ) )
   {
      button = apply_debounce(BUTTON_SELECT, button);
   }
   else {
   	prev_instant_button = BUTTON_NONE;
   }
   
   
   //handle button flags for just pressed and just released events
   if( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) )
   {
      //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
      //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
      buttonJustPressed  = true;
      buttonJustReleased = false;
   }
   
   
   if( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) )
   {
      buttonJustPressed  = false;
      buttonJustReleased = true;
   }
 
   //save the latest button value, for change event detection next time round
   buttonWas = button;
 
   return button;
}


bool keypad::RstReadKey()
{

   bool button = digitalRead( BUTTON_RST_PIN );

   if (button != rstbuttonWas) {
       rstDtime = millis();
       rstbuttonWas = button;
   }
   

   if ((millis()-rstDtime) > DEBOUNCE_DELAY && (millis()-rstDtime) < 2000){
     if (button != rststate){
       rststate = button;
       if (rststate == false){
         rstbuttonJustPressed  = true;
         rstbuttonJustReleased = false;
         rstbuttonLongPress    = false;
       }
       else {
         rstbuttonJustPressed  = false;
         rstbuttonJustReleased = true;
         rstbuttonLongPress    = false;
       }
    }
   }

   else if ((millis()-rstDtime)>=2000){
     if (button == false){
       if (rststate == false){
         rstbuttonJustPressed  = false;
         rstbuttonJustReleased = false;
         rstbuttonLongPress    = true;
       }
       else {
         rstbuttonJustPressed  = false;
         rstbuttonJustReleased = true;
         rstbuttonLongPress    = false;
       }
     }

   }

 
   //save the latest button value, for change event detection next time round

 
   return( button );
}



void stepMOTOR::begin(uint8_t _address){
    Wire.begin();
    address = _address;
    running = false;
    direction = false;
}

void stepMOTOR::speed(int speed_m1){
    String sp = String(speed_m1);
    sp += ":";
    char charBuf[sp.length()+1];
    sp.toCharArray(charBuf, sp.length()+1);
    Wire.beginTransmission(address); 
    Wire.write(charBuf);    
    Wire.endTransmission();
    running = true;

}

void stepMOTOR::setDirection(bool dir){
    Wire.beginTransmission(address); 
    if (dir){
      Wire.write("a");
      direction = true; 
    } 
   else {
      Wire.write("b");  
      direction = false;
    }     
    Wire.endTransmission();
}

void stepMOTOR::step(){
    Wire.beginTransmission(address); 
    Wire.write(".");  
    Wire.endTransmission();
}

void stepMOTOR::enable(){
    Wire.beginTransmission(address); 
    Wire.write("e");    
    Wire.endTransmission();
}

void stepMOTOR::disable(){
    Wire.beginTransmission(address); 
    Wire.write("d");    
    Wire.endTransmission();
}

void stepMOTOR::stop(){
    Wire.beginTransmission(address); 
    Wire.write(":");   
    Wire.endTransmission();
    running = false;
}

