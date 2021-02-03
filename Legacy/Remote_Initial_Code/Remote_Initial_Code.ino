/*
  Code By: Deagan Malloy - Code for the Arduino UNO - subject to change to PPM for the remote instead of PWM
  For: Senior design, will be basic code for how to interface with the remote control for the toy car
  PINS: 3: Throttle input
        5: Steer Input
        6: Neutral/ Drive/ Reverse switch input
        9: Headlights switch input
        10: PWM output for 1st input to H-Bridge
        11: PWM output for 2nd input to H-Bridge
        ~: Output to turn on Headlights (TBD)
*/

// Setting the four channel input pins from the reciever for PWM
const int throttle = 3;       // Channel 3 from Reciever
const int steer = 5;          // Channel 4 from Reciever
const int DriveOrReverse = 6; // Channel 6 from Reciever
const int headlights = 9;     // Channel 5 from Reciever

const int PWMout1 = 10; // these two will be the outputs for the motors
const int PWMout2 = 11; // left and right motor will run at same speed by only using 2 outputs

void setup() {
  pinMode(throttle, INPUT);
  pinMode(steer, INPUT);
  pinMode(DriveOrReverse,INPUT);
  pinMode(headlights, INPUT);

  pinMode(PWMout1, OUTPUT);
  pinMode(PWMout2, OUTPUT);

  Serial.begin(9600);
  delay(50); 
}

void loop() {
  static int throttle_pulse = 0; // These 4 are the pulse length in microseconds that they are active high
  static int steer_pulse = 0;
  static int drive_state_pulse = 0;
  static int headlights_pulse = 0;
  static int drive_state = 0; // 0=neutral, 1=drive, 2=reverse
  static int channel_in = 0; // 0=throttle, 1=steer, 2=drive_state, 3=headlights - counter for getting pulse inputs
  
// this will be moved to interupt section that runs every 20ms (how long the period is of PWM signals to DC motor
  if(channel_in == 0){ // update throttle
    throttle_pulse = pulseIn(throttle, HIGH, 30000);
    updateThrottleOut(throttle_pulse, drive_state);
    channel_in++;
 
  } else if(channel_in == 1){ // update steering direction
    steer_pulse = pulseIn(steer, HIGH, 30000);
    // need analog read to read in the input from the potentiometer
    int cur_pos = 0;
    updateSteering(steer_pulse, cur_pos); 
    channel_in++;
    
  } else if(channel_in == 2){ // update drive_state
    drive_state_pulse = pulseIn(DriveOrReverse, HIGH, 30000);
    drive_state = updateDriveState(drive_state_pulse);
    channel_in++;
    
  } else {  // update headlights on/off
    headlights_pulse = pulseIn(headlights, HIGH, 30000);
    updateHeadlights(headlights_pulse);
    channel_in = 0;
  }
}

void updateThrottleOut(int &pulse, int &drive_state){
  static int setRpm1 = 0, setRpm2 = 0;
  
  if(drive_state == 0){ // We are in Neutral - Ouput L L to start breaking and bring car to stop
    setRpm1 = 0;
    setRpm2 = 0;
    analogWrite(PWMout1, setRpm1);
    analogWrite(PWMout2, setRpm2);
    
  } else if(drive_state == 1){                // We are in Drive - Output H L to drive forward
    if(pulse > 1100)
      setRpm1 = map(pulse, 1100, 2000, 0, 200); // map input pulse to PWM output duty cycle between 0 and 255 (changed to 200 to not blow up PMOS)
    else 
      setRpm1 = 0;                              // if input pulse is too slow, stay still
    setRpm2 = 0;                                // Second needs to be low to drive Forwards
    analogWrite(PWMout1, setRpm1);
    analogWrite(PWMout2, setRpm2);
    
  } else if(drive_state == 2){                // We are in Reverse - Output L H to drive backwards (just opposite of drive)
   if(pulse > 1100)
      setRpm2 = map(pulse, 1100, 2000, 0, 200); // map input pulse to PWM output duty cycle between 0 and 255 (changed to 200 to not blow up PMOS)
    else 
      setRpm2 = 0;                              // if input pulse is too slow, stay still
    setRpm1 = 0;                                // First needs to be low to Reverse
    analogWrite(PWMout1, setRpm1);
    analogWrite(PWMout2, setRpm2);
    
  } else {  // should never happen, but if something goes wrong, do nothing by outputting both Low
    setRpm1 = 0;
    setRpm2 = 0;
    analogWrite(PWMout1, setRpm1);
    analogWrite(PWMout2, setRpm2);  
  }
}

void updateHeadlights(int &pulse){ // just reads in if switch is flipped or not, and turns on headlights ... pretty basic
    if(pulse > 1500){
      // turn on headlights - just output constant 5V output
    } else {
      // turn off headlights - turn off that output
    }
}

int updateDriveState(int &pulse){
  if(pulse > 1600){ // It is in Reverse
    return 2;
  } else if(pulse > 1400){ // It is in Drive
    return 1;
  } else { // It is in Neutral
    return 0;
  }
}

void updateSteering(int &pulse, int &pos){ // This is probably going to run off of another HBridge, it should also have some sort of position feedback however. I need to decode that feedback to find the position of the motor, then adjust depending on the difference from those two inputs
  if(pulse > 1600){ // steering right
    
  } else if(pulse > 1400){ // in the middle
    
  } else if(pulse > 980){ // steering left
    
  } else { // should never happen, but do not change steering
    
  }
}
