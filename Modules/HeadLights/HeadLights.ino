#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(7,6, NEO_GRBW );
int right;
int left;
int hazard;
int brake;

void setup() {
  pinMode(6, OUTPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(13, INPUT);
  strip.begin();
  strip.setPixelColor(6,0,0,0,100);
  strip.setPixelColor(2,0,0,0,100);
  strip.setPixelColor(3,100,0,0,0);
  strip.setPixelColor(5,100,0,0,0);
  strip.show();
}

void loop() {
  right = digitalRead(9);
  left = digitalRead(8);
  hazard=digitalRead(10);
  brake=digitalRead(11);
  delay(50);

  if(hazard){
    Shazard();
  }
  if(right){
     Tright(); 
     }
  if (left){
      Tleft();
      }
  if(brake){   
   Tbrake();
   }
  else{
   strip.setPixelColor(6,0,0,0,150);
   strip.setPixelColor(2,0,0,0,150);
   strip.setPixelColor(3,100,0,0,0);
   strip.setPixelColor(5,100,0,0,0);
   strip.show();
   }
   
  
}


void Tbrake(){
   strip.setPixelColor(3,255,0,0,0);
   strip.setPixelColor(5,255,0,0,0);
   strip.setPixelColor(2,0,0,0,100);
   strip.setPixelColor(6,0,0,0,100);
   strip.show();
   delay(50);
   strip.setPixelColor(3,100,0,0,0);
   strip.setPixelColor(5,100,0,0,0);
   strip.setPixelColor(2,0,0,0,100);
   strip.setPixelColor(6,0,0,0,100);
   strip.show();
}


void Tright(){
  if(brake){
    strip.setPixelColor(5,255,0,0,0);
    strip.setPixelColor(2,255,90,0,0);       
    strip.setPixelColor(3,255,90,0,0);
    strip.setPixelColor(6,0,0,0,100);
    strip.show();
    delay(500);
    strip.setPixelColor(5,255,0,0,0);
    strip.setPixelColor(2,0,0,0,0);       
    strip.setPixelColor(3,0,0,0,0);
    strip.setPixelColor(6,0,0,0,100); 
    strip.show();
    delay(500);
    }
  else{
    strip.setPixelColor(2,255,90,0,0);       
    strip.setPixelColor(3,255,90,0,0);
    strip.setPixelColor(6,0,0,0,100);
    strip.setPixelColor(5,100,0,0,0);
    strip.show();
    delay(500);
    strip.setPixelColor(2,0,0,0,0);
    strip.setPixelColor(3,0,0,0,0);
    strip.setPixelColor(6,0,0,0,100);
    strip.setPixelColor(5,100,0,0,0);
    strip.show();
    delay(500);
   }
}

void Tleft(){
    if(brake){
    strip.setPixelColor(5,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(6,255,90,0,0);
    strip.setPixelColor(2,0,0,0,100);
    strip.setPixelColor(3,255,0,0,0);
    strip.show();
    delay(500);
    strip.setPixelColor(5,0,0,0,0);
    strip.setPixelColor(6,0,0,0,0);
    strip.setPixelColor(2,0,0,0,100);
    strip.setPixelColor(3,100,0,0,0);
    strip.show();
    delay(500);
    }
    strip.setPixelColor(5,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(6,255,90,0,0);
    strip.setPixelColor(2,0,0,0,100);
    strip.setPixelColor(3,100,0,0,0);
    strip.show();
    delay(500);
    strip.setPixelColor(5,0,0,0,0);
    strip.setPixelColor(6,0,0,0,0);
    strip.setPixelColor(2,0,0,0,100);
    strip.setPixelColor(3,100,0,0,0);
    strip.show();
    delay(500);
}

void Shazard(){
    strip.setPixelColor(2,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(3,255,90,0,0);
    strip.setPixelColor(5,255,90,0,0);       // 255 90 00 00 for amber
    strip.setPixelColor(6,255,90,0,0);
    strip.show();
    delay(500);
    strip.setPixelColor(2,0,0,0,0);
    strip.setPixelColor(3,0,0,0,0);
    strip.setPixelColor(5,0,0,0,0);
    strip.setPixelColor(6,0,0,0,0);
    strip.show();
    delay(500);
  }
