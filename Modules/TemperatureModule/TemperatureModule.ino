/*  Code For E-GoKart Senior Design Project for Colorado State University 
 *   Written by: Ian Johnson
 *   This will be the code that senses temperature using K type thermocouples and digitizer boards.
 *   The requirements are one SPI bus, one internal clock output, and two Chip Select ouputs.
 * 
 */
#include <SparkFunMAX31855k.h> // Using the max31855k driver
#include <SPI.h>  // Included here too due Arduino IDE; Used in above header

// Define SPI Arduino pin numbers 
// UNO
const uint8_t CS1 = 10; // Using standard CS line (SS)
const uint8_t CS2 = 9;
// MEGA
//const uint8_t CS1 = 20;
//const uint8_t CS2 = 19;
// To add more probes, the Chip Select pin needs to be set to another pin, and then only set high, when all other pins are set low. 
// SCK (13) & MISO (12) are defined by Arduino for UNO
// SCK (52) & MISO (50) are defined by Arduino for MEGA


// Instantiate an instance of the SparkFunMAX31855k class
SparkFunMAX31855k probe1(CS1);
SparkFunMAX31855k probe2(CS2);

void setup() {
  Serial.begin(9600);
  Serial.println("\nBeginning...");
  delay(50);  // Let IC stabilize or first readings will be garbage
}

void loop() {
  float ambient1 = probe1.readCJT();
  float ambient2 = probe2.readCJT();
  
  float temp1 = probe1.readTempC();
  delay(150);
  float temp2 = probe2.readTempC();
  delay(150);
  //float temp3 = probe3.readTempC();
  delay(150);
  //float temp4 = probe4.readTempC();
  delay(150);
  //float temp5 = probe5.readTempC();
  delay(150);

  float temperature = (temp1 + temp2) / 5;

  // Read methods return NAN if they don't have a valid value to return.
  // The following conditionals only print the value if it's not NAN.
  if (!isnan(temperature)) {
    Serial.print("CJT 1 is (˚C): ");
    Serial.println(ambient1);
    Serial.print("CJT 2 is (˚C): ");
    Serial.println(ambient2);
    Serial.print("Probe is (˚C): ");
    Serial.println(temperature);
    if (temperature > 70) {
      // THIS IS WHERE ANY INTERUPTS WOULD NEED TO GO
      Serial.println("Too Hot");
    }
  }
  else {
    Serial.println("Error");
  }
}
