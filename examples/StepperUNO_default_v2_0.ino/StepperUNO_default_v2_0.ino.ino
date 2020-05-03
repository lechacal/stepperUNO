// StepperUNO Default v2.0.0
// lechacal.com

// Libraries:
// stepperUNO v1.2
// http://lechacal.com/stepperUNO/stepperUNO_v1_2.zip

//***************************
// GENERAL PARAMETERS
//***************************
const boolean X_DIR = false; // set to true to change direction
const boolean Y_DIR = false;
const long POT_RANGE_1 = 512000;// Modify speed range from the pot using this parameter
const long POT_RANGE_2 = 512000;// reasonable range between 256000 and 2048000
//END SWITCH PORT
#define END_LEFT  10 // END SWITCH LEFT on D10
#define END_RIGHT 11
#define END_UP    12
#define END_DOWN  13
//#define END_XXX 15 // labelled A1
//#define END_XXX 16 // labelled A2

//***************************
// INITIALISATIONS
//***************************
#define Pot 3
//#define KEYPAD_VERSION 1

#include <LiquidCrystal.h>
#include <Wire.h>
#include <stepperUNO.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

keypad Keypad;
byte wasbutton; // for keypad usage
stepMOTOR motor1, motor2;
//motor1 - Y axis - UP DOWN
//motor2 - X axis - LEFT RIGHT

//long timer1 = 0;

const byte MODE_DISABLED = 0; // Free running motors
const byte MODE_HOLD = 1; // Holding torque - SPeed = 0
const byte MODE_RUN = 2; // Motor travelling.
const byte MODE_OTHER = 99;
byte MODE = MODE_DISABLED;

long step_delay_1; // Parameter controlling speed. This is the number of microseconds between steps.
long step_delay_2;
unsigned int POTval, prev_val; // Potentiometer values.
const byte pot_delta = 5; //Potentiometer sensitivity
boolean update_speed; // For pot usage

boolean disable_left = false; // If end switch activated. Flag a direction a disabled.
boolean disable_right = false;
boolean disable_up = false;
boolean disable_down = false;



//***************************
// SETUP
//***************************
void setup() {
  pinMode(END_LEFT, INPUT); // Pin configuration as input
  pinMode(END_RIGHT, INPUT);
  pinMode(END_UP, INPUT);
  pinMode(END_DOWN, INPUT);

  char_definitions(); // For drawing the arrows on LCD
  lcd.begin(16, 2); //LCD INITIALISATION
  
  motor1.begin(0x5); // Motor 1 at address 0x5
  motor2.begin(0x4);

  status_disabled(); // We start disabled.

}


//***************************
// MAIN LOOP
//***************************

