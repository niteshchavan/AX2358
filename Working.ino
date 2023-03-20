#include <Wire.h>


#define AX2358_address 0b1001010


int a = 0b11100000; // 0/0
int b = 0b11100001; //- 1/-10
int c = 0b11100010; //- 2/-20
int d = 0b11100011; //- 3/-30
int e = 0b11100100; //- 4/-40
int f = 0b11100101; //-5/-50
int g = 0b11100110; //-6/-60
int h = 0b11100111; //-7/-70
int i = 0b11101000; //- 8/--
int j = 0b11101001; //- 9/--
int y = 0b11010100;

void setup() {
  
  Serial.begin(9600); //Start serial monitor
  while (!Serial); //Wait until Serial is ready - Leonardo
  Wire.begin();
  // Check if device is active 
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
  // Volume Adjust
 //   Wire.beginTransmission(AX2358_address);
 // Wire.write (0b11100010);
//  Wire.write (0b11010100);
 // error = Wire.endTransmission();
 // if (error == 0) {
//    Serial.println("-40db and -2db");
//  } else {
 //   Serial.print("Error -40db and -2db ");
 //   Serial.println(error);
 // }
}

   // case 0: a = 0b11001011; break; // 1 input
   // case 1: a = 0b11001010; break; // 2 input
   // case 2: a = 0b11001001; break; // 3 input
   // case 3: a = 0b11001000; break; // 4 input
  //  case 4: a = 0b11001111; break; // 6 CH input

void loop() {
  int command = 0; //Creates a holder for incoming serial data
  
  if (Serial.available() > 0) { 
    
   command = Serial.read(); //Stores serial data in the command holder
  
   if (command == 'a') {
     int p = a;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
    else if(command == 'b'){
     int p = b;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
    else if(command == 'c'){
     int p = c;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
    else if(command == 'd'){
     int p = d;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
    else if(command == 'e'){
     int p = e;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
        else if(command == 'f'){
     int p = f;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
    else if(command == 'g'){
     int p = g;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
        else if(command == 'h'){
     int p = h;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }

    else if(command == 'i'){
     int p = i;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
        else if(command == 'j'){
     int p = j;
     int q = y;
	    sendvol(p,q);
      Serial.println("volsent");
    }
  }


}


void sendvol(int p, int q){ //Creates function to transmit given code to speakers
Wire.beginTransmission(0x4a); // transmit to AX2358 chip
Wire.write(0b11000100);
Wire.write (p);
Wire.write (q);

  int error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("volume change");
  } else {
    Serial.print("Error volume change ");
    Serial.println(error);
  }
}
