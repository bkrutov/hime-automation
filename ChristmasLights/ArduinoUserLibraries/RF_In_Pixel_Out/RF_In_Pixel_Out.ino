/*
 * TODO
 *
 *    Input: nRF
 *    Output: Multiple Pixel Types (configurable below)
 *
 * Created on: May, 28th 2014
 * Author: Mat Mrosko <rfpixelcontrol@matmrosko.com>
 *
 * License:
 *    Users of this software agree to hold harmless the creators and
 *    contributors of this software.  By using this software you agree that
 *    you are doing so at your own risk, you could kill yourself or someone
 *    else by using this software and/or modifying the factory controller.
 *    By using this software you are assuming all legal responsibility for
 *    the use of the software and any hardware it is used on.
 *
 *    The Commercial Use of this Software is Prohibited.
 */

#include <EEPROM.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <FastLED.h>

#include "IRFShowControl.h"
#include "RFShowControl.h"

#include "MemoryFree.h"
#include "printf.h"

/********************* START OF REQUIRED CONFIGURATION ***********************/
// NRF_TYPE Description: http://learn.komby.com/wiki/58/configuration-settings#NRF_TYPE
// Valid Values: RF1, MINIMALIST_SHIEILD, WM_2999_NRF, RFCOLOR_2_4
#define NRF_TYPE                        RF1

// PIXEL_TYPE Description: http://learn.komby.com/wiki/58/configuration-settings#PIXEL_TYPE
// Valid Values: GECE, LPD_6803, LPD_8806, SM_16716, TM_1803, TM_1809, UCS_1903, WM_2999, WS_2801, WS_2811
#define PIXEL_TYPE                      WS_2811

// PIXEL_COLOR_ORDER Description: http://learn.komby.com/wiki/58/configuration-settings#PIXEL_COLOR_ORDER
// Valid Values: RGB, RBG, GRB, GBR, BRG, BGR
#define PIXEL_COLOR_ORDER               RGB

// OVER_THE_AIR_CONFIG_ENABLE Description: http://learn.komby.com/wiki/58/configuration-settings#OVER_THE_AIR_CONFIG_ENABLE
// Valid Values: OTA_ENABLED, OTA_DISABLED
#define OVER_THE_AIR_CONFIG_ENABLE      0

// RECEIVER_UNIQUE_ID Description: http://learn.komby.com/wiki/58/configuration-settings#RECEIVER_UNIQUE_ID
// Valid Values: 1-255
#define RECEIVER_UNIQUE_ID              33
/********************** END OF REQUIRED CONFIGURATION ************************/

/****************** START OF NON-OTA CONFIGURATION SECTION *******************/
// LISTEN_CHANNEL Description: http://learn.komby.com/wiki/58/configuration-settings#LISTEN_CHANNEL
// Valid Values: 0-83, 101-127  (Note: use of channels 84-100 is not allowed in the US)
#define LISTEN_CHANNEL                  25

// DATA_RATE Description:  http://learn.komby.com/wiki/58/configuration-settings#DATA_RATE
// Valid Values: RF24_250KBPS, RF24_1MBPS
#define DATA_RATE                       RF24_1MBPS//RF24_250KBPS

// HARDCODED_START_CHANNEL Description:  http://learn.komby.com/wiki/58/configuration-settings#HARDCODED_START_CHANNEL
// Valid Values: 1-512
#define HARDCODED_START_CHANNEL         1

// HARDCODED_NUM_PIXELS Description:  http://learn.komby.com/wiki/58/configuration-settings#HARDCODED_NUM_PIXELS
// Valid Values: 1-170
#define HARDCODED_NUM_PIXELS            170
/******************* END OF NON-OTA CONFIGURATION SECTION ********************/

/************** START OF ADVANCED SETTINGS SECTION (OPTIONAL) ****************/
// PIXEL_DATA_PIN Description:  http://learn.komby.com/wiki/58/configuration-settings#PIXEL_DATA_PIN
// Valid Values: Any arduino Analog or Digital pin, typically ~1-16
#define PIXEL_DATA_PIN                  2

// PIXEL_CLOCK_PIN Description:  http://learn.komby.com/wiki/58/configuration-settings#PIXEL_CLOCK_PIN
// Valid Values: Any arduino Analog or Digital pin, typically ~1-16
//#define PIXEL_CLOCK_PIN                 4

//How Bright should our LEDs start at Description:  http://learn.komby.com/wiki/58/configuration-settings#LED_BRIGHTNESS
#define LED_BRIGHTNESS                  255 //50%

#define DEBUG                           1

//FCC_RESTRICT Description: http://learn.komby.com/wiki/58/configuration-settings#FCC_RESTRICT
//Valid Values: 1, 0  (1 will prevent the use of channels that are not allowed in North America)
//#define FCC_RESTRICT 1
/********************* END OF ADVANCED SETTINGS SECTION **********************/


//Include this after all configuration variables are set
#include "RFShowControlConfig.h"

//Arduino setup function.
void setup(void)
{
#ifdef DEBUG
  Serial.begin(115200);
  printf_begin();
  Serial.println("Initializing Radio");
#endif
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  radio.EnableOverTheAirConfiguration(OVER_THE_AIR_CONFIG_ENABLE);
  uint8_t logicalControllerNumber = 0;
  if(!OVER_THE_AIR_CONFIG_ENABLE)
  {
    radio.AddLogicalController(logicalControllerNumber, HARDCODED_START_CHANNEL, HARDCODED_NUM_PIXELS * 3, 0);
  }

  radio.Initialize(radio.RECEIVER, pipes, LISTEN_CHANNEL, DATA_RATE, RECEIVER_UNIQUE_ID);

#ifdef DEBUG
  radio.printDetails();
  Serial.print(F("PixelColorOrder: "));
  printf("%d\n", PIXEL_COLOR_ORDER);
#endif

  logicalControllerNumber = 0;
  int countOfPixels = radio.GetNumberOfChannels(0)/3;

#ifdef DEBUG
  Serial.print(F("Number of channels configured "));
  printf("%d\n", countOfPixels);
#endif

Serial.println("FAST SPI Init path");
leds = (CRGB*) radio.GetControllerDataBase(logicalControllerNumber++);
memset(leds, 0, countOfPixels * sizeof(struct CRGB));
//Initalize the data for LEDs
//todo eventually this will be a bug
delay(200);
LEDS.setBrightness(LED_BRIGHTNESS);

LEDS.addLeds(new WS2811Controller800Khz<PIXEL_DATA_PIN, PIXEL_COLOR_ORDER>(), leds, countOfPixels, 0);

#ifdef DEBUG
  radio.PrintControllerConfig();
  Serial.print(F("freeMemory()="));
  Serial.println(freeMemory());
#endif
  digitalWrite(4, LOW);
}

void loop(void)
{
  digitalWrite(4, LOW);
  //When Radio.Listen returns true its time to update the LEDs for all controllers, a full update was made
  if (radio.Listen())
  {
      digitalWrite(4, HIGH);
      LEDS.show();
  }
}
