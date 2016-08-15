/*
   GEColorEffects.cpp - Library for driving GE Color Effects
   LED Holiday Lights with an Arduino

   Created by Paul Martis, October 1, 2011.
   http://www.digitalmisery.com

   Based on Christmas Light Control by Robert Quattlebaum <darco@deepdarc.com>
   (http://www.deepdarc.com/2010/11/27/hacking-christmas-lights)
   and Arduino adaptation by Scott Harris <scottrharris@gmail.com>
   http://scottrharris.blogspot.com/2010/12/controlling-ge-color-effects-lights.html
*/

#include "Arduino.h"
#include "GEColorEffects.h"

GEColorEffects::GEColorEffects(int pin, int light_count)
{
   pinMode(pin, OUTPUT);
   _pin = pin;
   _light_count = light_count;
}

//The timings below have been hand-tuned. Adjust if necessary.
void GEColorEffects::begin()
{
   digitalWrite(_pin, HIGH);  
   delayMicroseconds(6);  //Output should be ~ 10uS long 
   digitalWrite(_pin, LOW); 
}

void GEColorEffects::one()
{
   digitalWrite(_pin, LOW);  
   delayMicroseconds(12); //Output should be ~ 20uS long 
   digitalWrite(_pin, HIGH);  
   delayMicroseconds(6);  //Output should be ~ 10uS long 
   digitalWrite(_pin, LOW);
}

void GEColorEffects::zero()
{
   digitalWrite(_pin, LOW);  
   delayMicroseconds(2);  //Output should be ~ 10uS long
   digitalWrite(_pin, HIGH);  
   delayMicroseconds(17); //Output should be ~ 20uS long  
   digitalWrite(_pin, LOW);
}

void GEColorEffects::end()
{
   digitalWrite(_pin, LOW);  
   delayMicroseconds(30); //Quiet time should be ~ 30us long
}

//Turn on a specific LED with a color and brightness
void GEColorEffects::set_color(uint8_t led, uint8_t intensity, color_t color)
{  
   uint8_t i;  
   begin();  
   for(i=6;i;i--,(led<<=1))  
      if(led&(1<<5))  
         one();  
      else  
         zero();
  
   for(i=8;i;i--,(intensity<<=1))  
      if(intensity&(1<<7))  
         one();  
      else  
         zero();
  
   for(i=12;i;i--,(color<<=1))  
      if(color&(1<<11))  
         one();  
      else  
         zero();  
   end();  
}

//Returns 12-bit color from red, green, and blue components
color_t GEColorEffects::color(uint8_t r, uint8_t g, uint8_t b)
{  
   return COLOR(r,g,b);  
}

//Returns primary hue colors
color_t GEColorEffects::color_hue(uint8_t h)
{  
   switch(h>>4)
   {  
      case 0:     h-=0; return color(h,CHANNEL_MAX,0);  
      case 1:     h-=16; return color(CHANNEL_MAX,(CHANNEL_MAX-h),0);  
      case 2:     h-=32; return color(CHANNEL_MAX,0,h);  
      case 3:     h-=48; return color((CHANNEL_MAX-h),0,CHANNEL_MAX);  
      case 4:     h-=64; return color(0,h,CHANNEL_MAX);  
      case 5:     h-=80; return color(0,CHANNEL_MAX,(CHANNEL_MAX-h));  
   }   
}

//Make all LEDs the same color starting at specified beginning LED
void GEColorEffects::fill_color(uint8_t begin, uint8_t count, uint8_t intensity, color_t color)
{
   while(count--)  
   {  
      set_color(begin++, intensity, color);  
   }
}

//Make all LEDs the same color
void GEColorEffects::fill_color_same(uint8_t begin, uint8_t count, uint8_t intensity, color_t color)
{
   while(count--)  
   {  
      set_color(0, intensity, color);
   }
}

//Initialize string for individual bulb addressing
void GEColorEffects::enumerate()
{
   uint8_t i;
   while(i < _light_count)
   {
      fill_color(0, _light_count, DEFAULT_INTENSITY, COLOR_BLACK);  
      i++;
   }
}