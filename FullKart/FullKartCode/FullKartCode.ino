 /*    Code for CSU EKart Senior Design project
 *    By: Deagan Malloy
 *    
 *    PINS:
 *      VESC_left       - Serial1 - RX1 TX1 - 0,  1
 *      VESC_right      - Serial2 - RX2 TX2 - 7,  8
 *      Screens         -   I2C   - SDA SCL - 18, 19
 *      Throttle Pedal  -   ADC   - A8      - 22
 *      Brake Pressure  -   ADC   - A9      - 23
 *      Reverse Switch  - Digital - D Read  - 2
 */

#include<VescUart.h>  // for the UART communication between VESC and MEGA

#include<Wire.h>      // for the I2C communication between Screens and MEGA
#include<LCD.h>
#include<LiquidCrystal_I2C.h>

// Const values to be used throughout program
const float MAXMOTORCURRENT = 20;   // Should be set to 50 once the full goKart is built - absolutleMax value is around 200
const float MAXMOTORDUTY = .50;     // Should probably stay here once the full kart is built
const float MAXREVERSECURRENT = 5;  // Will also get larger as we have more testing, but should be less than the forward current and Duty cycle
const float MAXREVERSEDUTY = .20;   // 
const float MAXBRAKECURRENT = 20;   // Max current for setBrakeCurrent()

// Analog pin for acceleration pedal and brake pedal
const int accel_in = A8;
const int brake_in = A9;

// Digital switches for switches on dash
const int switch1 = 2;    // For Drive / Reverse Functionality
const int switch2 = 3;    // unused
const int switch3 = 4;    // unused
const int switch4 = 5;    // unused

// Initialize the two VESCs
VescUart VESC_left;
VescUart VESC_right;

