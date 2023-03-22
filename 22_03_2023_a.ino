#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define AX2358_address 0b1001010

int mas_vol = 6;
int a,b;

//---------------------------------------------------------------
//HTML webpage contents in program memory
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
  <style>
    .button {
      background-color: #000000;
	  width: 100%;
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
<button class="button" onclick="sendRequest('volup')">Volup</button> <br><br><br>
<button class="button" onclick="sendRequest('voldown')">Voldown</button><br><br><br>
<button class="button" onclick="sendRequest('unmuteall')">Unmute</button><br><br><br>
<button class="button" onclick="sendRequest('suron')">Surround On</button><br><br><br>
<button class="button" onclick="sendRequest('suroff')">Surround Off</button><br><br><br>
<button class="button" onclick="sendRequest('mixon')">Mix On</button><br><br><br>
<button class="button" onclick="sendRequest('mixoff')">Mix Off</button><br><br><br>
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
  server.on("/volup", volup);
  server.on("/voldown", voldown);
  server.begin(); 
  Serial.println("HTTP server started");
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

void volup() {
  //Basic command
    if (mas_vol > 0) {
      mas_vol--;
      set_vol();
    }
server.send(200, "text/html", "volup");
}

void voldown() {
  //Basic command

    if (mas_vol < 7 ) {
      mas_vol++;
      set_vol();
    
  }
server.send(200, "text/html", "down");
}

void loop() {
  server.handleClient();
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
