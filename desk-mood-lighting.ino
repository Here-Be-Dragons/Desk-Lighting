// This #include statement was automatically added by the Particle IDE.
#include "Sunrise/Sunrise.h"

// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define LEFT_LED 31
#define BACK_LED 100
#define RIGHT_LED 31

#define MOTION_PIN A3

#define PIXEL_PIN D6
#define PIXEL_COUNT 162
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

uint32_t cOffset = 0;
bool vMotion = 0;
bool vMotionPrevious = 0;
uint32_t lastMotion = 0;
uint8_t sunriseHour = 0;
uint8_t sunsetHour = 0;
uint8_t lastHour = 24;  // hour() returns zero through twenty-three
uint32_t debug1 = 0;
uint32_t debug2 = 0;
uint32_t debug3 = 0;
uint8_t brightness = 255;


void setup()
{
  Particle.subscribe("hook-response/check_sunrise", gotSunriseData, MY_DEVICES);
  strip.begin();
  for (int i=0; i <= strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
  }
  strip.show(); // Initialize all pixels to 'off'
  Particle.variable("Debug1", debug1);
  Particle.variable("Debug2", debug2);
  Particle.variable("Debug3", debug3);
  pinMode(MOTION_PIN,   INPUT);
  Particle.publish("check_sunrise"); //Get sunrise sunset times
  delay(1000);
}




void loop() {
    onceADay(); //Refresh
    vMotion = checkMotion();
    setBrightness();
    if (vMotion != vMotionPrevious) {
        fade(vMotion);
        vMotionPrevious = vMotion;
    }
    if (vMotion) {
        //side desk strips
        for (int i=0; i <= LEFT_LED; i++) {
            int c = abs(((i + cOffset) % 90) - 45) + 191;
            strip.setPixelColor(i, Wheel(c));
            strip.setPixelColor(i + 132, Wheel(c));
        }
        //back desk strip
        for (int i = LEFT_LED + 1; i <= ( LEFT_LED + BACK_LED ); i++) {
            int c = abs(((i + cOffset) % 70) - 35) + 75;
            strip.setPixelColor(i, Wheel(c));
        }
        cOffset++;
    //} else {
    } else {
        for (int i=0; i <= strip.numPixels(); i++) {
            strip.setPixelColor(i, (brightness*0/255), (brightness*0/255), (brightness*50/255));
        }
    }
    strip.show();
    delay(200);
    debug1 = strip.getPixelColor(5);
}

void onceADay() {
    uint8_t currentHour = Time.hour();
    if (lastHour != currentHour && currentHour == 1) {
        Particle.publish("check_sunrise"); //This runs every day at 1:00 AM
        lastHour = currentHour;
    }
}

void gotSunriseData(const char *name, const char *data) {
    
    String str = String(data);
    char strBuffer[400] = "";
    str.toCharArray(strBuffer, 400);
    //lastGotSunrise = Time.now();
    
    sunriseHour         = atoi(strtok(strBuffer, "~"));
    sunsetHour          = atoi(strtok(NULL, "~"));
    //debug1 = sunriseHour;
    //debug2 = sunsetHour;
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color((brightness*(WheelPos * 3)/255), (brightness*(255 - WheelPos * 3)/255), (brightness*(0)/255));
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color((brightness*(255 - WheelPos * 3)/255), (brightness*(0)/255), (brightness*(WheelPos * 3)/255));
  } else {
   WheelPos -= 170;
   return strip.Color((brightness*(0)/255), (brightness*(WheelPos * 3)/255), (brightness*(255 - WheelPos * 3)/255));
  }
}

bool checkMotion() {                           //Check for motion
  uint16_t motion = analogRead(MOTION_PIN);    // Analog Read
  debug2 = motion;
  static bool intMotion = 1;
  if ( motion > 0 ) {
      intMotion = 1;
      lastMotion = Time.now();
  } 
  if ( motion < 5 && Time.now() - lastMotion >= 1 ) { //600
      intMotion = 0;
  }
  return intMotion;
}

void setBrightness() {
    bool i;
    if ((Time.hour() < sunsetHour || Time.hour() > sunriseHour) && vMotion) {
        i = 1;
    } else {
        i = 0;
    }
    if ( i && brightness != 255) {
        brightness++;
    } else if ( !i && brightness != 100) {
        brightness--;
    }
    
}
//             int c = abs((j % 90) - 45) + 191;
//            strip.setPixelColor(i, Wheel(c));
//c = abs(((i + cOffset) % 70) - 35) + 75;
//

void fade(bool a) {
    int16_t steps = 20;
    
    if (a) {
        for (int16_t i = 0 ; i < steps - 1 ; ++i) {
                
                //side desk strips
                for (int j=0; j <= LEFT_LED; j++) {
                    int final = Wheel(abs((j % 90) - 45) + 191);
                    int diff = final - strip.getPixelColor(j);
    
                    int c = strip.getPixelColor(j) + (diff * (i / steps));
                    strip.setPixelColor(j, c);
                    strip.setPixelColor(j + 132, c);
                }
                //back desk strip
                for (int j = LEFT_LED + 1; j <= ( LEFT_LED + BACK_LED ); j++) {
                    int final = Wheel(abs((j % 70) - 35) + 75);
                    int diff = final - strip.getPixelColor(j);
    
                    int c = strip.getPixelColor(j) + (diff * (i / steps));
                    strip.setPixelColor(j, c);
                }
            strip.show();
            delay(200);
        }
    } else {
        for (int16_t i = 0 ; i < steps - 1 ; ++i) {
                
                //side desk strips
                for (int j=0; j <= strip.numPixels(); j++) {
                    int final = Wheel(abs((j % 90) - 45) + 191);
                    int diff = final - strip.getPixelColor(j);
    
                    int c = strip.getPixelColor(j) + (diff * (i / steps));
                    strip.setPixelColor(j, c);
                    strip.setPixelColor(j + 132, c);
                }
            strip.show();
            delay(200);
        }

    }
}

/*void loop() {
    color %= 255;
    runner %= 81;
    for (int k = 0; k < 20; k++){

        //left desk strip
        for (int i=0; i <= LEFT_LED; i++) {
            //strip.setPixelColor(i, 25, 175, 175); //teal full-brightness
            strip.setPixelColor(i, 10, 70, 70);
        }
        //back desk strip
        for (int i = LEFT_LED + 1; i <= ( LEFT_LED + BACK_LED ); i++) {
            strip.setPixelColor(i, 100, 0, 0); //Red
        }
        //back desk strip
        for (int i = ( LEFT_LED + BACK_LED ) + 1; i <= ( LEFT_LED + BACK_LED + RIGHT_LED ); i++) {
            strip.setPixelColor(i, 10, 70, 70);
        }
        for (int j = LEFT_LED + 1; j <= (LEFT_LED + BACK_LED ); j += 20 ) {
        strip.setPixelColor(j+k,200,200,200); //White
        
        }
        strip.show();
        delay(70);
    }

    
    color++;
    runner++;
    
}*/
