// albawinder v1.0
// lechacal.com

#define Pot 3
#define POS_LEFT 16
#define POS_RIGHT 15

#include <LiquidCrystal.h>
#include <Wire.h>
#include <stepperUNO.h>
#include <EEPROM.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

keypad Keypad;
stepMOTOR motor1, motor2;
//motor1 - Spooler
//motor2 - Carrier


long timer1 = 0;

const byte MODE_DISABLE = 0;
const byte MODE_MANUAL = 1;
const byte MODE_SYNC = 2;
const byte MODE_OTHER = 99;

byte MODE = MODE_DISABLE;
int incr = 1;
const float leadscrewpitch = 1.5;
float wiresize = 1.5;
float speed_ratio = leadscrewpitch / wiresize;
float step_delay;
int POTval, prev_val;
boolean update_speed;


void setup(){

  motor1.begin(0x4);
  motor2.begin(0x5);
  motor1.disable();
  motor2.disable();
  
  pinMode(POS_LEFT, INPUT);
  pinMode(POS_RIGHT, INPUT);
  
  lcd.begin(16,2); //LCD INITIALISATION
  
  update_lcd();
}

void loop(){
  
  if (!digitalRead(POS_RIGHT) && MODE == MODE_SYNC && motor2.direction){
    motor2.setDirection(LOW);
  }
  if (!digitalRead(POS_LEFT) && MODE == MODE_SYNC && !motor2.direction){
    motor2.setDirection(HIGH);
  }

 
 POTval = analogRead(Pot);
 if ((POTval>prev_val+15) || (POTval<prev_val-15)){
   
   update_speed = true;
   prev_val = POTval;
   //step_delay1 = 800000/(1024-val);
   step_delay = 1024000/(POTval+1);// if micro stepped
   //rpm1 = 60/(2*step_delay1*400/1000000);
   
   if (step_delay > 32767){// i2c to attiny85 only supports 2byte int. If bigger then we stop both motors
     step_delay = 0;
   }
   
   if (motor1.running){
     motor1.speed(step_delay);
   }
   if (motor2.running){
     motor2.speed(step_delay*speed_ratio);
   }
 }
 

 
 byte button = Keypad.ReadKey();
 
 if( Keypad.buttonJustPressed ) {

        switch (button){              

            case BUTTON_RIGHT:{  
              switch (MODE){
                case MODE_MANUAL:{
                 motor2.setDirection(true);
                 motor2.speed(2750);
                 motor2.speed(1750);
                 motor2.speed(750);
                 break;
                }//end case
                case MODE_SYNC:{
                  motor2.setDirection(true);
                  motor2.speed(step_delay*speed_ratio);
                  motor1.speed(step_delay);
                  //motor1.running = true;
                  //mot2_onrun = true;

                  break;
                }
              }
              break;
            }
            case BUTTON_LEFT:{     
              switch (MODE){
                case MODE_MANUAL:{
                 motor2.setDirection(false);
                 motor2.speed(2750);
                 motor2.speed(1750);
                 motor2.speed(750);
                 break;
                }//end case
                case MODE_SYNC:{
                  motor2.setDirection(false);
                  motor2.speed(step_delay*speed_ratio);
                  motor1.speed(step_delay);

                  break;
                }
              }
              break;
            }
            case BUTTON_UP:{
               switch (MODE){
                case MODE_MANUAL:{
                 motor1.setDirection(false);
                 motor1.speed(step_delay);
                 break;
                }//end case
                case MODE_SYNC:{
                  motor1.setDirection(false);

                  break;
                }
              }
              break;
            }
            case BUTTON_DOWN:{
               switch (MODE){
                case MODE_MANUAL:{
                 motor1.setDirection(true);
                 motor1.speed(step_delay);
                 //mot1_onrun = true;
                 break;
                }//end case
                case MODE_SYNC:{
                  motor1.setDirection(true);

                  break;
                }
              }
              break;
            }
            case BUTTON_SELECT:{     
                 MODE += 1;
                 if (MODE == 3){
                   MODE = MODE_DISABLE;  
                   motor1.disable();
                   motor2.disable();
                 }
                 else {
                   motor1.stop();
                   motor2.stop();
                   motor1.enable();
                   motor2.enable();
                 }
                 
             
                 break;
            }
        }//end switch
        update_lcd();
 }// end if
 
 if( Keypad.buttonJustPressed ) Keypad.buttonJustPressed = false; //We have now done all we wanted once the button pressed. Closing it.
 
 if( Keypad.buttonJustReleased ) {  
   switch (MODE) {
     case MODE_MANUAL:{
       motor1.stop();
       motor2.stop();
     }
   }

 }
 if( Keypad.buttonJustReleased ) Keypad.buttonJustReleased = false;
    
    

//  if (MODE==MODE_MANUAL) {
//     switch (button){              
//        case BUTTON_RIGHT:{  
//          if (millis() > timer1 + step_delay) {
//            timer1 = millis();
//            Position += 1;
//            motor1.step();
//            motor1.step();
//            update_position_lcd();
//          }
//          break;
//        }
//        case BUTTON_LEFT:{  
//          if (millis() > timer1 + step_delay) {
//            timer1 = millis();
//            Position -= 1;
//            motor1.step();
//            motor1.step();
//            update_position_lcd();
//          }
//          break;
//        }
//     } //END switch button
//  }//end if MODE_RUN

   Keypad.RstReadKey();
      
  if (Keypad.rstbuttonLongPress) {
    
        lcd.setCursor(0,0);
        lcd.print("Entering Menu...");
        lcd.setCursor(0,1);
        lcd.print("                ");
        while (digitalRead(BUTTON_RST_PIN)==false){
          delay(10);
        }
        boolean in_menu = true;
        int menu_page = 1;
        motor1.stop();
        
        while (in_menu){
          
          byte button = Keypad.ReadKey();
          
          switch (menu_page){   
 
             case 1:{            
              lcd.setCursor(0,0);
              lcd.print("* Wire Size     ");
              lcd.setCursor(0,1);
              lcd.print(wiresize);

              if (button == BUTTON_DOWN) {
                if( Keypad.buttonJustPressed) wiresize -= 0.01;
              }
              if (button == BUTTON_UP) {
                if( Keypad.buttonJustPressed) wiresize += 0.01;
              }
                  
                  break;
             }
             case 2:{
               lcd.setCursor(0,0);
               lcd.print("* Option 2      ");
               lcd.setCursor(0,1);
               lcd.print("                ");
               break;
               
             }
             case 3:{
               lcd.setCursor(0,0);
               lcd.print("* Option 3      ");
               lcd.setCursor(0,1);
               lcd.print("                ");
               break;
               
             }
             case 4:{
               lcd.setCursor(0,0);
               lcd.print("* Option 4      ");
               lcd.setCursor(0,1);
               lcd.print("                ");
               break;
               
             }
          }//END SWITCH

          
          
          if (button == BUTTON_LEFT){
           if( Keypad.buttonJustPressed) menu_page -= 1;
          }
          else if (button == BUTTON_RIGHT){
            if( Keypad.buttonJustPressed) menu_page += 1;
          }
          if (menu_page == 5){
            menu_page = 1;
          }
          else if (menu_page == -1){
            menu_page = 4;
          }
          
          Keypad.RstReadKey();
          //if (digitalRead(RST)==false){
          if (Keypad.rstbuttonJustPressed){
            in_menu = false;
            Keypad.rstbuttonJustPressed = false;
            //PAGE -= 1;
          }
          
          if( Keypad.buttonJustPressed ) Keypad.buttonJustPressed = false;
          
        }//END WHILE
        
        Keypad.rstbuttonLongPress = false;
        speed_ratio = leadscrewpitch / wiresize;
        update_lcd();
      }//END IF 




  
  
}//end loop


void update_lcd(){
  lcd.setCursor(0,0);
  switch (MODE){
    case MODE_DISABLE:{
      lcd.print("DISABLED        ");      
      break;
    }
    case MODE_MANUAL:{
      lcd.print("MANUAL          ");      
      break;
    }
        case MODE_SYNC:{
      lcd.print("SYNC            ");      
      break;
    }
        case MODE_OTHER:{
      lcd.print("OTHER           ");      
      break;
    }
  }//end switch
  
  
  lcd.setCursor(0,1);
  lcd.print("                ");

  
  
  
}//end function




void EEPROM_writefloat(int ee, float value)
{
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
}//end function

float EEPROM_readfloat(int ee)
{
    double value = 0.0;
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return value;
}//end function
  
