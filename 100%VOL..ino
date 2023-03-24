#include <Wire.h>

#define AX2358_address 0b1001010

int array1[] = {
  0b11011001,
  0b11011000,
  0b11010111,
  0b11010110,
  0b11010101,
  0b11010100,
  0b11010011,
  0b11010010,
  0b11010001,
  0b11010000
};
int array2[] = {
  0b11101001,
  0b11101000,
  0b11100111,
  0b11100110,
  0b11100101,
  0b11100100,
  0b11100011,
  0b11100010,
  0b11100001,
  0b11100000
};

int currentRecord = 0; // the index of the current record
int array2Index = 0; // the index of the current value from array2

void setup() {
  Wire.begin();
  poweron(); // Start
  // initialize serial   communication
  Serial.begin(9600);
  while (!Serial);

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

void loop() {
  serialhandel();

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
      print();
    }
    else {
      array2Index = 0;
      if(currentRecord < 9) {
        currentRecord++;
        print();
      }
    }
    delay(100); // debounce delay to prevent multiple button presses
  }
  
  // check if the "-" button is pressed and decrement the current record if it is
  if(command == '-') { // "-" button is pressed
    if(array2Index > 0) {
      array2Index--;
      print();
    }
    else {
      array2Index = 9;
      if(currentRecord > 0) {
        currentRecord--;
        print();
      }
    }
    delay(100); // debounce delay to prevent multiple button presses
  }


}

void print(){
  // print the current record from array1 with the current value from array2
  Serial.print("Record ");
  Serial.print(currentRecord);
  Serial.print(": ");
  Serial.print(array1[currentRecord], BIN);
  Serial.print(" with array2 value: ");
  Serial.print(array2[array2Index], BIN);
  Serial.println();
  set_vol(array1[currentRecord], array2[array2Index]);
  // check if we've printed all the records
  if(currentRecord == 9 && array2Index == 9) {
    Serial.println("All records printed!");
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
