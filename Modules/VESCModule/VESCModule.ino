/*  Code By: Deagan Malloy
 *  Using VescUart library by SolidGeek on GitHub
 *  Pins:
 *    Mega TX -> VESC RX
 *    Mega RX -> VESC TX 
 *    
 *    With this library, we are able to:
 *      Set the current to the motor for const current
 *      Able to adjust the speed of motor with the esc.nunchuck commands
 *        esc.nunchuck.lowerButton -> used for cruise control
 *        esc.nunchuck.upperButton -> used for ???
 *        esc.nunchuck.valueY      -> used for the speed (between 0-255 with 127 being nuetral)
 *        esc.nunchuck.valueX      -> used for ???
 *        esc.setNunchuckValues()  -> used to update all the set values
 *        
 *        esc.setRPM(float rpm)   - Set desired RPM
 *        esc.setDuty(float duty) - float between 0.0-1.0
 *        esc.setCurrent(float)   - float between 0.0 and 
 */

#include<VescUart.h>

VescUart esc;

void setup() {
  Serial.begin(9600);     // baud rate for Serial Monitor for debugging
  Serial1.begin(115200);  // baud rate for the VESC

  while(!Serial) {;}      // while Serial Monitor not open - stall

  esc.setDebugPort(&Serial);    // attach the VESC to the Debug Port for printing off values
  esc.setSerialPort(&Serial1);  // attach the VESC to the Serial Port
}

void loop() {
  if(esc.getVescValues()){  // If succesfully returned, print the data
    /*
    Serial.print("RPM: ");
    Serial.println(esc.data.rpm);
    Serial.print("InV: ");
    Serial.println(esc.data.inpVoltage);
    Serial.print("Amps: ");
    Serial.println(esc.data.ampHours);
    Serial.print("");
    Serial.println(esc.data.tachometerAbs);
    */
    // or can print off all data values
    // will only work if debug port has been initialised
    esc.printVescValues();   
    
  } else {                  // else could not get data
    Serial.println("Failed to get Data.");
  }

  delay(500);
}
