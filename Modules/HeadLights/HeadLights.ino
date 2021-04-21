#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel right(20,6, NEO_GRBW ); // 20 for how many LED's
Adafruit_NeoPixel left(20, 7, NEO_GRB);   // second number is pinout

const int brakeInput = 9;
const int reverseInput = 11;
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
}

void loop() {
  Test(left);
}

void Test(Adafruit_NeoPixel &strip){
  // 0-6  = Reverse
  // 7-13 = Brake
  // 14-20 = Turn Signal

// The first round of for loops turns off the sections if they are activated

  bool Reverse = digitalRead(reverseInput);
  bool Brake   = digitalRead(brakeInput);
  bool Hazard  = digitalRead(hazardInput);

  for(int i=0; i<21; i++){
    if(Hazard){   // if hazards, make whole strip flashing yellow
      strip.setPixelColor(i, 255,90,0);
      continue;
    }
    
    if(i<7 && Reverse){
      strip.setPixelColor(i,255,255,255);
    } else if (i<7) {
      strip.setPixelColor(i,0,0,0);
    }

    if(i>=7 && i<14 && Brake){
      strip.setPixelColor(i, 255,0,0);
    } else if(i>=7 && i<14){
      strip.setPixelColor(i, 0,0,0);
    }

    if(i>=14 && Hazard){
      strip.setPixelColor(i, 255,90,0);
    } else if(i>=14){
      strip.setPixelColor(i, 0,0,0); 
    }

    if(!Reverse && !Brake){
      strip.setPixelColor(i, 255,0,0);
      strip.setBrightness(10);
    }
  }
  if(Reverse || Brake || Hazard) 
    strip.setBrightness(255);
  
  strip.show(); // show the changes
  delay(200);   // delay 

  for(int i=0; i<21; i++){
    if(Hazard){
      strip.setPixelColor(i, 0,0,0);
      continue;
    }
    
    if(i<7 && Reverse){
      strip.setPixelColor(i, 255,255,255);
    } else if(i<7) {
      strip.setPixelColor(i, 0,0,0);
    }

    if(i>=7 && i<14 && Brake){
      strip.setPixelColor(i, 255,0,0);
    } else if(i>=7 && i<14) {
      strip.setPixelColor(i, 0,0,0);
    }

    if(i>=14 && Hazard){
      strip.setPixelColor(i, 255,90,0);
    } else if(i>=14) {
      strip.setPixelColor(i, 0,0,0);
    }

    if(!Reverse && !Brake){
      strip.setPixelColor(i, 255,0,0);
      strip.setBrightness(10);
    }
  }

  if(Reverse || Brake || Hazard) 
    strip.setBrightness(255);
  strip.show();
  delay(200);  
}


void Brake(Adafruit_NeoPixel &left, Adafruit_NeoPixel &right){
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

void Hazard(Adafruit_NeoPixel &left, Adafruit_NeoPixel &right){
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

void LeftTurn(Adafruit_NeoPixel &strip){
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

void RightTurn(Adafruit_NeoPixel &strip){
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
