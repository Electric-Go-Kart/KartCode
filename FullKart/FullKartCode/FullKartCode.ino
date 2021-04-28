 /*    Code for CSU EKart Senior Design project
 *    By: Deagan Malloy
 *    
 *    PINS:
 *      VESC_left       - Serial1 - RX1 TX1 - 0,  1
 *      VESC_right      - Serial2 - RX2 TX2 - 7,  8
 *      Screens         -   I2C   - SDA SCL - 18, 19
 *      Throttle Pedal  -   ADC   - A8      - 20
 *      Brake Pressure  -   ADC   - A9      - 23
 *      Reverse Switch  - Digital - D Read  - 2
 *      Unused          -         -         - 3
 *      Headlight Sw    - Digital - D Read  - 4
 *      Limit Speed Sw  - Digital - D Read  - 5
 *      Pin for Fans    - PWM     - PWM out - 6
 */

#include<VescUart.h>  // for the UART communication between VESC and MEGA

#include<Wire.h>      // for the I2C communication between Screens and MEGA
#include<LCD.h>
#include<LiquidCrystal_I2C.h>

// Const values to be used throughout program
const float MAXMOTORCURRENT = 100;  // Absolute max per motor should be 150A
const float MAXMOTORDUTY = .35;     // NOT USED CURRENTLY - planning on doing error checking if we go over certain duty cycle on motors
const float MAXREVERSECURRENT = 5;  // Absolute max per motor should be like 100A
const float MAXREVERSEDUTY = .20;   // NOT USED CURRENTLY - planning on doing error checking similar to forward driving
const float MAXBRAKECURRENT = 10;   // Max current for regenerative braking

// Analog pin for acceleration pedal and brake pedal
const int accel_in = A6;
const int brake_in = A9;

// Digital switches for switches on dash
const int switch1 = 2;    // For Drive / Reverse Functionality
const int switch2 = 3;    // unused
const int switch3 = 4;    // Headlights on
const int switch4 = 5;    // For limited mode - max current cut in half

// Initialize the two VESCs
VescUart VESC_left; 
VescUart VESC_right;

