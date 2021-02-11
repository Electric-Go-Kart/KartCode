/*    Code for CSU EKart Senior Design project
 *    By: Deagan Malloy
 *    
 *    VESC_left   - Serial1 - RX1 TX1
 *    VESC_right  - Serial2 - RX2 TX2
 *    
 *    
 */

#include<VescUart.h>  // for the UART communication between VESC and MEGA

#include<Wire.h>    // for the I2C communication between Screens and MEGA
#include<LCD.h>
#include<LiquidCrystal_I2C.h>


// Const values to be used throughout program
const float maxMotorCurrent = 10;
const float maxMotorDuty = .50;

// Analog pin for acceleration pedal
const int accel_in = A0;

// Digital switches for switches on dash
const int switch1 = 22;
const int switch2 = 23;
const int switch3 = 24;
const int switch4 = 25;

// Initialize the two VESCs
VescUart VESC_left;
VescUart VESC_right;

// Initialize every screen
LiquidCrystal_I2C screen1(0x27, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen2(0x26, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen3(0x25, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen4(0x24, 2, 1, 0, 4, 5, 6, 7);

// to make custom characters for screen //
uint8_t custom[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // hex values represent that pixels to be lit up



void setup() {
  Serial.begin(9600);     // baud rate for Serial Monitor for debugging
  while(!Serial){;}       // wait while Serial Monitor not open
  Serial1.begin(115200);  // initialize serial port 1 for VESC_left
  VESC_left.setSerialPort(&Serial1);
  
  Serial2.begin(115200);  // initialize serial port 2 for VESC_right;
  VESC_right.setSerialPort(&Serial2);

  // initialize all of our screens
  screen1.begin(16,2);
  screen2.begin(16,2);
  screen3.begin(16,2);
  screen4.begin(16,2);

  screen1.setBacklightPin(3,POSITIVE);
  screen2.setBacklightPin(3,POSITIVE);
  screen3.setBacklightPin(3,POSITIVE);
  screen4.setBacklightPin(3,POSITIVE);

  screen1.setBacklight(HIGH);
  screen2.setBacklight(HIGH);
  screen3.setBacklight(HIGH);
  screen4.setBacklight(HIGH);

  screen1.clear();
  screen1.print("initializing");
  screen2.clear();
  screen2.print("initializing");
  screen3.clear();
  screen3.print("initializing");
  screen4.clear();
  screen4.print("initializing");
}

void loop() {
   updateDrive();
   //updateScreens();
   //Serial.println();
   delay(20);
}


// Function to read in the analogInput from the Acceleration pedal
// Maps that 0-5V value to a duty cycle (0-maxMotorDuty)
// Prints off the time the function took as well for testing purposes

void updateDrive(){
  long unsigned int startTime = micros();
  int pedal = analogRead(A0);
  float current = 0;
  if(pedal > 200 && pedal < 1024)
    current = mapping(pedal, 0, 1024, 0, maxMotorDuty);
  else
    current = 0;

/* To update motors:    
 *    setCurrent(float)
 *    setDuty(float)
 *    setRpm(float)
 *    setBreakCurren(float)*/
  Serial.print("Duty: ");
  Serial.println(current);
  VESC_right.setDuty(current);  // by setting current, both motors are not at same RPM
  VESC_left.setDuty(current);   // by setting duty, ensures they stay at same RPM

  long unsigned int totalTime = micros() - startTime;
  Serial.print("Time to run updateDrive: ");
  Serial.print(totalTime);
  Serial.println("us");
  return;
}

// works the same as the built in map function, but returns a
// float for more accurate mapping at smaller values
float mapping(float input, float a, float b, float c, float d){
  return (input /(b-a)*(d-c)+c);
}


// Function to update and display values to the screens
// Screen 1 - Both Motors current Speed (or just avg speed of both motors)
// Screen 2 - Battery Life left
// Screen 3 - Avg current to both motors
// Screen 4 - Temps of motors / batteries?

// Patrick says MPH, Voltage and Voltage/12(each cell estimated)
void updateScreens(){
  long unsigned int startTime = micros();
  long R_currentRpm = VESC_right.data.rpm;
  long L_currentRpm = VESC_left.data.rpm;
  char Rbuf[16];
  char Lbuf[16];
  ltoa(R_currentRpm, Rbuf, 10);
  ltoa(L_currentRpm, Lbuf, 10);

  // set pos to 0,0 and print right data
  // set pos to 1,0 and print left data
  screen1.clear();
  screen1.setCursor(0,0);
  screen1.write("Right Rpm:");
  screen1.write(Rbuf);
  screen1.setCursor(0,1);
  screen1.write("Left  Rpm:");
  screen1.write(Lbuf);

  long unsigned int TotalTime = micros() - startTime;
  Serial.print("Time to run updateScreens: ");
  Serial.print(TotalTime);
  Serial.println("us");
  return;
}
