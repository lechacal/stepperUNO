// simpledrive v1.2
// lechacal.com
// Example usage of stepperUNO library
// drive a single motor with left/right button

#define Pot 3

#include <LiquidCrystal.h>
#include <Wire.h>
#include <stepperUNO.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

keypad Keypad;
stepMOTOR motor1;

void setup(){

  motor1.begin(0x5); // Motor 1 is at address 0x5. Motor 2 on 0x4
  motor1.enable();

  lcd.begin(16,2); //LCD INITIALISATION
}

void loop(){
 
 int POTval = analogRead(Pot);
 float step_delay = 1024000./(POTval+1.);
 
 byte button = Keypad.ReadKey();
 
 if( Keypad.buttonJustPressed ) {

        switch (button){              

            case BUTTON_RIGHT:{  
                 motor1.setDirection(true);
                 motor1.speed(step_delay);
                 lcd.setCursor(0,0);
                 lcd.print("RIGHT");
                 break;
            }
            case BUTTON_LEFT:{     
                 motor1.setDirection(false);
                 motor1.speed(step_delay);
                 lcd.setCursor(0,0);
                 lcd.print("LEFT ");
                 break;
            }
        }//end switch
        
        Keypad.buttonJustPressed = false;// Must set this flag once we are done with the button pressed
 }// end if
 
 if( Keypad.buttonJustReleased ) {  
   if (button==BUTTON_NONE){
     motor1.stop();
     lcd.setCursor(0,0);
     lcd.print("READY");
   }
   Keypad.buttonJustReleased = false;// Must set this flag once we are done with the button released
 }  
  
}//end loop