// Initialize every screen
LiquidCrystal_I2C screen1(0x27, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen2(0x26, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen3(0x25, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C screen4(0x24, 2, 1, 0, 4, 5, 6, 7);

// to make custom characters for screen //
uint8_t fullBox[8] = {0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7}; 
uint8_t halfBox[8] = {0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7}; 
uint8_t Full[8]    = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
uint8_t Half[8]    = {0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c};
uint8_t Low[8]     = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
uint8_t Left[8]    = {0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03};
uint8_t Right[8]   = {0x18, 0x08, 0x08, 0x0c, 0x0c, 0x08, 0x08, 0x18};


// Predefine all functions within this sketch
// Might make a seperate file in order to simplify this code
//  That header will contain all of these functions
void initialize_screens();
void updateScreens();
void updateDrive();
void updateScreen1();
void updateScreen2();
void updateScreen3();
void updateScreen4();
float mapping(float input, float in_min, float in_max, float out_min, float out_max);

/*  Setup Function
 *   Start Serial Ports 1 and 2 at 115200 for communication with VESCs
 *   Create custom characters to be used on the LCD Displays for battery
 *   Initialize all screens to say "Initializing"
 * 
 */
void setup() {
  Serial.begin(9600);     // baud rate for Serial Monitor for debugging
  //while(!Serial){;}       // wait while Serial Monitor not open
  Serial1.begin(115200);  // initialize serial port 1 for VESC_left
  VESC_left.setSerialPort(&Serial1);
  //VESC_left.setDebugPort(&Serial);
  
  Serial2.begin(115200);  // initialize serial port 2 for VESC_right;
  VESC_right.setSerialPort(&Serial2);
  //VESC_right.setDebugPort(&Serial);

  // Create custom Characters
  screen4.createChar(1, Full);
  screen4.createChar(2, Half);
  screen4.createChar(3, Low);
  screen4.createChar(4, Left);
  screen4.createChar(5, Right);

  // Make call to initialize the 4 LCD displays
  initialize_screens();
}

/*  Loop function
 *   Update Drive is called
 *      - This function will read in all values necessary and update the currents to the motors
 *   Update Screens is called
 *      - This function will read values from the VESCs and will display them onto the LCD screens
 */
void loop() {
   updateDrive();
   updateScreens();
}


// Function to read in the analogInput from the Acceleration pedal
// Maps that 0-5V value to a duty cycle (0-maxMotorDuty)
// Prints off the time the function took as well for testing purposes
void updateDrive(){
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

/* Need to add in a safety feature to not allow immediate switch from forward to reverse while kart is moving
 *  
 */
  if(pedal > 350 && pedal < 1024){  // We have stepped on the gas
    if(digitalRead(switch1)){                        // have toggled the reverse switch
      current = -mapping(pedal, 0, 1024, 0, MAXREVERSECURRENT);   // sets current to negative current for reverse
    } else {                                                  // are still in Drive
      current = mapping(pedal, 0, 1024, 0, MAXMOTORCURRENT);      // sets current to positive for drive
    }

    VESC_right.setCurrent(current);
    VESC_left.setCurrent(current); 

    Serial.print("Current being set: ");
    Serial.println(current);
    Serial.println("");

/*  Need to add in sequential braking so that
 *  the kart uses more current to brake at higher speeds
 */
  //} else if(pedal > 290){    
  //  VESC_right.setCurrent(0);
  //  VESC_left.setCurrent(0); 
  
  } else {    // We are not stepping down on pedal, so apply braking through motors
    VESC_right.setBrakeCurrent(MAXBRAKECURRENT);
    VESC_left.setBrakeCurrent(MAXBRAKECURRENT);
  }
  
  long unsigned int totalTime = micros() - startTime;
  Serial.print("Time to run updateDrive: ");
  Serial.print(totalTime);
  Serial.println("us");
  return;
}

// works the same as the built in map function, but returns a
// float for more accurate mapping at smaller values
float mapping(float input, float in_min, float in_max, float out_min, float out_max){
  return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// Function to update and display values to the screens
// Screen 1 - RPM of both motors
// Screen 2 - Temp of both motors
// Screen 3 - Current on both motors
// Screen 4 - Voltage on both motors
void updateScreens(){
  bool leftGood = VESC_left.getVescValues();
  bool rightGood = VESC_right.getVescValues();
  
  if(leftGood && rightGood){  // If we have connection to both VESCs - update the screens
    long unsigned int startTime = micros();

    Serial.print("Voltage remaining: ");
    Serial.println(VESC_right.data.inpVoltage);

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
    Serial.print("Screen num: ");
    Serial.println(screenToBeUpdated);

    long unsigned int TotalTime = micros() - startTime;
    Serial.print("Time to run updateScreens: ");
    Serial.print(TotalTime);
    Serial.println("us");

  // If not connected to one of the VESCs - print out which VESC lost connection
  // Will later update this to one of the LCD displays
  } else if(!leftGood && !rightGood) {
    Serial.println("Could not get data from both VESCs");
  } else if(!leftGood){
    Serial.println("Could not get data from left VESC");
  } else if(!rightGood){
    Serial.println("Could not get data from right VESC");
  } else {
    Serial.println("Something is going horribly wrong - good luck figuring out why");
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
// 50.4 - 42 Volts - 43V means cutoff power
void updateScreen4(){
  float voltageLeft = VESC_left.data.inpVoltage;
  float voltageRight= VESC_right.data.inpVoltage;

  float batteryV = 0;

  if(voltageLeft < voltageRight) batteryV = voltageLeft;
  else batteryV = voltageRight;

  batteryV = 52.0;

  screen4.clear();
  screen4.setCursor(0,0);
  screen4.write("Battery: ");
  
  // calculate the battery percentage
  float batteryPercent = ((52 - batteryV) / 8.4) * 100;
  Serial.print("Battery Percentage: ");
  Serial.println(batteryPercent);

  // display the battery percentage
  char buf[16];
  itoa(int(batteryPercent), buf, 10);
  screen4.write(buf);
  screen4.write("%");
  
  screen4.setCursor(1,1); // write the left side of battery
  screen4.write(4);
  screen4.setCursor(1,8); // write the right side of the battery
  screen4.write(5);
  screen4.setCursor(1,7);

  if(batteryV > 49.4) screen4.write(1);
  else screen4.write(" ");

  screen4.setCursor(1,6);
  if(batteryV > 48.4) screen4.write(1);
  else screen4.write(" ");

  screen4.setCursor(1,5);
  if(batteryV > 47.4) screen4.write(1);
  else screen4.write(" ");

  screen4.setCursor(1,4);
  if(batteryV > 46.4) screen4.write(1);
  else screen4.write(" ");

  screen4.setCursor(1,3);
  if(batteryV > 45.4) screen4.write(1);
  else screen4.write(" ");

  screen4.setCursor(1,2);
  if(batteryV > 44.4) screen4.write(1);
  else if(batteryV > 43.8) screen4.write(2);
  else if(batteryV > 43.2) screen4.write(3);
  else {  // Battery is too low, cut power to motors
    screen4.write(" ");
    // CUT POWER TO MOTORS BATTERY IS EMPTY
  }
  
  /*char Rbuf[16];
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
  screen4.write("V");*/
  
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
