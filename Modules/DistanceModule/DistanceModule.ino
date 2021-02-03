/*  Code For E-GoKart Senior Design Project for Colorado State University 
 *   Written by: Ian Johnson
 *   This will be the code which senses distance using LiDar.
 *   The requirements are two UART Tx and Rx ports.
 * 
 */
#include <SoftwareSerial.h>
#include <TFMini.h>
//UART Protocol
//Not all pins on the Mega and Mega 2560 support change interrupts,
//so only the following can be used for RX:
//10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

//Uno
SoftwareSerial portOne(8,9); //Rx,Tx
SoftwareSerial portOne(6,7); //Rx,Tx

/*Mega
SoftwareSerial portOne(53,54); //Rx,Tx
SoftwareSerial portOne(62,63); //Rx,Tx 
*/

TFMini Sensor1;
TFMini Sensor2;

void setup() {
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);
     
  Serial.println ("Initializing...");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  portOne.begin(9600);
  portTwo.begin(9600);

  // Step 3: Initialize the TF Mini sensor
  Sensor1.begin(&portOne);    
  Sensor2.begin(&portTwo); 
}


void loop() {
  // Take one TF Mini distance measurement
  uint16_t dist1 = Sensor1.getDistance();
  uint16_t strength1 = Sensor1.getRecentSignalStrength();
  uint16_t dist2 = Sensor2.getDistance();
  uint16_t strength2 = Sensor2.getRecentSignalStrength();

  // Display the measurement
  Serial.print(dist1);
  Serial.print(" cm      sigstr: ");
  Serial.print(strength1);
  Serial.print("           ");
  Serial.print(dist2);
  Serial.print(" cm      sigstr: ");
  Serial.println(strength2);

  // Wait some short time before taking the next measurement
  delay(250);  
}
