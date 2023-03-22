#include <Wire.h>

#define AX2358_address 0b1001010

int mas_vol = 6;
int a,b;

 

void setup() {
  Wire.begin(); //Join i2c bus (address optional for master)
  Serial.begin(9600); //Start serial monitor
  while (!Serial); //Wait until Serial is ready - Leonardo
  Serial.println("Enter command to control speakers: "); //Invites input to control speakers
  Wire.beginTransmission(AX2358_address);
  Wire.endTransmission();
  delay(300);
  // send clear register code as per documents
  Wire.beginTransmission(AX2358_address);
  Wire.write (0b11000100);
  Wire.endTransmission();
  // Input Selection
  Wire.beginTransmission(AX2358_address);
  Wire.write (0b11001011);
  Wire.endTransmission();

}

void process_command(char command) {
  //Basic command
  if (command == '-') {
    if (mas_vol < 7 ) {
      mas_vol++;
      set_vol();
    }
  }
  else if (command == '+') {
    if (mas_vol > 0) {
      mas_vol--;
      set_vol();
    }
  }
}

void loop() {
  if (Serial.available() > 0) { //Tells Arduino to look out for serial data
    char command = Serial.read(); //Stores serial data in the command holder
    process_command(command);
  }
}

void set_vol() {
  int c = mas_vol;
  a = c / 10;
  b = c % 10;
  AX2358_vol(0b11100000 + a, 0b11010000 + b);  // CH1
}

void AX2358_vol(int x, int y){
Wire.beginTransmission(0x4a); // transmit to AX2358 chip
Wire.write (x);
Wire.write (y);
Wire.endTransmission();
}