void loop() {

  //***************************
  // END SWITCH CONTROL
  //***************************

  if (digitalRead(END_LEFT)) {
    if (!disable_left) {
      if (MODE == MODE_RUN) {
        //status_hold();// Uncomment to enable stop switch
      }

      //disable_left = true;// Uncomment to enable stop switch
      lcd.setCursor(0, 0);
      lcd.print((char)255);
      lcd.setCursor(0, 1);
      lcd.print((char)255);
    }
  }
  else {
    if (disable_left) {
      disable_left = false;
      lcd.setCursor(0, 0);
      lcd.print(' ');
      lcd.setCursor(0, 1);
      lcd.print(' ');
    }
  }

  if (digitalRead(END_RIGHT)) {
    if (!disable_right) {
      if (MODE == MODE_RUN) {
        //status_hold();// Uncomment to enable stop switch
      }
      //disable_right = true;// Uncomment to enable stop switch
      lcd.setCursor(15, 0);
      //lcd.write((uint8_t)6);
      lcd.print((char)255);
      lcd.setCursor(15, 1);
      //lcd.write((uint8_t)6);
      lcd.print((char)255);
    }
  }
  else {
    if (disable_right) {
      disable_right = false;
      lcd.setCursor(15, 0);
      lcd.print(' ');
      lcd.setCursor(15, 1);
      lcd.print(' ');
    }
  }

  if (digitalRead(END_DOWN)) {
    if (!disable_down) {
      if (MODE == MODE_RUN) {
        //status_hold();// Uncomment to enable stop switch
      }
      //disable_down = true;// Uncomment to enable stop switch
      lcd.setCursor(1, 1);
      lcd.write((uint8_t)6);
      lcd.write((uint8_t)6);
      lcd.setCursor(13, 1);
      lcd.write((uint8_t)6);
      lcd.write((uint8_t)6);
    }
  }
  else {
    if (disable_down) {
      disable_down = false;
      lcd.setCursor(1, 1);
      lcd.print(' ');
      lcd.print(' ');
      lcd.setCursor(13, 1);
      lcd.print(' ');
      lcd.print(' ');
    }
  }

  if (digitalRead(END_UP)) {
    if (!disable_up) {
      if (MODE == MODE_RUN) {
        //status_hold(); // Uncomment to enable stop switch
      }
      //disable_up = true;// Uncomment to enable stop switch
      lcd.setCursor(1, 0);
      lcd.write((uint8_t)7);
      lcd.write((uint8_t)7);
      lcd.setCursor(13, 0);
      lcd.write((uint8_t)7);
      lcd.write((uint8_t)7);
    }
  }
  else {
    if (disable_up) {
      disable_up = false;
      lcd.setCursor(1, 0);
      lcd.print(' ');
      lcd.print(' ');
      lcd.setCursor(13, 0);
      lcd.print(' ');
      lcd.print(' ');
    }
  }

  //***************************
  // POTENTIOMETER READ
  //***************************

  POTval = analogRead(Pot);
  // if the pot has moved by more than 15 units
  if ((POTval > prev_val + pot_delta) || (POTval < prev_val - pot_delta)) {
    update_speed = true;
    prev_val = POTval;
    step_delay_1 = POT_RANGE_1 / (POTval + 1); // if micro stepped
    //float rpm1 = 60./(2.*step_delay_1*180./1000000.);
    step_delay_2 = POT_RANGE_2 / (POTval + 1);

    // i2c to attiny85 only supports unsigned int. If out of range then we stop both motors
    if (step_delay_1 < 0 || step_delay_1 > 65535) step_delay_1 = 0;
    if (step_delay_2 < 0 || step_delay_2 > 65535) step_delay_2 = 0;

    if (motor1.running) {
      motor1.speed(step_delay_1);
    }
    if (motor2.running) {
      motor2.speed(step_delay_2);
    }

    // Below is to show the speed in RPM
    //lcd.setCursor(10, 0);
    //lcd.print(step_delay_1);
    //lcd.setCursor(10, 1);
    //lcd.print(POTval);
    //lcd.print(rpm1,1);


  }

  //***************************
  // KEYPAD READ & PROCESS
  //***************************


  byte button = Keypad.ReadKey(); // Read keypad

  //Only making action once the button has been released. Not pressed.
  if ( Keypad.buttonJustPressed ) {
    wasbutton = button; // recording the activated button
    //    switch (button) {
    //    case BUTTON_RIGHT:
    //      {
    //      }
    //    case BUTTON_LEFT:
    //      {
    //      }
    //    case BUTTON_DOWN:
    //      {
    //      }
    //    case BUTTON_SELECT:
    //      {
    //      }
    //    }

    Keypad.buttonJustPressed = false; //We have now done all we wanted once the button pressed. Closing it.
  }// end if


  // This is where action happen below. Button has been released.
  if ( Keypad.buttonJustReleased ) {

    switch (wasbutton) {

      case BUTTON_RIGHT:
        {
          switch (MODE) {
            case MODE_DISABLED:
              {
                //Nothing here
                break;
              }
            case MODE_HOLD:
              {
                MODE = MODE_RUN;
                if (!disable_right) {
                  motor2.setDirection(X_DIR);
                  motor2.speed(step_delay_2);
                  lcd.setCursor(6, 0);
                  lcd.print("==>>");
                }
                break;
              }
            case MODE_RUN:
              {
                MODE = MODE_HOLD;
                status_hold();
                break;
              }
          }
          break;
        }//end case


      case BUTTON_LEFT:
        {
          switch (MODE) {
            case MODE_DISABLED:
              {
                break;
              }
            case MODE_HOLD:
              {
                MODE = MODE_RUN;
                if (!disable_left) {
                  motor2.setDirection(!X_DIR);
                  motor2.speed(step_delay_2);
                  lcd.setCursor(6, 0);
                  lcd.print("<<==");
                }
                break;
              }
            case MODE_RUN:
              {
                MODE = MODE_HOLD;
                status_hold();
                break;
              }
          }
          break;
        }
      case BUTTON_UP:
        {
          switch (MODE) {
            case MODE_DISABLED:
              {
                break;
              }
            case MODE_HOLD:
              {
                MODE = MODE_RUN;
                if (!disable_up) {
                  motor1.setDirection(Y_DIR);
                  motor1.speed(step_delay_1);
                  lcd_up_arrow();
                }
                break;
              }
            case MODE_RUN:
              {
                MODE = MODE_HOLD;
                status_hold();
                break;
              }
          }
          break;
        }
      case BUTTON_DOWN:
        {
          switch (MODE) {
            case MODE_DISABLED:
              {
                break;
              }
            case MODE_HOLD:
              {
                MODE = MODE_RUN;
                if (!disable_down) {
                  motor1.setDirection(!Y_DIR);
                  motor1.speed(step_delay_1);
                  lcd_down_arrow();
                }
                break;
              }
            case MODE_RUN:
              {
                MODE = MODE_HOLD;
                status_hold();
                break;
              }
          }
          break;
        }

      case BUTTON_SELECT:
        {
          switch (MODE) {
            case MODE_DISABLED:
              {
                MODE = MODE_HOLD;
                status_hold();
                break;
                break;
              }
            case MODE_HOLD:
              {
                status_disabled();
                break;
              }
            case MODE_RUN:
              {
                MODE = MODE_HOLD;
                status_hold();
                break;
              }
          }
          break;

        }

    }

    Keypad.buttonJustReleased = false; // Closing the flag as we are done with the button release.
  }

  Keypad.RstReadKey(); // Reading the 'RST' key. Rightmost button

  //if (false) {
  if (Keypad.rstbuttonJustPressed) {
    status_disabled();
    Keypad.rstbuttonJustPressed = false;
  }//END IF


}//end loop

