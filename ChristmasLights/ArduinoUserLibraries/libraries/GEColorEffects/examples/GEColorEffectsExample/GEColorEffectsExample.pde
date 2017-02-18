#include <GEColorEffects.h>

#define outPin 19  //Arduino pin #
#define lightCount 50  //Total # of lights on string (usually 50, 48, or 36)

static uint16_t c;

GEColorEffects lights(outPin, lightCount);  //Constructor

void setup() {
  lights.enumerate();  //Enumerate lights on string to enable individual bulb addressing
}

void loop() {
  //fill_color makes all bulbs from 0 to lightCount the same brightness and color
  lights.fill_color(0, lightCount, DEFAULT_INTENSITY, COLOR_RED);
  delay(1000);
  lights.fill_color(0, lightCount, DEFAULT_INTENSITY, COLOR_GREEN);
  delay(1000);
  //set_color addresses one specific bulb at a time with brightness and color setting
  for(int i=0;i<lightCount/2;i++) {
    color_t color = lights.color_hue((i+c)%(HUE_MAX+1));
    lights.set_color(i, DEFAULT_INTENSITY, color);
    lights.set_color(lightCount-1-i, DEFAULT_INTENSITY, color);
  }
  c++;
  delay(1000);
}
