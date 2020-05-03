// StepperUNO Position v2.0
// lechacal.com
// Example usage of stepperUNO library
//
// Position Control of both motors
// Ability to store and recall positions in memory
//
//
//Select button is used to switch more
//Left button runs left and store recall on position A.
//Right button runs right and store recall on position B.

// Required libraries:
//
// stepperUNO
// http://lechacal.com/stepperUNO/stepperUNO_v1_3.zip

#define Pot 3

#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <stepperUNO.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

keypad Keypad;
stepMOTOR motor1, motor2;

long Position = 0;
long goto_Position;

long timer1 = 0;

const byte MODE_DISABLE = 0;
const byte MODE_RUN = 1;
const byte MODE_STORE = 2;
const byte MODE_RECALL = 3;
const byte MODE_AUTO = 4;

long P[32];                 // array storing position being stored
uint8_t P_IDX = 0;          // index for the above array
uint16_t P_eeprom_idx = 32; // location of current position in eeprom

byte MODE = MODE_DISABLE;
int incr = 1;

void setup() {

  motor1.begin(0x4);
  motor2.begin(0x5);
  motor1.disable();
  motor2.disable();

  lcd.begin(16, 2); //LCD INITIALISATION

  // Reading all values from eeprom
  for (uint16_t i = 0; i < 32; i++) {
    P[i] = load_long_of_eeprom(i);
  }

  Position = load_long_of_eeprom(P_eeprom_idx);

  update_lcd();
  update_position_lcd();
}

void loop() {

  int POTval = analogRead(Pot);
  float step_delay = 1024 / (POTval + 1);
  // step_delay = 100; // uncomment for debug

  byte button = Keypad.ReadKey();

  if ( Keypad.buttonJustPressed ) {

    switch (button) {

      case BUTTON_RIGHT: {
          switch (MODE) {
            case MODE_RUN: {
                motor1.setDirection(true);
                motor2.setDirection(true);
                break;
              }//end case
          }
          break;
        }
      case BUTTON_LEFT: {
          switch (MODE) {
            case MODE_RUN: {
                motor1.setDirection(false);
                motor2.setDirection(false);
                break;
              }//end case

          }
          break;
        }
      case BUTTON_SELECT: {
          if (MODE == 4) MODE = MODE_DISABLE;
          else {
            MODE += 1;
            if (MODE == 4) MODE = MODE_DISABLE;
          }
          if (MODE == MODE_DISABLE) {
            motor1.disable();
            motor2.disable();
          }
          else {
            motor1.enable();
            motor2.enable();
          }


          break;
        }
      case BUTTON_UP: {
          P_IDX++;
          if (P_IDX == 32) {
            P_IDX = 0;
          }
          break;
        }
      case BUTTON_DOWN: {
          if (P_IDX == 0) {
            P_IDX = 32;
          }
          P_IDX--;
          break;
        }
    }//end switch
    update_lcd();
  }// end if

  if ( Keypad.buttonJustPressed ) Keypad.buttonJustPressed = false; //We have now done all we wanted once the button pressed. Closing it.

  if ( Keypad.buttonJustReleased ) {
    Keypad.buttonJustReleased = false;
  }


  Keypad.RstReadKey(); // Reading the 'RST' key. Rightmost button

  if (Keypad.rstbuttonJustPressed) {
    switch (MODE) {
      case MODE_STORE: {
          P[P_IDX] = Position;
          write_long_to_eeprom(P[P_IDX], P_IDX);
          update_lcd();
          break;
        }
      case MODE_RECALL: {
          do_recall(P[P_IDX]);
          break;
        }

    }

    Keypad.rstbuttonJustPressed = false;
  }//END IF

  if (Keypad.rstbuttonLongPress) {
    Position = 0;
    update_position_lcd();
    Keypad.rstbuttonLongPress = false;
  }




  if (MODE == MODE_RUN) {
    switch (button) {
      case BUTTON_RIGHT: {
          if (millis() > timer1 + step_delay) {
            timer1 = millis();
            Position += 1;
            motor1.step();
            motor1.step();
            motor2.step();
            motor2.step();
            update_position_lcd();
          }
          break;
        }
      case BUTTON_LEFT: {
          if (millis() > timer1 + step_delay) {
            timer1 = millis();
            Position -= 1;
            motor1.step();
            motor1.step();
            motor2.step();
            motor2.step();
            update_position_lcd();
          }
          break;
        }
    } //END switch button
  }//end if MODE_RUN

  else if (MODE == MODE_AUTO) {

    if (Position != goto_Position) {

      if (millis() > timer1 + step_delay) {
        timer1 = millis();
        Position += incr;
        motor1.step();
        motor1.step();
        motor2.step();
        motor2.step();
        update_position_lcd();
      }//end if

    }//end if
    else {
      MODE = MODE_RECALL;
      update_lcd();
    }
  }//end if MODE_AUTO



}//end loop


void update_lcd() {
  lcd.setCursor(0, 0);
  switch (MODE) {
    case MODE_DISABLE: {
        lcd.print("DIS ");
        break;
      }
    case MODE_RUN: {
        lcd.print("MANU");
        break;
      }
    case MODE_STORE: {
        lcd.print("STO ");
        break;
      }
    case MODE_RECALL: {
        lcd.print("CALL");
        break;
      }
    case MODE_AUTO: {
        lcd.print("AUTO");
        break;
      }
  }//end switch


  //update_position_lcd();

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("P");
  lcd.print(P_IDX);
  lcd.print(" ");
  lcd.print(P[P_IDX]);

}//end function

void update_position_lcd() {
  lcd.setCursor(11, 0);
  lcd.print("    ");
  lcd.setCursor(11, 0);
  lcd.print(Position);

  write_long_to_eeprom(Position, P_eeprom_idx);

}


void do_recall(long stored_pos) {
  if (Position > stored_pos) {
    motor1.setDirection(false);
    motor2.setDirection(false);
    incr = -1;
  }
  else {
    motor1.setDirection(true);
    motor2.setDirection(true);
    incr = 1;
  }

  MODE = MODE_AUTO;

  goto_Position = stored_pos;

  update_lcd();


}


long load_long_of_eeprom(uint16_t nidx) {
  long number;
  uint16_t e = nidx * 4;
  char * N_arr = (char *) &number;
  for (uint16_t j = 0; j < 4; j++) {
    N_arr[j] = EEPROM.read(e + j);
  }
  return number;
}


void write_long_to_eeprom(long number, uint16_t nidx) {
  uint16_t e = nidx * 4;
  char * N_arr = (char *) &number;
  for (uint16_t j = 0; j < 4; j++) {
    EEPROM.write(e + j, N_arr[j]);
  }
}
