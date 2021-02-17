/*  Code For E-GoKart Senior Design Project for Colorado State University 
 *   Written by: Ian Johnson
 *   This will be the code which senses pressure from the analog transducer.
 *   The requirements are one analog input pin.
 * 
 */

const int pressurePIN = A0;               // Analog input pin for the pressure transducer

void setup() {
  Serial.begin(9600);                     // Initializes serial communication
}

void loop() {
  static float pressure = 0;
  pressure = analogRead(pressurePIN);
  pressure = (pressure-98)/828*500;
  // The ADC value 98 is the lowest measurement point, the ADC value 926 is the maximum measurement, 828 is the range of ADC values
  // The ADC measurement is offset from the minimum measurement, divided by the range of allowed values, and multiplied by the measureable range of pressure
  Serial.print(pressure, 1);                // Formated with one decimal point
  Serial.println(" psi");
  delay(250);                               //  Delay in milliseconds between read values
}