// Initialize every screen
LiquidCrystal_I2C screen1(0x27, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen2(0x26, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen3(0x25, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen4(0x24, 2, 1, 0, 4, 5, 6, 7);

// to make custom characters for screen //
uint8_t fullBox[8] = {0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7}; // 
uint8_t halfBox[8] = {0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7}; // 

// Predefine all functions within this sketch
// Might make a header file in order to simplify this code
//  This header will contain all of these functions
void initialize_screens();
void updateScreens();
void updateDrive();
float mapping(float input, float a, float b, float c, float d);
void updateScreen1();
void updateScreen2();
void updateScreen3();
void updateScreen4();


void setup() {
  Serial.begin(9600);     // baud rate for Serial Monitor for debugging
  while(!Serial){;}       // wait while Serial Monitor not open
  Serial1.begin(115200);  // initialize serial port 1 for VESC_left
  VESC_left.setSerialPort(&Serial1);
  //VESC_left.setDebugPort(&Serial);
  
  Serial2.begin(115200);  // initialize serial port 2 for VESC_right;
  VESC_right.setSerialPort(&Serial2);
  //VESC_right.setDebugPort(&Serial);

  // Make call to initialize the 4 LCD displays
  initialize_screens();

  while(!Serial){;}
}

void loop() {
   updateDrive();
   //updateScreens();
}


// Function to read in the analogInput from the Acceleration pedal
// Maps that 0-5V value to a duty cycle (0-maxMotorDuty)
// Prints off the time the function took as well for testing purposes
void updateDrive(){
  static int lastRead = 260;
  long unsigned int startTime = micros();
  int pedal = analogRead(accel_in); // Because we are running on 3.3V not 5V now this reads between 160-850 
  int brake = analogRead(brake_in); // Read the brake input
  
  Serial.print("Pedal in: "); // around 250-800 instead of the 0-1024 on 0-5V
  Serial.println(pedal);
  Serial.print("D/R   in: ");
  Serial.println(digitalRead(switch1));
  Serial.print("Brake in: ");
  Serial.println(brake);
  
  float current = 0;
  float reverseCurrent = 0;

  if(pedal > 300 && pedal < 1024){
    current = mapping(pedal, 260, 850, 0, MAXMOTORCURRENT);
    VESC_right.setCurrent(current);
    VESC_left.setCurrent(current);
  } else {
    VESC_right.setBrakeCurrent(MAXBRAKECURRENT);
    VESC_left.setBrakeCurrent(MAXBRAKECURRENT);
  }

  // No braking engaged
  /*
  if(pedal > 260 && pedal < 800 && brake < 20){ 
    current        = mapping(pedal, 260, 850, 0, MAXMOTORCURRENT);
    reverseCurrent = mapping(pedal, 260, 850, 0, MAXREVERSECURRENT);

  // Braking Engaged, limit the current
  } else if(brake >= 20){ // Brakes have been engaged, need to limit driving power
    int maxCurrent = mapping(brake, 0, 1024, 0, MAXMOTORCURRENT); // 0, 1024 might have to change dependi g on how the sensor works
    current = mapping(pedal, 260, 850, 0, MAXMOTORCURRENT) - maxCurrent;
    if(current < 0) current = 0;

    int maxReverseCurrent = mapping(brake, 0, 1024, 0, MAXREVERSECURRENT);  // 0, 1024 might have to change depending on how the sensor works
    reverseCurrent = mapping(pedal, 260, 850, 0, MAXREVERSECURRENT);
    if(reverseCurrent < 0) reverseCurrent = 0;

    Serial.print("Current: ");
    Serial.println(current);
    Serial.print("reverse: ");
    Serial.println(reverseCurrent);
    float brakeCurrent = mapping(brake, 0, 1024, 0, MAXBRAKECURRENT);
    VESC_right.setBrakeCurrent(brakeCurrent);
    VESC_left.setBrakeCurrent(brakeCurrent);
    Serial.print("Brake Current: ");
    Serial.println(brakeCurrent);
    

  // else default to no current
  } else {   
    current = 0;
    reverseCurrent = 0;
  }
  
  /*if(digitalRead(switch1) == 0){ // Forward Mode
    VESC_right.setCurrent(current);
    VESC_left.setCurrent(current);
    
  } else {                  // Reverse 
    VESC_right.setCurrent(-reverseCurrent);
    VESC_left.setCurrent(-reverseCurrent);
  }*/


  
  
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
// Screen 1 - RPM of both motors
// Screen 2 - Temp of both motors
// Screen 3 - Current on both motors
// Screen 4 - Voltage on both motors
void updateScreens(){
  if(VESC_left.getVescValues() && VESC_right.getVescValues()){
    long unsigned int startTime = micros();

    static int screenToBeUpdated = 1;
    switch (screenToBeUpdated) {
      case 1:
        updateScreen1();
        break;
      case 2:
        updateScreen2();
        break;
      case 3:
        updateScreen3();
        break;
      case 4:
        updateScreen4();
        break;
    }

    if(++screenToBeUpdated > 4)
    screenToBeUpdated = 1;

    long unsigned int TotalTime = micros() - startTime;
    Serial.print("Time to run updateScreens: ");
    Serial.print(TotalTime);
    Serial.println("us");
  } else if(!VESC_right.getVescValues()) {
    Serial.println("Could not get data from right VESC");
  } else if(!VESC_left.getVescValues()){
    Serial.println("Could not get data from left VESC");
  } else {
    Serial.println("Could not get data from both VESCS");
  }
}


// The next four function are made to update 1 screen per function. 
// This allows the screens to be updated individually instead of all in one method
// Each screen takes roughly 20ms to update, so these cause a large delay within the bulk of the code
// These are updated individually to help reduce the response time of the important features

// Update the RPM of both motors
void updateScreen1(){
  char Rbuf[16];
  char Lbuf[16];

  ltoa(VESC_right.data.rpm/14, Rbuf, 10);
  ltoa(VESC_left.data.rpm/14 , Lbuf, 10);
  
  screen1.clear();
  screen1.setCursor(0,0);
  screen1.write("Left  Rpm:");
  screen1.write(Lbuf);
  screen1.setCursor(0,1);
  screen1.write("Right Rpm:");
  
  screen1.write(Rbuf);
}

// update the temperature of both motors
void updateScreen2(){
  char Rbuf[16];
  char Lbuf[16];
  
  itoa(int(VESC_left.data.tempMotor),  Lbuf, 10); // convert left motor temp to char[] to print
  itoa(int(VESC_right.data.tempMotor), Rbuf, 10); // convert right motor temp to char[] to print
  screen2.clear();
  screen2.setCursor(0,0);
  screen2.write("Left T: ");
  screen2.write(Lbuf);                        // print to screen2
  screen2.setCursor(0,1);                     // set cursor to second screen
  screen2.write("RightT: ");
  screen2.write(Rbuf);                        // print to screen2
  
}

// Current draw for both motors
void updateScreen3(){
  char Rbuf[16];
  char Lbuf[16];

  itoa(int(VESC_left.data.avgMotorCurrent), Lbuf, 10);
  itoa(int(VESC_right.data.avgMotorCurrent), Rbuf, 10);
  screen3.clear();
  screen3.setCursor(0,0);
  screen3.write("Left : ");// averageMotorCurrent and inputVoltage
  screen3.write(Lbuf);
  screen3.setCursor(15,0);
  screen3.write("A");
  screen3.setCursor(0,1);
  screen3.write("Right: ");
  screen3.write(Rbuf);
  screen3.setCursor(15,1);
  screen3.write("A");
  
}

// Voltage of both motors
void updateScreen4(){
  char Rbuf[16];
  char Lbuf[16];
  
  itoa(int(VESC_left.data.inpVoltage), Lbuf, 10);
  itoa(int(VESC_right.data.inpVoltage), Rbuf, 10);
  screen4.clear();
  screen4.setCursor(0,0);
  screen4.write("Left : ");
  screen4.write(Lbuf);
  screen4.setCursor(15,0);
  screen4.write("V");
  screen4.setCursor(0,1);
  screen4.write("Right: ");
  screen4.write(Rbuf);
  screen4.setCursor(15,1);
  screen4.write("V");
  
}

// Function to Initialize the screens
// Turns on the backlight, and prints "Initializing to all screens"
void initialize_screens(){
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
