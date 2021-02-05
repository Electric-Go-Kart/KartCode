/*  Code For E-GoKart Senior Design Project for Colorado State University 
 *   Written by: Ian Johnson
 *   This will be the code which senses distance using LiDar.
 *   The requirements are two UART Tx and Rx ports.
 * 
 */

#include <TFMini.h>
//NOTE: This library is not the one available through the IDE

TFMini Sensor1;
TFMini Sensor2;
//Declaraing the Sensors (Required for the library)

void setup() {
  Serial.begin(115200);
  //Serial for Monitor
  
  while (!Serial);
  Serial.println ("Initializing...");

  Serial1.begin(115200);
  Serial2.begin(115200);
  //Serials for the Sensors
  
  Sensor1.begin(&Serial1);    
  Sensor2.begin(&Serial2); 
  //Initialize Sensors with Serial
  
  delay(100);
  //Let the Sensors Stabliize
  
  Sensor1.setSingleScanMode();
  Sensor2.setSingleScanMode();
  //Force each sensor to only read when triggered
}


void loop() {
  Sensor1.externalTrigger();  
  uint16_t dist1 = Sensor1.getDistance();
  //Trigger and Read one distance
  
  Sensor2.externalTrigger();
  uint16_t dist2 = Sensor2.getDistance();
  //Trigger and Read another distance

  Serial.print("Dist1 ");
  Serial.print(dist1);
  Serial.println(" cm");
  Serial.print("Dist2 ");
  Serial.print(dist2);
  Serial.println(" cm");
  //Display the measurement

  delay(25);  
  //Wait some short time
  //Longer waits cause bufferring in the UART, will continue to work on this
}
