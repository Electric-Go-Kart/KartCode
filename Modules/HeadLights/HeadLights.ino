#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel right(20,6, NEO_GRBW ); // 20 for how many LED's
Adafruit_NeoPixel left(20, 7, NEO_GRB);   // second number is pinout

const int brakeInput = 2;
const int leftInput = 3;
const int rightInput = 4;
const int hazardInput = 5;

void setup() {
  pinMode(7,OUTPUT);

  pinMode(2, INPUT);    // input from brake line
  pinMode(3, INPUT);    // input from left signal
  pinMode(4, INPUT);    // input from right signal
  pinMode(5, INPUT);
  
  left.begin();
  left.setBrightness(25);

  right.begin();
  right.setBrightness(25);

  while(!Serial){;}
  
}

void loop() {
  while(true){
    Test(left);
 
    Serial.println("Back in Loop");
  }
}

void Test(Adafruit_NeoPixel strip){
  // 0-6  = Reverse
  // 7-13 = Brake
  // 14-20 = Turn Signal

// The first round of for loops turns off the sections if they are activated
  Serial.println("1");
  for(int i=0; i<7; i++){
    if(true/*Reverse*/){
      strip.setPixelColor(i, 0,0,0);
    } else {
      strip.setPixelColor(i, 0,0,0);
    }
  }
  Serial.println("2");
  for(int i=7; i<14; i++){
    if(true/*Brake*/){
      strip.setPixelColor(i, 255,0,0);
    } else {
      strip.setPixelColor(i, 0,0,0);
    }
  }
  Serial.println("3");
  for(int i=14; i<21; i++){
    if(true/*Left or Right*/){
      strip.setPixelColor(i, 0,0,0);
    } else {
      strip.setPixelColor(i, 0,0,0);
    }
  }
  
  strip.show(); // show the changes
  delay(250);   // delay 
  Serial.println("4");
  for(int i=0; i<7; i++){
    if(true/*Reverse*/){
      strip.setPixelColor(i, 255,255,255);//reversing
    } else {
      strip.setPixelColor(i, 255,0,0);    // not reversing
    }
  }
  Serial.println("5");
  for(int i=7; i<14; i++){
    if(true/*Brake*/){  
      strip.setPixelColor(i, 255,0,0);  // braking
    } else {  
      strip.setPixelColor(i, 0,0,0);  // not braking
    }
  }
  Serial.println("6");
  for(int i=14; i<21; i++){
    if(true/*Left or Right*/){
      strip.setPixelColor(i, 255,0,0);  // turning
    } else {
      strip.setPixelColor(i, 0,0,0);  // not turning
    }
  }
  Serial.println("7");
  strip.show();
  Serial.println("8");
  delay(250);
  Serial.println("9");
}


void Brake(Adafruit_NeoPixel left, Adafruit_NeoPixel right){
  int numP = left.numPixels();

  for(int i=0; i<numP; i++){
    left.setPixelColor(i, 0,0,0);
    right.setPixelColor(i,0,0,0);
  }

  delay(250);

  for(int i=0; i<numP; i++){
    left.setPixelColor(i, 255,0,0);
    right.setPixelColor(i,255,0,0);
  }

  delay(250);
}

void Hazard(Adafruit_NeoPixel left, Adafruit_NeoPixel right){
    int numL = left.numPixels();

    for(int i=numL/2; i<numL; i++){
      left.setPixelColor(i,255,90,0);  // turn top half yellow
      right.setPixelColor(i,255,90,0);
    }
    left.show();
    right.show();

    delay(250);

    for(int i=numL/2; i<numL; i++){
      left.setPixelColor(i, 255,0,0);
      right.setPixelColor(i, 255,0,0);
    }

    left.show();
    right.show();

    delay(250);
  }

void LeftTurn(Adafruit_NeoPixel strip){
  int numP = strip.numPixels();
  
  for(int i=0; i<numP; i++){
    strip.setPixelColor(i, 255,90,0);
    strip.show();
    delay(33);
  }

  for(int i=0; i<numP; i++){
    strip.setPixelColor(i,0,0,0);
  }
  strip.show();
}

void RightTurn(Adafruit_NeoPixel strip){
  int numP = strip.numPixels();
  
  for(int i=numP; i>0; i--){
    strip.setPixelColor(i, 255,90,0);
    strip.show();
    delay(33);
  }

  for(int i=0; i<numP; i++){
    strip.setPixelColor(i,0,0,0);
  }
  strip.show();
}
