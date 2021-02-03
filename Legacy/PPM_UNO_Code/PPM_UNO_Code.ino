/*  Code For E-GoKart Senior Design Project for Colorado State University 
 *   Written by: Deagan Malloy
 *   This will be the code to use PPM from the remote control reciever.
 *    This way there will only be 1 input data pin from the reciever, instead of 4-6 of them
 * 
 */
#include <LiquidCrystal_I2C.h>
#include<Servo.h>

Servo steeringServo;

//LiquidCrystal_I2C lcd1(0x27, 16, 2, 4, 5, );
//LiquidCrystal_I2C lcd2(0x26, 16, 2);
//LiquidCrystal_I2C lcd3(0x25, 16, 2);
//LiquidCrystal_I2C lcd4(0x24, 16, 2);

const int DATA_PIN   = 2;   // PPM input pin from Reciever (channel 1 on receiver, PIN2 on arduino UNO)
const int Headlights = 4;   // Digital pin for the headlights
const int enableDriver = 7; // Digital enable for the driver chip on the driver for the motors
const int servoPIN   = 9;   // Pin for the servo motor for steering
const int PWMout1    = 10;  // PWM output 1 to HBridge  - PIN10
const int PWMout2    = 11;  // PWM output 2 to HBridge  - PIN11

volatile int data_length[9] = {9400, 1500, 1500, 1000, 1500, 1000, 1000, 1500, 1500};   // All volatile variables are used in ISR
/*  [0] = long pulse filler pulse to fill to ~20ms total
 *  [1] = start bit ~1.5ms
 *  [2] = channel 1 pulse ~1.5ms (not used)(up and down on right stick)
 *  [3] = Throttle Pulse  -- between 1ms and 2ms
 *  [4] = Steering Pulse  -- between 1ms and 2ms
 *  [5] = Headlight Pulse -- either 1ms or 2ms
 *  [6] = Drive State     -- either 1ms(neutral) 1.5ms(drive) or 2ms(reverse)
 *  [7] = channel 6 pulse ~1.5ms (not used)(side to side on left stick)
 *  [8] = stop bit ~1.5ms
 */ 
volatile long startTime = 0;
volatile bool dataStart = false;
volatile bool canStart = false;
volatile bool stopIt = false;
volatile int pos = 1;
volatile int pulseLen;

int servoPosNew = 90;
int maxPWM = 127;

void setup() {
  pinMode(DATA_PIN, INPUT);

  pinMode(PWMout1, OUTPUT);
  pinMode(PWMout2, OUTPUT);
  pinMode(Headlights, OUTPUT);
  pinMode(enableDriver, OUTPUT);

  steeringServo.attach(servoPIN);
/*
  lcd1.backlight();
  lcd2.noBacklight();

  lcd2.clear();
  lcd2.home();
  lcd2.print("hello world");
*/
  attachInterrupt(digitalPinToInterrupt(DATA_PIN), read_PPM, RISING);
  Serial.begin(9600);
  delay(200); 

}

void loop() {
  if(stopIt){
      detachInterrupt(digitalPinToInterrupt(DATA_PIN));
    for(auto val : data_length){
      Serial.print(val);
      Serial.print(", ");
    }
    Serial.println();
    Serial.print("pos: ");
    Serial.println(pos);
    Serial.print("pulseLen: ");
    Serial.println(pulseLen);
    while(true){ // infinite loop to stop the program and to stop the car until reset
    }
  }
  
  if(!canStart){
    Serial.println("Turn on Remote Control"); // should change to display on a screen (as with all other print statements);
    delay(2000);
    return;
  }

  static int drive_state = 0;   // 0=neutral, 1=drive, 2=reverse
  updateThrottleOut(data_length[3], drive_state);
  drive_state = updateDriveState(data_length[6]);
  updateSteering(data_length[4]);
  updateHeadlights(data_length[5]);
  
/*
  Serial.println("Start of the inputs:");
  Serial.print("Off time:  ");
  Serial.println(data_length[0]);
  Serial.print("Start bit: ");
  Serial.println(data_length[1]);
  Serial.print("Ch1:       ");
  Serial.println(data_length[2]);
  Serial.print("Throttle:  ");
  Serial.println(data_length[3]);
  Serial.print("Steering:  ");
  Serial.println(data_length[4]);
  Serial.print("Headlights:");
  Serial.println(data_length[5]);
  Serial.print("Drive:     ");
  Serial.println(data_length[6]);
  Serial.print("Ch6:       ");
  Serial.println(data_length[7]);
  Serial.print("Stop bit:  ");
  Serial.println(data_length[8]);
  Serial.println();
  delay(1000);
*/
}

