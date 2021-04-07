#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(7,6, NEO_GRBW );

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
strip.begin();

strip.setPixelColor(6,0,0,0,50);
strip.setPixelColor(2,0,0,0,50);
strip.setPixelColor(3,100,0,0,0);
strip.setPixelColor(5,100,0,0,0);
strip.show();


}

void loop() {
  // put your main code here, to run repeatedly:

  while(!Serial.available());
  int dir=Serial.read()-'0';
  Serial.print(dir);
  switch(dir){
    case 0:               
        right();
        break;
    case 1:
         left();
        break;
    case 2:
      strip.setPixelColor(3,255,0,0,0);
      strip.setPixelColor(5,255,0,0,0);
      strip.show();
      break;
    case 3:
      
      hazard();
      break;
     default:
         strip.setPixelColor(6,0,0,0,150);
         strip.setPixelColor(2,0,0,0,150);
         strip.setPixelColor(3,100,0,0,0);
         strip.setPixelColor(5,100,0,0,0);
         strip.show();
      break;
     
        
    
    
    
    
    }
  
}



void right(){
    strip.setPixelColor(2,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(3,255,90,0,0);
    strip.show();
    delay(750);
    strip.setPixelColor(2,0,0,0,0);
    strip.setPixelColor(3,0,0,0,0);
    strip.show();
    delay(750);
  }

void left(){
    strip.setPixelColor(5,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(6,255,90,0,0);
    strip.show();
    delay(750);
    strip.setPixelColor(5,0,0,0,0);
    strip.setPixelColor(6,0,0,0,0);
    strip.show();
    delay(750);
  }

void hazard(){
    strip.setPixelColor(2,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(3,255,90,0,0);
    strip.setPixelColor(5,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(6,255,90,0,0);
    strip.show();
    delay(750);
    strip.setPixelColor(2,0,0,0,0);
    strip.setPixelColor(3,0,0,0,0);
    strip.setPixelColor(5,0,0,0,0);
    strip.setPixelColor(6,0,0,0,0);
    strip.show();
    delay(750);
  
  
  
  }
