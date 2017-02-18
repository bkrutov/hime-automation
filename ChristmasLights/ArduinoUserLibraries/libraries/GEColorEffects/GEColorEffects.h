/*
   GEColorEffects.h - Library for driving GE Color Effects
   LED Holiday Lights with an Arduino

   Created by Paul Martis, October 1, 2011.
   http://www.digitalmisery.com

   Based on Christmas Light Control by Robert Quattlebaum <darco@deepdarc.com>
   (http://www.deepdarc.com/2010/11/27/hacking-christmas-lights)
   and Arduino adaptation by Scott Harris <scottrharris@gmail.com>
   http://scottrharris.blogspot.com/2010/12/controlling-ge-color-effects-lights.html
*/

#ifndef GEColorEffects_h
#define GEColorEffects_h

#include "Arduino.h"

#define color_t uint16_t
#define CHANNEL_MAX          (0xF) //Each color channel is 4-bit 
#define DEFAULT_INTENSITY     (0xCC) //Brightness is 8-bit
#define HUE_MAX               ((CHANNEL_MAX+1)*6-1)  
#define COLOR(r,g,b)     ((r)+((g)<<4)+((b)<<8))  //Color is 12-bit (4-bit each R, G, B)
#define COLOR_WHITE     COLOR(CHANNEL_MAX,CHANNEL_MAX,CHANNEL_MAX)  
#define COLOR_BLACK     COLOR(0,0,0)  
#define COLOR_RED          COLOR(CHANNEL_MAX,0,0)  
#define COLOR_GREEN     COLOR(0,CHANNEL_MAX,0)  
#define COLOR_BLUE          COLOR(0,0,CHANNEL_MAX)  
#define COLOR_CYAN          COLOR(0,CHANNEL_MAX,CHANNEL_MAX)  
#define COLOR_MAGENTA     COLOR(CHANNEL_MAX,0,CHANNEL_MAX)  
#define COLOR_YELLOW     COLOR(CHANNEL_MAX,CHANNEL_MAX,0)
#define COLOR_PURPLE     COLOR(0xa,0x3,0xd)
#define COLOR_ORANGE    COLOR(0xf,0x1,0x0)
#define COLOR_WARMWHITE     COLOR(0xf,0x7,0x2)

class GEColorEffects
{
   public:
      GEColorEffects(int pin, int light_count);  //Arduino Pin # and total lights on string
      void set_color(uint8_t led, uint8_t intensity, color_t color); //Turn on a specific LED 
      color_t color(uint8_t r, uint8_t g, uint8_t b); //Color data type
      color_t color_hue(uint8_t h); 
      void fill_color(uint8_t begin, uint8_t count, uint8_t intensity, color_t color); 
      void fill_color_same(uint8_t begin, uint8_t count, uint8_t intensity, color_t color); //Make all LEDs the same color
	    int _pin;
	    int _light_count;
      void enumerate(); //Need to enumerate in order to address individual LEDs
   private:
      void begin(); //Low-level one-wire protocol commands
      void one();
      void zero();
      void end();
     // int _pin;
     // int _light_count;
};  
 
#endif