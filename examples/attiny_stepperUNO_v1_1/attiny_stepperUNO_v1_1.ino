// Stepper Motor Control with I2C
// Version 1.0
// Target MCU: Attiny85
//
// Sketch for speed control with I2C
//
//Command Usage:
// a for direction 1 - e.g. Master will issue Wire.write("a")
// b for direction 2 - e.g. Master will issue Wire.write("b")
// delay: for speed control - e.g. Master will issue Wire.write("1500:")
// 
// Requires Arduino IDE with arduino-tiny core: http://code.google.com/p/arduino-tiny/


// the 7-bit address 
#define I2C_SLAVE_ADDRESS 0x5
#define ENABLE PB3
#define DIRECT PB1                      //Direction Pin
#define STEP__ PB4                           //Step Pin - Pulse this to step the motor in the direction selected by the Direction Pin


#include <TinyWireS.h>   // Get this from https://github.com/rambo/TinyWire

// Default buffer size.
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif


volatile uint8_t i2c_regs[] =
{
    0xDE, 
    0xAD, 
    0xBE, 
    0xEF, 
};

//Character variable used to echo data back. 
char chrSendData;

//Variables used in getting and parsing data.
char rxChrData; //Receives the data.
char rxString[12];  //Variable for holding one string of data.
int rxIndex = 0; //Used to index rxString.

unsigned int stepdelay = 0;
unsigned long timer;
//boolean steplevel = true;



//Handles receiving i2c data.
void receiveEvent(uint8_t howMany)
{
    if (TinyWireS.available()){  
      if (howMany < 1)
      {   // Sanity-check
          return;
      }
      if (howMany > TWI_RX_BUFFER_SIZE)
      {   // Also insane number
          return;
      }
  
      while(howMany--)
      {   //Gets i2c data. 
          rxChrData = TinyWireS.receive();
          
          if (rxChrData == ':'){ // Set speed
              //stepdelay = atoi(rxString);
              stepdelay = (unsigned int) atol(rxString);
              rxIndex = 0;  
              memset(rxString, 0, sizeof rxString);
         }
         else if (rxChrData == 'a'){  // Set direction 1
              PORTB &= ~(1 << DIRECT);//LOW
              rxIndex = 0;
         }
         else if (rxChrData == 'b'){ // Set direction 2
              PORTB |= (1 << DIRECT);//HIGH
              rxIndex = 0;
         }
         else if (rxChrData == 'e'){ // Set 
              PORTB &= ~(1 << ENABLE);//LOW
              rxIndex = 0;
         }
         else if (rxChrData == 'd'){ // Set 
              PORTB |= (1 << ENABLE);//HIGH
              rxIndex = 0;
         }
         else if (rxChrData == '.'){ // Set 
              PORTB ^= (1 << STEP__);
              rxIndex = 0;
         }
         else {
              rxString[rxIndex] = char(rxChrData);
              rxIndex++;
           
         }
      }
    }
}

void setup()
{
   
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    
    DDRB |= (1 << ENABLE); //set mode as output
    DDRB |= (1 << DIRECT);
    DDRB |= (1 << STEP__);
    
    PORTB |= (1<<ENABLE);// DISABLE MOTOR
   
}

void loop()
{
  TinyWireS_stop_check();
      
   if (stepdelay != 0)
   {
     if (micros()-timer > stepdelay){
       PORTB ^= (1 << STEP__);
       timer = micros();
       
      }
    }

}