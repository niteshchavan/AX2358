#include <Wire.h>

#define AX2358_address 0b1001010

int gain = 0b11010000;
int level = 0b11100000; 

void setup() {
  Wire.begin();
  Wire.beginTransmission(AX2358_address);
  Wire.endTransmission();
  delay(1000);

  // send clear register code as per documents
  Wire.beginTransmission(AX2358_address);
  Wire.write (0b11000100);
  Wire.endTransmission();
  delay(300);

  // Input Selection
  Wire.beginTransmission(AX2358_address);
  Wire.write (0b11001011);
  Wire.endTransmission();
  delay(300);

  // Transmit Audio to AX2358 chip
  Wire.beginTransmission(0x4a); 
  Wire.write(0b11000100);
  Wire.write (gain);
  Wire.write (level);
  Wire.endTransmission();
}

void loop(){}
