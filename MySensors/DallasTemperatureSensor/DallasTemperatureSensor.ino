/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * Example sketch showing how to send in DS1820B OneWire temperature readings back to the controller
 * http://www.mysensors.org/build/temp
 */


// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <MySensors.h>  
#include <DallasTemperature.h>
#include <OneWire.h>

int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point

int oldBatteryPcnt = 0;

#define COMPARE_TEMP 1 // Send temperature only if changed? 1 = Yes 0 = No

#define ONE_WIRE_BUS 3 // Pin where dallase sensor is connected 
#define MAX_ATTACHED_DS18B20 16
unsigned long SLEEP_TIME = 300000; // Sleep time between reads (in milliseconds)
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature. 
float lastTemperature[MAX_ATTACHED_DS18B20];
int numSensors=0;
bool receivedConfig = false;
bool metric = true;
// Initialize temperature message
MyMessage msg(0,V_TEMP);

void before()
{  
  
  Serial.println("Before - pin 6 on");
  pinMode (6, OUTPUT); 
  digitalWrite (6, HIGH);  

  // Startup up the OneWire library
  sensors.begin();
}

void setup()  
{  
  Serial.println("Setup - pin 6 on");
  pinMode (6, OUTPUT); 
  digitalWrite (6, HIGH);
  
  // use the 1.1 V internal reference
  #if defined(__AVR_ATmega2560__)
     analogReference(INTERNAL1V1);
  #else
     analogReference(INTERNAL);
  #endif

  
  // requestTemperatures() will not block current thread
  sensors.setWaitForConversion(false);
}


void presentation() {
  Serial.println("Presentation - pin 6 on");

  pinMode (6, OUTPUT); 
  digitalWrite (6, HIGH);
  
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Temperature Sensor", "1.1");

  // Fetch the number of attached temperature sensors  
  numSensors = sensors.getDeviceCount();

  // Present all sensors to controller
  for (int i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++) {   
     present(i, S_TEMP);
  }  
}

void loop()     
{    
   pinMode (6, OUTPUT); 
   digitalWrite (6, HIGH);

   Serial.println("Loop - pin 6 on");
   // get the battery Voltage
   int sensorValue = analogRead(BATTERY_SENSE_PIN);
   #ifdef MY_DEBUG
   Serial.println(sensorValue);
   #endif
   
   // 1M, 470K divider across battery and using internal ADC ref of 1.1V
   // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
   // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
   // 3.44/1023 = Volts per bit = 0.003363075
   
   int batteryPcnt = sensorValue / 10;

   #ifdef MY_DEBUG
   float batteryV  = sensorValue * 0.003363075;
   Serial.print("Battery Voltage: ");
   Serial.print(batteryV);
   Serial.println(" V");

   Serial.print("Battery percent: ");
   Serial.print(batteryPcnt);
   Serial.println(" %");
   #endif

   if (oldBatteryPcnt != batteryPcnt) {
     // Power up radio after sleep
     sendBatteryLevel(batteryPcnt);
     oldBatteryPcnt = batteryPcnt;
   }


  // Fetch temperatures from Dallas sensors
  sensors.requestTemperatures();

  // query conversion time and sleep until conversion completed
  int16_t conversionTime = 750 / (1 << (12 - sensors.getResolution()));
  // sleep() call can be replaced by wait() call if node need to process incoming messages (or if node is repeater)
  wait(conversionTime);

  // Read temperatures and send them to controller 
  for (int i=0; i<numSensors && i<MAX_ATTACHED_DS18B20; i++) {
 
    // Fetch and round temperature to one decimal
    float temperature = static_cast<float>(static_cast<int>((getConfig().isMetric?sensors.getTempCByIndex(i):sensors.getTempFByIndex(i)) * 10.)) / 10.;
 
    // Only send data if temperature has changed and no error
    #if COMPARE_TEMP == 1
    if (lastTemperature[i] != temperature && temperature != -127.00 && temperature != 85.00) {
    #else
    if (temperature != -127.00 && temperature != 85.00) {
    #endif
 
      // Send in the new temperature
      send(msg.setSensor(i).set(temperature,1));
      // Save new temperatures for next compare
      lastTemperature[i]=temperature;
    }
  }

  sleep(SLEEP_TIME);
  /*
  //explicit sleeping code below
  Serial.println("Loop - pin 6 off");

  //wait(20);
  digitalWrite (6, LOW);
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
  sleep_cpu ();  
  
  // cancel sleep as a precaution
  sleep_disable();

  pinMode (6, OUTPUT); 
  digitalWrite (6, HIGH);
  
  Serial.println("Loop - pin 6 on");
  */
}
