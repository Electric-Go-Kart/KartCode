 /*    Code for CSU EKart Senior Design project
 *    By: Deagan Malloy
 *    
 *    PINS:
 *      VESC_left   - Serial1 - RX1 TX1 - 19, 18
 *      VESC_right  - Serial2 - RX2 TX2 - 17, 16
 *      Screens     -   I2C   - SDA SCL - 20, 21
 *      "Gas" Pedal -   ADC   - A0      - A0
 */

#include<VescUart.h>  // for the UART communication between VESC and MEGA

#include<Wire.h>      // for the I2C communication between Screens and MEGA
#include<LCD.h>
#include<LiquidCrystal_I2C.h>

// Const values to be used throughout program
const float MAXMOTORCURRENT = 10;   // Should be set to 50 once the full goKart is built
const float MAXMOTORDUTY = .50;     // Should probably stay here once the full kart is built
const float MAXREVERSECURRENT = 5;
const float MAXREVERSEDUTY = .20;

// Analog pin for acceleration pedal
const int accel_in = A0;

// Digital switches for switches on dash
const int switch1 = 22;   // For Drive / Reverse Functionality
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

void initialize_screens();
void updateScreens();
void updateDrive();
float mapping(float input, float a, float b, float c, float d);


void setup() {
  Serial.begin(9600);     // baud rate for Serial Monitor for debugging
  while(!Serial){;}       // wait while Serial Monitor not open
  Serial1.begin(115200);  // initialize serial port 1 for VESC_left
  VESC_left.setSerialPort(&Serial1);
  VESC_left.setDebugPort(&Serial);
  
  Serial2.begin(115200);  // initialize serial port 2 for VESC_right;
  VESC_right.setSerialPort(&Serial2);
  VESC_right.setDebugPort(&Serial);

  initialize_screens();
}

void loop() {
   updateDrive();
   //updateScreens();
   //Serial.println();
   delay(10);
}


// Function to read in the analogInput from the Acceleration pedal
// Maps that 0-5V value to a duty cycle (0-maxMotorDuty)
// Prints off the time the function took as well for testing purposes
void updateDrive(){
  static int lastRead = 260;
  long unsigned int startTime = micros();
  int pedal = analogRead(A0); // Because we are running on 3.3V not 5V now this reads inbetween 

  bool driving = false;
  
  Serial.print("Pedal in: "); // around 250-800 instead of the 0-1024 on 0-5V
  Serial.println(pedal);
  Serial.print("Switch: ");
  Serial.println(digitalRead(switch1));
  
  float current = 0;
  float reverseCurrent = 0;

  if(pedal > 260 && pedal < 850){ 
    current        = mapping(pedal, 260, 850, 0, MAXMOTORCURRENT);
    reverseCurrent = mapping(pedal, 260, 850, 0, MAXREVERSECURRENT);
  } else {
    current = 0;
  }
    
  if(digitalRead(switch1)){ // Forward Mode
    VESC_right.setCurrent(current);
    VESC_left.setCurrent(current);
    
  } else {                  // Reverse 
    VESC_right.setCurrent(-reverseCurrent);
    VESC_left.setCurrent(-reverseCurrent);
  }


/* To update motors:    
 *    setCurrent(float)
 *    setDuty(float)
 *    setRpm(float)
 *    setBreakCurrent(float)*/
  
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
  if(VESC_left.getVescValues()){
  long unsigned int startTime = micros();
  long R_currentRpm = VESC_right.data.rpm;
  long L_currentRpm = VESC_left.data.rpm;
  char Rbuf[16];
  char Lbuf[16];
  ltoa(R_currentRpm, Rbuf, 10);
  ltoa(L_currentRpm, Lbuf, 10);
  Serial.print("Right RPM: ");
  Serial.print(R_currentRpm);
  Serial.print("   Rbuf: ");
  Serial.println(Rbuf);
  Serial.print("Left RPM : ");
  Serial.print(L_currentRpm);
  Serial.print("   Lbuf: ");
  Serial.println(Lbuf);

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
  } else {
    Serial.println("Could not get data from VESC");
  }

  if(VESC_right.getVescValues()){
    Serial.println("Got values from the Right one");
  } else {
    Serial.println("Could not get data from Right one");
  }
}

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