void read_PPM(){
  pulseLen = (int)(micros() - startTime);
  if(canStart){
    if(pos == 1){         // this is the start bit - check to make sure it is the right length
      if((pulseLen) > 1350 && (pulseLen) < 1650){
        dataStart = true;
      }
      
    } else if(pos == 8){  // this is the stop bit - check to make sure it is the right length
      if(pulseLen > 1350 && pulseLen < 1650){
        dataStart = false;
        //data_length[pos] = pulseLen; do not need this to store stop bit length
      }
    }

    if(dataStart){  // store the data in our array
      data_length[pos] = pulseLen;
    }

    if(pos == 8)  // increment our position counter
      pos = 0;
    else 
      pos++;

    startTime = micros();

  } else {    // we have synced the data and can start reading the data
    if(pulseLen > 9200 && pulseLen < 9600){
      canStart = true;
    } 
    startTime = micros();
  }
}

void updateThrottleOut(int pulse, int &drive_state){
  static int setRpm1 = 0, setRpm2 = 0;
  
  if(drive_state == 0){ // We are in Neutral - Ouput L L to start breaking and bring car to stop
    setRpm1 = 0;
    setRpm2 = 0;
     
  } else if(drive_state == 1){                // We are in Drive - Output H L to drive forward
    if(pulse > 1100)
      setRpm1 = map(pulse, 1100, 2000, 0, maxPWM); // map input pulse to PWM output duty cycle between 0 and 255
    else 
      setRpm1 = 0;                              // if input pulse is too slow, stay still
    setRpm2 = 0;                                // Second needs to be low to drive Forwards

  } else if(drive_state == 2){                // We are in Reverse - Output L H to drive backwards (just opposite of drive)
   if(pulse > 1100)
      setRpm2 = map(pulse, 1100, 2000, 0, maxPWM); // map input pulse to PWM output duty cycle between 0 and 255
    else 
      setRpm2 = 0;                              // if input pulse is too slow, stay still
    setRpm1 = 0;                                // First needs to be low to Reverse
    
  } else if(pulse > 2100 || pulse < 900){     // Error checking to see if we have bad information
    setRpm1 = 0;
    setRpm2 = 0; 
    analogWrite(PWMout1, setRpm1);
    analogWrite(PWMout2, setRpm2);
    stopProgram("Invalid pulse length for Acceleration"); // change this to setDefault() then it will be able to reconnect  
  }
  analogWrite(PWMout1, setRpm1);
  analogWrite(PWMout2, setRpm2);
}

void updateHeadlights(int pulse){ // just reads in if switch is flipped or not, and turns on headlights
    if(pulse > 1500 && pulse < 2100){       // turn on headlights
      digitalWrite(Headlights, HIGH);
    } else if(pulse > 900 && pulse < 1500){ // turn off headlights
      digitalWrite(Headlights, LOW);
    } else {
      stopProgram("Invalid pulse length for Headlights");
    }
}

int updateDriveState(int pulse){
  if(pulse > 1600 && pulse < 2100){         // It is in Reverse
    return 2;
  } else if(pulse > 1400 && pulse < 1600){  // It is in Drive
    return 1;
  } else if(pulse < 1400 && pulse > 900){   // It is in Neutral
    return 0;
  } else {
    stopProgram("Invalid pulse length for Shift Switch");
  }
}

void updateSteering(int pulse){
  if(pulse > 2100 || pulse < 900)
    stopProgram("Invalid pulse length for Steering");
  
  servoPosNew = map(pulse, 960, 2020, 2, 180); // map the input pulse to between 0 and 180 degrees, and output to the servo
  if((steeringServo.read() - servoPosNew < 4) && (steeringServo.read() - servoPosNew > -4))
    return;
  else
    steeringServo.write(servoPosNew);
}

void setDefault(){ // reset all values to initial values
    detachInterrupt(digitalPinToInterrupt(DATA_PIN));
    data_length[0] = 9400;
    data_length[1] = 1500;
    data_length[2] = 1500;
    data_length[3] = 1000;
    data_length[4] = 1500;
    data_length[5] = 1000;
    data_length[6] = 1000;
    data_length[7] = 1500;
    data_length[8] = 1500;
    canStart = false;
    dataStart = false;
    pos = 1;
    attachInterrupt(digitalPinToInterrupt(DATA_PIN), read_PPM, RISING);
}

void stopProgram(String str){ // if we occur and error, print out the reason and stop the car at all idle settings
  setDefault();
  stopIt = true;
  Serial.println(str);
  return;
}
