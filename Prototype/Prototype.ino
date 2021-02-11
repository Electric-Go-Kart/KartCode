/*
  Code By: Deagan Malloy - Code for the Arduino MEGA - subject to change to PPM for the remote instead of PWM
  For: Senior design, will be basic code for how to interface with the remote control for the toy car
  
  PINS: 2: Throttle input
        3: Steer Input
        4: Headlights switch input
        5: Neutral/ Drive/ Reverse switch input

        9 : Output for the Steering Servo
        10: PWM output for 1st input to H-Bridge
        11: PWM output for 2nd input to H-Bridge
        22: Output to turn on Headlights (TBD)
*/

#include<Servo.h>
Servo steeringServo;
const int servoPIN = 9;       //pin for servo output

// setting the four inputs from the reciever to PWM in ports
const int throttle = 2;       // channel 3 on reciever
const int steer = 3;          // channel 4 on reciever
const int DriveOrReverse = 5; // channel 6 on reciever
const int headlights = 4;     // channel 5 on reciever

const int PWMout1 = 10;       // these two will be the outputs for the motors
const int PWMout2 = 11;       // left and right motor will run at same speed by only using 2 outputs
     
const int lightsPIN = 22;     // pin used to turn the headlights on and off

int servoPosNew = 90;

void setup() {
  pinMode(throttle, INPUT);
  pinMode(steer, INPUT);
  pinMode(DriveOrReverse,INPUT);
  pinMode(headlights, INPUT);

  pinMode(PWMout1, OUTPUT);
  pinMode(PWMout2, OUTPUT);
  pinMode(lightsPIN, OUTPUT);

  steeringServo.attach(servoPIN);

  Serial.begin(9600); // starts serial with baud rate of 9600
  delay(50);
}

void loop() {
  static int throttle_pulse = 1000; // These 4 are the pulse length in microseconds that they are active high
  static int steer_pulse = 1500;
  static int drive_state_pulse = 1000;
  static int headlights_pulse = 1000;
  static int drive_state = 0; // 0=neutral, 1=drive, 2=reverse
  static int channel_in = 0; // 0=throttle, 1=steer, 2=drive_state, 3=headlights - counter for getting pulse inputs
  
// this will be moved to interupt section that runs every 20ms (how long the period is of PWM signals to DC motor
  /*if(channel_in == 0){ // update throttle
    throttle_pulse = pulseIn(throttle, HIGH, 30000);
    updateThrottleOut(throttle_pulse, drive_state);
    channel_in++;
 
  } else if(channel_in == 1){ // update steering direction
    steer_pulse = pulseIn(steer, HIGH, 30000);
    updateSteering(steer_pulse);
    channel_in++;
    
  } else if(channel_in == 2){ // update drive_state
    drive_state_pulse = pulseIn(DriveOrReverse, HIGH, 30000);
    drive_state = updateDriveState(drive_state_pulse);
    channel_in++;
    
  } else {  // update headlights on/off
    headlights_pulse = pulseIn(headlights, HIGH, 30000);
    updateHeadlights(headlights_pulse);
    channel_in = 0;
  }*/

  throttle_pulse = pulseIn(throttle, HIGH, 30000);
  updateThrottleOut(throttle_pulse, drive_state);

  steer_pulse = pulseIn(steer, HIGH, 30000);
  updateSteering(steer_pulse);

  drive_state_pulse = pulseIn(steer, HIGH, 30000);
  updateDriveState(drive_state_pulse);

  headlights_pulse = pulseIn(headlights, HIGH, 30000);
  updateHeadlights(headlights_pulse);
  

}

void updateThrottleOut(int &pulse, int &drive_state){
  static int setRpm1 = 0, setRpm2 = 0;

  if(drive_state == 0){ // We are in Neutral - Ouput L L to start breaking and bring car to stop
    setRpm1 = 0;
    setRpm2 = 0;
    analogWrite(PWMout1, setRpm1);  // set them both to 0
    analogWrite(PWMout2, setRpm2);  // set them both to 0
    
  } else if(drive_state == 1){                // We are in Drive - Output H L to drive forward
    if(pulse > 1100 && pulse < 2050)
      setRpm1 = map(pulse, 1100, 2050, 0, 255); // map input pulse to PWM output duty cycle between 0 and 255
    else 
      setRpm1 = 0;                              // if input pulse is too slow, stay still
    setRpm2 = 0;                                // Second needs to be low to drive Forwards
    analogWrite(PWMout2, setRpm2);  // set them both to 0
    delay(10);                      // small delay
    analogWrite(PWMout1, setRpm1);  // set PWM1 to PWM signal
    
  } else if(drive_state == 2){                // We are in Reverse - Output L H to drive backwards (just opposite of drive)
   if(pulse > 1100 && pulse < 2050)
      setRpm2 = map(pulse, 1100, 2050, 0, 255); // map input pulse to PWM output duty cycle between 0 and 255
    else 
      setRpm2 = 0;                              // if input pulse is too slow, stay still
    setRpm1 = 0;                                // First needs to be low to Reverse
    analogWrite(PWMout1, setRpm1);  // set them both to 0
    delay(10);                      // small delay
    analogWrite(PWMout2, setRpm2);  // set PWM2 to PWM signal
    
  } else {  // should never happen, but if something goes wrong, do nothing by outputting both Low
    setRpm1 = 0;
    setRpm2 = 0;
    analogWrite(PWMout1, setRpm1);  // set them both to 0
    analogWrite(PWMout2, setRpm2);  // set them both to 0
  }
}

void updateHeadlights(int &pulse){ // Reads in if pulse is long or short, and turns on and off headlights
    if(pulse > 1500 && pulse < 2050){
      // turn on headlights - just output constant 5V output
      digitalWrite(lightsPIN, HIGH);
    } else {
      // turn off headlights - turn off that output
      digitalWrite(lightsPIN, LOW);
    }
}

int updateDriveState(int &pulse){
  if(pulse > 1600 && pulse < 2050){ // It is in Reverse
    return 2;
  } else if(pulse > 1400 && pulse <= 1600){ // It is in Drive
    return 1;
  } else if(pulse > 950 && pulse <= 1400){ // It is in Neutral
    return 0;
  }
}

void updateSteering(int &pulse){
  if(pulse < 950 || pulse > 2050){  // if invalid pulse length, return to original position 
    steeringServo.write(90);  
    return;
  } 
  servoPosNew = map(pulse, 980, 2020, 0, 180);// + 12;  // Will need to change this to only rotate 90 degrees // with of offset of 12 currently in order to recenter after turning 
  if(servoPosNew > 180)
    servoPosNew = 180;
  steeringServo.write(servoPosNew);
  delay(50);
}
