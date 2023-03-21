#include <Wire.h>

#define AX2358_address 0b1001010

int mas_vol = 0;
int a,b;

 

void setup() {
  Wire.begin(); //Join i2c bus (address optional for master)
  Serial.begin(9600); //Start serial monitor
  while (!Serial); //Wait until Serial is ready - Leonardo
  Serial.println("Enter command to control speakers: "); //Invites input to control speakers
  Wire.beginTransmission(AX2358_address);
    int error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("Code sent successfully");
  } else {
    Serial.print("Error sending code: ");
    Serial.println(error);
  }
  delay(300);
  // send clear register code as per documents
  Wire.beginTransmission(AX2358_address);
  Wire.write (0b11000100);
  error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("Clear register Sucessfull");
  } else {
    Serial.print("Error sending Clear register: ");
    Serial.println(error);
  }
  // Input Selection
  Wire.beginTransmission(AX2358_address);
  Wire.write (0b11001011);
  error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("Input: 1 Selected");
  } else {
    Serial.print("Error selecting Input: 1 ");
    Serial.println(error);
  }
}

void loop() {
  int command = 0; //Creates a holder for incoming serial data

  if (Serial.available() > 0) { //Tells Arduino to look out for serial data
    command = Serial.read(); //Stores serial data in the command holder
    
    //Basic command
    if (command == '+') {
		  mas_vol++;
      set_vol();
     }else if (command == '-'){
		  mas_vol--;
      set_vol();
	  }
  }
  
}		
void set_vol() {
  if (mas_vol > 79) {
    mas_vol = 79;
  }
  if (mas_vol < -10) {
    mas_vol = -10;
  }

  int c = 79 - mas_vol;
  a = c / 10;
  b = c - a * 10;
  
  AX2358_vol(0b11100000 + a, 0b11010000 + b);  // CH1

}

void AX2358_vol(int x, int y){
Wire.beginTransmission(0x4a); // transmit to AX2358 chip
Wire.write (x);
Wire.write (y);
  Serial.println(x);
  Serial.println(y);
  Serial.println(mas_vol);
  int error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("volume change");
  } else {
    Serial.print("Error volume change ");
    Serial.println(error);
  }
}