void status_disabled() {
  MODE = MODE_DISABLED;
  motor1.stop();
  motor2.stop();
  motor1.disable();
  motor2.disable();
  lcd_print_disabled();
}

void status_hold() {
  MODE = MODE_HOLD;
  motor1.stop();
  motor2.stop();
  motor1.enable();
  motor2.enable();
  lcd_clear_mid();

}

void char_definitions() {
  byte cc_0[8] = {//mid arrow left up
    0b00001,
    0b00011,
    0b00101,
    0b01001,
    0b10001,
    0b00001,
    0b00001,
    0b00001
  };
  lcd.createChar(0, cc_0);
  byte cc_1[8] = {//mid arrow right up
    0b10000,
    0b11000,
    0b10100,
    0b10010,
    0b10001,
    0b10000,
    0b10000,
    0b10000
  };
  lcd.createChar(1, cc_1);
  byte cc_2[8] = {//full right
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b00001
  };
  lcd.createChar(2, cc_2);
  byte cc_3[8] = {//full left
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000
  };
  lcd.createChar(3, cc_3);
  byte cc_4[8] = {//mid arrow left down
    0b00001,
    0b00001,
    0b00001,
    0b10001,
    0b01001,
    0b00101,
    0b00011,
    0b00001
  };
  lcd.createChar(4, cc_4);
  byte cc_5[8] = {//mid arrow right down
    0b10000,
    0b10000,
    0b10000,
    0b10001,
    0b10010,
    0b10100,
    0b11000,
    0b10000
  };
  lcd.createChar(5, cc_5);
  byte cc_6[8] = { // full block
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  lcd.createChar(6, cc_6);
  byte cc_7[8] = { // up block
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcd.createChar(7, cc_7);


}

void lcd_up_arrow() {
  lcd.setCursor(6, 0);
  lcd.print(' ');
  lcd.write((uint8_t)0);
  lcd.write((uint8_t)1);
  lcd.print(' ');
  lcd.setCursor(7, 1);
  lcd.write((uint8_t)2);
  lcd.write((uint8_t)3);
}
void lcd_down_arrow() {
  lcd.setCursor(6, 0);
  lcd.print(' ');
  lcd.write((uint8_t)2);
  lcd.write((uint8_t)3);
  lcd.print(' ');
  lcd.setCursor(7, 1);
  lcd.write((uint8_t)4);
  lcd.write((uint8_t)5);
}

void lcd_clear_mid() {
  lcd.setCursor(4, 0);
  lcd.print("  HOLD  ");
  lcd.setCursor(4, 1);
  lcd.print("        ");

}

void lcd_print_disabled() {
  lcd.setCursor(4, 0);
  lcd.print("DISABLED");
  lcd.setCursor(7, 1);
  lcd.print("  ");
}




