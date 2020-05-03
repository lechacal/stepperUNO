// LCD Calibration v1.0
// lechacal.com


#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

unsigned int a;


void setup(){
  
  lcd.begin(16,2); //LCD INITIALISATION
  //Serial.begin(19200);
}

void loop(){
 
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,0);
  a = analogRead(A0);
  lcd.print(a);
 
  //Serial.print(millis());
  delay(1000);


    
  
}//end loop