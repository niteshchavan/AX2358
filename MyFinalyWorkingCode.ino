#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

//I2C control codes
int ch2 = 0xcb; // 2.0 Input
int ch6 = 0xcf; //5.1 Input
int sur_on = 0xC0; // Surround On  oldcode 0xd0;
int sur_off = 0xC1;  //Surround off  oldcode 0xd1;
int mix_on = 0xC2; // 6 chan mix 0db
int mix_off = 0xC3; // 6 chan mix 6db
int mute = 0xff; // {0xf1, 0xf3, 0xf9, 0xfb, 0xf5, 0xf7, }; //Creates array for muting speakers: FL, FR, RL, RR, CEN, SW, All
int unmute = 0xfe; //{0xf0, 0xf2, 0xf8, 0xfa, 0xf4, 0xf6, , 0xc4}; //Creates array for unmuting speakers: FL, FR, RL, RR, CEN, SW, All, CLEAR ALL VOLUME CHANGES
int vol10 = 0xd1; // {0x81, 0x41, 0x1, 0x21, 0x61, 0xa1, }; //Creates array for 10db stage volume control: FL, FR, Rl, RR, CEN, SW, ALL
int vol1 = 0xe1; // {0x91, 0x51, 0x11, 0x31, 0x71, 0xb1, }; //Creates array for 1db stage volume control: FL, FR, Rl, RR, CEN, SW, ALL


//---------------------------------------------------------------
//HTML webpage contents in program memory
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
<center>
<h1>WIFI Sound Control 6CH</h1><br>
Ciclk to turn on <a href="poweron">Powre ON</a><br>
Ciclk to turn off <a href="muteall">Mute</a><br>
Ciclk to turn off <a href="unmuteall">Unmute</a><br>
Ciclk to turn <a href="suron">Surround On</a><br>
Ciclk to turn <a href="suroff">Surround Off</a><br>
Ciclk to turn <a href="mixon">Mix On</a><br>
Ciclk to turn <a href="mixoff">Mix Off</a><br>
<hr>
</center>

</body>
</html>
)=====";
//---------------------------------------------------------------

//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 Serial.println("You called root page");
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void poweron() { 
 Serial.println("Poweron");
      int x = ch2;
      int y = vol1;
      int z = vol10;
	    start(x,y,z);
 server.send(200, "text/html", "Power On"); //Send ADC value only to client ajax request
}

void muteall() { 
 Serial.println("Mute all");
      int x = mute;
	    sendcode(x);
 server.send(200, "text/html", "Mute All"); //Send ADC value only to client ajax request
}

void unmuteall() { 
 Serial.println("Un Mute all");
      int x = unmute;
	    sendcode(x);
 server.send(200, "text/html", "Unmute All"); //Send ADC value only to client ajax request
}

void suron() { 
 Serial.println("surround on");
      int x = sur_on;
	    sendcode(x);
 server.send(200, "text/html", "Surround on"); //Send ADC value only to client ajax request
}

void suroff() { 
 Serial.println("surround off");
      int x = sur_off;
	    sendcode(x);
 server.send(200, "text/html", "Surround off"); //Send ADC value only to client ajax request
}

void mixon() { 
 Serial.println("Mix On");
      int x = mix_on;
	    sendcode(x);
 server.send(200, "text/html", "Mix On"); //Send ADC value only to client ajax request
}

void mixoff() { 
 Serial.println("Mix Off");
      int x = mix_off;
	    sendcode(x);
 server.send(200, "text/html", "Mix Off"); //Send ADC value only to client ajax request
}


void setup() {
  Serial.begin(9600); //Start serial monitor
  while (!Serial); //Wait until Serial is ready - Leonardo

  WiFi.begin("Atmos", "root@123");     //Connect to your WiFi router
  Serial.println("");

    // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/poweron", poweron);
  server.on("/muteall", muteall);
  server.on("/unmuteall", unmuteall);
  server.on("/suron", suron);
  server.on("/suroff", suroff);
  server.on("/mixon", mixon);
  server.on("/mixoff", mixoff);

  server.begin(); 
  Serial.println("HTTP server started");
 
  Wire.begin(); //Join i2c bus (address optional for master)
  //Serial.println("Enter command to control speakers: "); //Invites input to control speakers
}

void loop() {
  server.handleClient();
//  int command = 0; //Creates a holder for incoming serial data
//  
//  if (Serial.available() > 0) { 
//    
//    command = Serial.read(); //Stores serial data in the command holder
//   
//    if (command == 's') {
//      int x = ch2;
//      int y = vol1;
//      int z = vol10;
//	    start(x,y,z);
//      Serial.println("Start");
//    }
//
//  }
}

void sendcode(int x){ //Creates function to transmit given code to speakers
Wire.beginTransmission(0x4a); // transmit to AX2358 chip
Wire.write (x);
Wire.endTransmission();    // stop transmitting
}

void start(int x, int y, int z){ //Creates function to transmit given code to speakers
Wire.beginTransmission(0x4a); // transmit to AX2358 chip
Wire.write (x);
Wire.write (y);
Wire.write (z);
Wire.write (y);
Wire.endTransmission();    // stop transmitting
}