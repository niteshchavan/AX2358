#include <Wire.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define AX2358_address 0b1001010
#define Surr_On 0b11000000
#define Surr_Off 0b11000001
#define Mix_On 0b11000010
#define Mix_Off 0b11000011
#define Mute_all 0b11111111
#define UnMute_all 0b11111110


// Rotary Encoder Inputs
#define CLK 5 
#define DT 6
#define SW 7 



int array1[] = {0b11010111, 0b11010110, 0b11010101, 0b11010100, 0b11010011, 0b11010010, 0b11010001, 0b11010000};
            //  215         214         213          212        211          210        209         208
int array2[] = {0b11101001, 0b11101000, 0b11100111, 0b11100110, 0b11100101, 0b11100100, 0b11100011, 0b11100010, 0b11100001, 0b11100000};
            //  233       232       231         230         229         228         227         226         225          224

int currentRecord = 0; // the index of the current record
int array2Index = 0; // the index of the current value from array2

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

int pin1 = 2; // input pin1 surr on off
int pin2 = 3; // input pin2 mix on off
int pin3 = 9;
int pin4 = 10;
int pin5 = 11;
int pin6 = 12;

int reading1;           // the current reading from pin1
int reading2;           // the current reading from pin2

int previous1 = LOW;    // the previous reading from the input pin
int previous2 = LOW;    // the previous reading from the input pin


int state = HIGH;      // the current state of the output pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long time = 0;           // the last time the output pin was toggled
unsigned long debounce = 200UL;   // the debounce time, increase if the output flickers


void setup() {
  Serial.begin(9600);
  pinsetup();
  Wire.begin();
  lcd.init();
  lcd.backlight();
  
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
  pinMode(pin1, INPUT_PULLUP);  // for Surr on off
  pinMode(pin2, INPUT_PULLUP);  // for mix on off
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(pin5, OUTPUT);
  pinMode(pin6, OUTPUT);
  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
  
}

void SurrOn() {
AX2358(Surr_On);
}

void SurrOff() {
AX2358(Surr_Off);
}

void MixOn() {
AX2358(Mix_On);
}

void MixOff() {
AX2358(Mix_Off);
}

void Mute() {
AX2358(Mute_all);
}

void UnMute() {
AX2358(UnMute_all);
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
      if (counter < 79){
			counter ++;
      Serial.print(counter);
			currentDir ="CW";
        if(array2Index < 9) {
				array2Index++;
				
				}
				else {
					array2Index = 0;
					if(currentRecord < 9) {
						currentRecord++;
						
						}
				}
        send_vol_data();
      }
		} 
		else {
			// Encoder is rotating CW so increment
      if (counter > 0){
			counter --;
      Serial.print(counter);
			currentDir ="CCW";
      if(array2Index > 0) {
				array2Index--;
				
			}
			else {
				array2Index = 9;
					if(currentRecord > 0) {
						currentRecord--;
						
						}
				}
        send_vol_data();
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
      lcd.setCursor(0, 0);
      lcd.print("Power On");
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
      }
      else {
        array2Index = 9;
      if(currentRecord > 0) {
        currentRecord--;
      }
    }
    send_vol_data();
	
}

void volup() {
	  if(array2Index < 9) {
      array2Index++;

    }
    else {
      array2Index = 0;
      if(currentRecord < 9) {
        currentRecord++;

      }
    }
    send_vol_data();
	
}

void send_vol_data(){
  set_vol(array1[currentRecord], array2[array2Index]);
  // check if we've printed all the records
  if(currentRecord == 9 && array2Index == 9) {
    Serial.println("Volume Full");
    
  }
  // wait for a short period before checking the buttons again
  //delay(100);
}


void set_vol(int x, int y) {
  Wire.beginTransmission(AX2358_address);
  Wire.write (x);
  Wire.write (y);
  Wire.endTransmission();
  Serial.println(" ");
  lcd.setCursor(0,1);
  lcd.print("Volume: ");
  lcd.print(counter);

}

void AX2358(int x){
  Wire.beginTransmission(AX2358_address); // transmit to AX2358 chip
  Wire.write (x);
  Wire.endTransmission();
}



void buttons(){
  reading1 = digitalRead(pin1);
  reading2 = digitalRead(pin2);

  if (reading1 == HIGH && previous1 == LOW && millis() - time > debounce){
     if (state == HIGH)
      state = LOW;
      
    else
      state = HIGH;

    time = millis();
    printState1();
  }
  if (reading2 == HIGH && previous2 == LOW && millis() - time > debounce){
     if (state == HIGH)
      state = LOW;
      
    else
      state = HIGH;

    time = millis();
    printState2();
  }  
  previous1 = reading1;
  previous2 = reading2;
}

void printState1() {
  if (state == HIGH) {
    Serial.println("Button state: Surr on");
    SurrOn();
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, LOW);
  } else {
    Serial.println("Button state: Surr off");
    SurrOff();
    digitalWrite(pin4, HIGH);
    digitalWrite(pin3, LOW);
  }
}
void printState2() {
  if (state == HIGH) {
    Serial.println("Button state: Mix on");
    MixOn();
    digitalWrite(pin5, HIGH);
    digitalWrite(pin6, LOW);
  } else {
    Serial.println("Button state: Mix off");
    MixOff();
    digitalWrite(pin6, HIGH);
    digitalWrite(pin5, LOW);
  }
}

void loop() {
  buttons();  
  serialhandel();
  encoder();
}

