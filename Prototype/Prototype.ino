/*  Code For E-GoKart Senior Design Project for Colorado State University
    Written By: Deagan Malloy
    Written For: Arduino Mega
    This will be the code which takes the PWM signals from the remote and controls the motors, headlights, steering, and steering mode.
    It will also take in distance measurements from LiDar modules and factors that distance into the drive output.
    The requirements are two UART Tx and Rx ports, 4 digital PWM input pins, 3 digital PWM output pins, and 1 analog output pin.

    PINS:  2: Throttle input
           3: Steer Input
           4: Headlights switch input
           5: Neutral/ Drive/ Reverse switch input
           9: Output for the Steering Servo
          10: PWM output for 1st input to H-Bridge
          11: PWM output for 2nd input to H-Bridge
          22: Output to turn on Headlights
*/

#include <Servo.h>
Servo steeringServo;

#include "TFMini.h"
TFMini Sensor1;
TFMini Sensor2;

const int throttle = 2;           // channel 3 on reciever  // 2 for Mega
const int steer = 3;              // channel 4 on reciever  // 3 for Mega
const int headlights = 4;         // channel 5 on reciever  // 4 for Mega
const int DriveOrReverse = 5;     // channel 6 on reciever  // 5 for Mega


const int servoPIN = 9;           // Pin for servo output   // 9 for Mega
const int PWMout1 = 7;           // These two will be the outputs for the motors // 10 for Mega
const int PWMout2 = 6;           // Left and right motor will run at same speed by only using 2 outputs  // 11 for Mega
const int lightsPIN = 8;         // Pin used to turn the headlights on and off   // 22 for Mega

const int echoPin = 22;
const int trigPin = 23;

void setup() {
  pinMode(throttle, INPUT);
  pinMode(steer, INPUT);
  pinMode(DriveOrReverse, INPUT);
  pinMode(headlights, INPUT);

  pinMode(PWMout1, OUTPUT);
  pinMode(PWMout2, OUTPUT);
  pinMode(lightsPIN, OUTPUT);

  steeringServo.attach(servoPIN);

  Serial.begin(9600);                                          // Serial with baud rate of 9600
  Serial3.begin(115200);
  Serial4.begin(115200);                                       // Serials for the Sensors

  Sensor1.begin(&Serial3);  // was Serial1 on Mega
  Sensor2.begin(&Serial4);  // was Serial2 on Mega             // Initialize Sensors with Serial

  //Sensor1.setSingleScanMode();
  //Sensor2.setSingleScanMode();                                 // Force each sensor to only read when triggered

  delay(50);                                                   // Let signals stabilize
}

void loop() {
  // only initialize the variables first time
  static int throttle_pulse = 1000;                            // These 4 are the pulse length in microseconds that they are active high
  static int steer_pulse = 1500;                               // These 4 are the pulse length in microseconds that they are active high
  static int drive_state_pulse = 1000;                         // These 4 are the pulse length in microseconds that they are active high
  static int headlights_pulse = 1000;                          // These 4 are the pulse length in microseconds that they are active high
  static int drive_state = 0;                                  // 0=neutral, 1=drive, 2=reverse
  static int counter = 0;

  // switch statement to control which function to update
  switch (counter) {
    case 0:
      throttle_pulse = pulseIn(throttle, HIGH, 30000);
      updateThrottleOut(throttle_pulse, drive_state);
      break;
    case 1:
      steer_pulse = pulseIn(steer, HIGH, 30000);
      updateSteering(steer_pulse);
      break;
    case 2:
      drive_state_pulse = pulseIn(DriveOrReverse, HIGH, 30000);
      drive_state = updateDriveState(drive_state_pulse);
      break;
    case 3:
      headlights_pulse = pulseIn(headlights, HIGH, 30000);
      updateHeadlights(headlights_pulse);
      break;
    default:
      counter = -1;
      break;
  }
  counter++;
}

