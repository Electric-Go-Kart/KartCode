/*   Code by: Deagan Malloy
 *   Code to run multiple LCD's off of the two analog pins A4 and A5
 *   A4: Goes to the SDA pin on the backpack of the LCD's
 *   A5: Goes to the SCL pin on the backpack of the LCD's
 *   
 *   Each LCD is addressed by shorting the different spots on the backpack
 *    Can run up to 8 of these LCD's off of I2C
 */
#include<Wire.h>
#include<LCD.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd1(0x27, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C lcd2(0x26, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C lcd3(0x25, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C lcd4(0x24, 2, 1, 0, 4, 5, 6, 7);

uint8_t circle[8]   = {0xe ,0x11,0x11,0x11,0x11,0x11,0xe }; // seems that you cannot use 0 as one of the createChar because it thinks it is passing in an int
uint8_t heart[8]    = {0x0 ,0xa, 0x1f,0x1f,0xe, 0x4, 0x0 };
uint8_t what[8]     = {0x1f,0x11,0xa, 0x11,0xa, 0x11,0x1f};
uint8_t topLeft[8]  = {0x0, 0x0, 0x7, 0x8, 0x10,0x13,0x13,0x10};
uint8_t topRight[8] = {0x0, 0x0, 0x1c,0x2, 0x1, 0x19,0x19,0x1 };
uint8_t botLeft[8]  = {0x10,0x10,0x10,0x10,0x8, 0x7, 0x0, 0x0 }; 
uint8_t botRight[8] = {0x1, 0x1, 0x1, 0x1, 0x2, 0x1c,0x0, 0x0 }; 

void setup(){
  lcd1.begin(16, 2);
  lcd3.begin(16, 2);
  lcd1.setBacklightPin(3,POSITIVE);
  lcd3.setBacklightPin(3,POSITIVE);
  lcd1.setBacklight(HIGH);
  lcd3.setBacklight(HIGH);
  lcd1.backlight();
  lcd3.backlight();

  lcd1.createChar(7, circle);
  lcd1.createChar(1, heart);
  lcd1.createChar(2, what);
  lcd1.createChar(3, topLeft);
  lcd1.createChar(4, topRight);
  lcd1.createChar(5, botLeft);
  lcd1.createChar(6, botRight);

}
void loop(){
  lcd1.clear();
  lcd1.home();
  lcd1.write(7);
  lcd1.write(1);
  lcd1.setCursor(0,1);
  lcd1.write(2);
  lcd1.write(1);
  
  lcd3.clear();
  lcd3.setCursor(0,0);
  lcd3.print("Coding is");
  lcd3.setCursor(0,1);
  lcd3.print("Kind of Cool");
  
  delay(5000);
}
