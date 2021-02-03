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
  VESC_right.setDebugPort(&Serial);

  /*// initialize all of our screens
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
  screen4.print("initializing");*/
}

void loop() {
   Serial.print(VESC_right.getVescValues());
   if(VESC_right.getVescValues()){
    Serial.print("RPM: ");
    Serial.println(VESC_right.data.rpm);
    Serial.print("InV: ");
    Serial.println(VESC_right.data.inpVoltage);
    Serial.print("Amps: ");
    Serial.println(VESC_right.data.ampHours);
    Serial.print("Tach:");
    Serial.println(VESC_right.data.tachometerAbs);
   } else {
    Serial.println("Failed to get Data.");
   }

  delay(500);
}

void updateDrive(){
  // read in the ADC0 to determine position of pedal, and update Duty Cycle To VESC's
  /*int pedalPos = analogRead(A0);
  pedalPos = map(pedalPos, 0, 1024, 0, 5);*/

  
  
  
}
