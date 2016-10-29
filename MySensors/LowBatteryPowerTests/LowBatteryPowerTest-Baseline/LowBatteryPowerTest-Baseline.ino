
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "nRF24L01.h"
#include "RF24.h"

void before()
{
  Serial.begin(115200);

  delayMicroseconds(1000000);
  Serial.println("before");  
}

void setup()  
{
  Serial.begin(115200);

  delayMicroseconds(1000000);
  Serial.println("\n[TEST Low Power]\n");  

  delayMicroseconds(100000);

  Serial.println("Setup - pin 6 on");
  delayMicroseconds(100000);

  pinMode (6, OUTPUT); 
  digitalWrite (6, HIGH);
  
}

void loop()     
{    
   pinMode (6, OUTPUT); 
   digitalWrite (6, HIGH);
  delayMicroseconds(100000);

   Serial.println("Loop - pin 6 on");
  delayMicroseconds(100000);

  //explicit sleeping code below
  //Serial.println("Loop - pin 6 off");

  //wait(20);
  //digitalWrite (6, LOW);
  Serial.println("Going to sleep");
  delayMicroseconds(1000000);

  // disable ADC
  ADCSRA = 0;  

  // clear various "reset" flags
  MCUSR = 0;     
  // allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE);
  // set interrupt mode and an interval 
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  wdt_reset();  // pat the dog
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  sleep_cpu();  

  // the code below this line seem do not execute, instead setup is fired again
  
  // cancel sleep as a precaution
  sleep_disable();
  

 // pinMode (6, OUTPUT); 
 // digitalWrite (6, HIGH);
  
//  Serial.println("Loop - pin 6 on");
 
}
