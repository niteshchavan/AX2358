#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define AX2358_address 0b1001010
#define Surr_On 0b11000000
#define Surr_Off 0b11000001
#define Mix_On 0b11000010
#define Mix_Off 0b11000011
#define Mute_all 0b11111111
#define UnMute_all 0b11111110

// Rotary Encoder Inputs
#define CLK 12 //D6
#define DT 14 //D5
#define SW 13 // D7

//Volume Control from -70dB to 0db
int array1[] = {0b11011001, 0b11011000, 0b11010111, 0b11010110, 0b11010101, 0b11010100, 0b11010011, 0b11010010, 0b11010001, 0b11010000};
int array2[] = {0b11101001, 0b11101000, 0b11100111, 0b11100110, 0b11100101, 0b11100100, 0b11100011, 0b11100010, 0b11100001, 0b11100000};

int currentRecord = 0; // the index of the current record
int array2Index = 0; // the index of the current value from array2

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;


//---------------------------------------------------------------
//HTML webpage contents in program memory
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
  <style>
    .button {
      background-color: #000000;
	  width: 50%;
      color: white;
      padding: 14px 28px;
      font-size: 16px;
      cursor: pointer;
      text-align: center;
      border-radius: 15%;
      position: absolute;
    }
  </style>
<h1>WIFI Sound Control 6CH</h1>
<hr>
<button class="button" onclick="sendRequest('poweron')">poweron</button><br><br><br>
<button class="button" onclick="sendRequest('volup')">Volup</button> <br><br><br>
<button class="button" onclick="sendRequest('voldown')">Voldown</button><br><br><br>
<button class="button" onclick="sendRequest('SurrOn')">Surround On</button><br><br><br>
<button class="button" onclick="sendRequest('SurrOff')">Surround Off</button><br><br><br>
<button class="button" onclick="sendRequest('MixOn')">Mix On</button><br><br><br>
<button class="button" onclick="sendRequest('MixOff')">Mix Off</button><br><br><br>
<button class="button" onclick="sendRequest('Mute')">Mute</button><br><br><br>
<button class="button" onclick="sendRequest('UnMute')">Un Mute</button><br><br><br>
<hr>
<script>
function sendRequest(action) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      // Update the content of a div element with the response from the server
      document.getElementById("response").innerHTML = this.responseText
    }
  };
  xhttp.open("GET", action, true);
  xhttp.send();
}
</script>
<div id="response"></div>
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

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinsetup();
  wifi(); //Star Wifi
  webserver(); // Start Server at port 80
}

void webserver(){
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/poweron", poweron);
  server.on("/volup", volup);
  server.on("/voldown", voldown);
  server.on("/SurrOn", SurrOn);
  server.on("/SurrOff", SurrOff);
  server.on("/MixOn", MixOn);
  server.on("/MixOff", MixOff);
  server.on("/Mute", Mute);
  server.on("/UnMute", UnMute);
  server.begin(); 
  Serial.println("HTTP server started at Port 80");
}

void wifi(){
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
}


void poweron(){
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
  server.send(200, "text/html", "Power On");
}

void pinsetup(){
  // Set encoder pins as inputs
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
}

void SurrOn() {
AX2358(Surr_On);
server.send(200, "text/html", "Surr On");
}

void SurrOff() {
AX2358(Surr_Off);
server.send(200, "text/html", "Surr Off");
}

void MixOn() {
AX2358(Mix_On);
server.send(200, "text/html", "Mix On");
}

void MixOff() {
AX2358(Mix_Off);
server.send(200, "text/html", "Mix Off");
}

void Mute() {
AX2358(Mute_all);
server.send(200, "text/html", "Mute All");
}

void UnMute() {
AX2358(UnMute_all);
server.send(200, "text/html", "Unmute All");
}


void encoder() {
	// Read the current state of CLK
	currentStateCLK = digitalRead(CLK);
	// If last and current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
		// If the DT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(DT) != currentStateCLK) {
			counter --;
			currentDir ="CCW";
				if(array2Index < 9) {
				array2Index++;
				send_vol_data();
				}
				else {
					array2Index = 0;
					if(currentRecord < 9) {
						currentRecord++;
						send_vol_data();
						}
				}
		} 
		else {
			// Encoder is rotating CW so increment
			counter ++;
			currentDir ="CW";
      			if(array2Index > 0) {
				array2Index--;
				send_vol_data();
			}
			else {
				array2Index = 9;
					if(currentRecord > 0) {
						currentRecord--;
						send_vol_data();
						}
				}

		}

	}
	// Remember last CLK state
	lastStateCLK = currentStateCLK;
	// Read the button state
	int btnState = digitalRead(SW);
	//If we detect LOW signal, button is pressed
	if (btnState == LOW) {
		//if 50ms have passed since last LOW pulse, it means that the
		//button has been pressed, released and pressed again
		if (millis() - lastButtonPress > 50) {
      poweron();
			Serial.println("Power On");
		}
		// Remember last button press event
		lastButtonPress = millis();
	}
	// Put in a slight delay to help debounce the reading
	delay(1);
}

void serialhandel(){
  if (Serial.available() > 0) { //Tells Arduino to look out for serial data
    char command = Serial.read(); //Stores serial data in the command holder
    process_command(command);
  }
}

void process_command(char command) {
  // check if the "+" button is pressed and increment the current record if it is
  if(command == '+') { // "+" button is pressed
    if(array2Index < 9) {
      array2Index++;
      send_vol_data();
    }
    else {
      array2Index = 0;
      if(currentRecord < 9) {
        currentRecord++;
        send_vol_data();
      }
    }
    delay(100); // debounce delay to prevent multiple button presses
  }
  
  // check if the "-" button is pressed and decrement the current record if it is
  if(command == '-') { // "-" button is pressed
    if(array2Index > 0) {
      array2Index--;
      send_vol_data();
    }
    else {
      array2Index = 9;
      if(currentRecord > 0) {
        currentRecord--;
        send_vol_data();
      }
    }
    //delay(100); // debounce delay to prevent multiple button presses
  }


}

void voldown() {
    if(array2Index > 0) {
      array2Index--;
      send_vol_data();
    }
    else {
      array2Index = 9;
      if(currentRecord > 0) {
        currentRecord--;
        send_vol_data();
      }
    }
	server.send(200, "text/html", "voldown");
}

void volup() {
	if(array2Index < 9) {
      array2Index++;
      send_vol_data();
    }
    else {
      array2Index = 0;
      if(currentRecord < 9) {
        currentRecord++;
        send_vol_data();
      }
    }
	server.send(200, "text/html", "volup");
}

void send_vol_data(){
  set_vol(array1[currentRecord], array2[array2Index]);
  // check if we've printed all the records
  if(currentRecord == 9 && array2Index == 9) {
    Serial.println("Volume Full");
    server.send(200, "text/html", "Volume Full");
  }
  // wait for a short period before checking the buttons again
  //delay(100);
}


void set_vol(int x, int y) {
  Wire.beginTransmission(AX2358_address);
  Wire.write (x);
  Wire.write (y);
  Serial.println(x);
  Serial.println(y);
  Wire.endTransmission();
}

void AX2358(int x){
  Wire.beginTransmission(AX2358_address); // transmit to AX2358 chip
  Wire.write (x);
  Wire.endTransmission();
}

void loop() {
  server.handleClient();
  serialhandel();
  encoder();
}


