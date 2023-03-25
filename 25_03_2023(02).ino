#include <Wire.h>
#define AX2358_address 0b1001010

// Rotary Encoder Inputs
#define CLK 12
#define DT 14
#define SW 13

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

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinsetup();
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
}

void pinsetup(){
  // Set encoder pins as inputs
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
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
		else {
			// Encoder is rotating CW so increment
			counter ++;
			currentDir ="CW";
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
    delay(100); // debounce delay to prevent multiple button presses
  }


}

void send_vol_data(){
  set_vol(array1[currentRecord], array2[array2Index]);
  // check if we've printed all the records
  if(currentRecord == 9 && array2Index == 9) {
    Serial.println("Volume Full");
  }
  // wait for a short period before checking the buttons again
  delay(100);
}


void set_vol(int x, int y) {
  Wire.beginTransmission(AX2358_address);
  Wire.write (x);
  Wire.write (y);
  Serial.println(x, BIN);
  Serial.println(y, BIN);
  Wire.endTransmission();
}

void loop() {
  serialhandel();
  encoder();
}