/* takes in 2 integers as argumetns
   pulse is the pulse width in us from pin 2, or 3 on the reciever
   drive_state is the int 0, 1, or 2 for neutral, drive, or reverse
   complex function to determine the duty cycle to set to the motors
   uses analogWrite to output a PWM signal with different duty cycles to conrtol motor speeds
   returns void
*/
void updateThrottleOut(int &pulse, int &drive_state) {
  static int setRpm1 = 0, setRpm2 = 0;            // Initialize RPM's

  if (drive_state == 0) {                         // Neutral - Ouput Low Low to start breaking and bring car to stop
    setRpm1 = 0;
    setRpm2 = 0;
    analogWrite(PWMout1, setRpm1);                // Set both PWM to 0
    analogWrite(PWMout2, setRpm2);                // Set both PWM to 0

  } else if (drive_state == 1) {                  // Drive - Output High, Low to drive forward
    int maxRPM = 0;
    if (pulse > 1100 && pulse < 2050) {     // Good input pulse
      uint16_t Dist = distanceCheck();        // distance to closest object (in cm)
      setRpm1 = map(pulse, 1100, 2050, 0, 255);
      Serial.print("one: ");
      Serial.println(setRpm1);

      if (Dist < 400 && Dist >= 0) {                  // if distance to object is too close -> set maxRPM
        maxRPM = map(Dist, 0, 400, 0, 255);
        Serial.print("Dis: ");
        Serial.println(Dist);
        Serial.print("Max: ");
        Serial.println(maxRPM);
        if (setRpm1 > maxRPM) setRpm1 = maxRPM;
        if (setRpm1 < 0) setRpm1 = 0;
        Serial.print("two: ");
        Serial.println(setRpm1);
      }

    } else {                                // Bad input pulse
      setRpm1 = 0;                              // If input pulse is too slow, stay still
    }

    setRpm2 = 0;                                // Second needs to be Low to drive Forwards
    analogWrite(PWMout2, setRpm2);              // Set second PWM Low
    analogWrite(PWMout1, setRpm1);              // Set first PWM to PWM signal

  } else if (drive_state == 2) {                  // Reverse - Output Low High to drive backwards (just opposite of drive)
    if (pulse > 1100 && pulse < 2050)
      setRpm2 = map(pulse, 1100, 2050, 0, 127); // map input pulse to PWM output duty cycle between 0 and 255
    else
      setRpm2 = 0;                              // If input pulse is too slow, stay still

    setRpm1 = 0;                                // First needs to be Low to Reverse
    analogWrite(PWMout1, setRpm1);              // Set first PWM Low
    delay(10);                                  // Small delay
    analogWrite(PWMout2, setRpm2);              // Set second PWM to PWM signal

  } else {                                        // Default Case - Output Low Low to stop
    setRpm1 = 0;
    setRpm2 = 0;
    analogWrite(PWMout1, setRpm1);              // Set them both to 0
    analogWrite(PWMout2, setRpm2);              // Set them both to 0
  }
}

/* takes in 1 int named pulse
   pulse is the pulse width in microseconds from pin 4 on the teensy, or 5 on the reciever
   based on the pulse width from input, turns on or off the headlights on the car
   return void
*/
void updateHeadlights(int &pulse) {               // Reads in if pulse is long or short, and turns on and off headlights
  if (pulse > 1500 && pulse < 2050)
    digitalWrite(lightsPIN, HIGH);              // Turn on headlights
  else
    digitalWrite(lightsPIN, LOW);               // Turn off headlights
}

/*  takes in one int named pulse
    pulse is the pulse width in microseconds from pin 5 on teensy, 6 on reciever
    returns 0, 1, or 2 for drive state
    0 = neutral
    1 = drive
    2 = reverse
*/
int updateDriveState(int &pulse) {
  if (pulse > 1600 && pulse < 2050) {             // Reverse
    return 2;
  } else if (pulse > 1400 && pulse <= 1600) {     // Drive
    return 1;
  } else if (pulse > 950 && pulse <= 1400) {      // Neutral
    return 0;
  } else {                                        // Default to Neutral
    return 0;
  }
}

/* Takes in one integer named pulse
    pulse is the pulse width in microseconds from pin 3 on teensy, or 4 on reciever
    maps this pulse width to steer the car in the corresponding direction
    no return value
*/
void updateSteering(int &pulse) {
  static int servoPosNew = 90;

  if (pulse < 950 || pulse > 2050) {              // If invalid pulse length, return to original position
    steeringServo.write(90);
    return;
  }

  servoPosNew = map(pulse, 950, 2050, 0, 180);
  if (servoPosNew > 180) servoPosNew = 180;                   // don't allow it to go over 180 degrees
  if (servoPosNew < 0)   servoPosNew = 0;                     // don't allow it to go below 0 degrees
  if (servoPosNew < 100 && servoPosNew > 80) servoPosNew = 90; // stop the jitter when it turns on

  steeringServo.write(servoPosNew);               // Write the new position
  delay(50);
}

/*  takes in no arguments
    returns an int for the closest distance from either lidar sensors, or ultrasonic sensor
    return value is cm
*/
uint16_t distanceCheck() {
  Serial.println("In distCheck");
  uint16_t distance_one = 0;
  uint16_t distance_two = 0;               // Initialize the Variables
  distance_one = Sensor1.getDistance();             // Read Data
  Serial.println(distance_one);
  distance_two = Sensor2.getDistance();
  Serial.println(distance_two);

  uint8_t shortestDistance = 0;

  if (distance_one > distance_two) {              // Return the distance to the closest object on either sensor
    shortestDistance = distance_two;
  } else {
    shortestDistance = distance_one;
  }

  return shortestDistance;
}
  // Measurements which are out of range will give the value 65535, which is the maximum value of a 16 bit unsigned integer.
  // From 0 to 30cm and 30m plus, the readings are unreliable in LiDar.
  // To overcome this, an ultrasonic sensor is used in the case of distances larger than 65530, 5 below the out of range value, and distances shorter than 50cm.
