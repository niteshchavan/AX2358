

int pin1 = 16;   //0
int pin2 = 5;    //0
int pin3 = 4;    //0
int pin4 = 0;    //1
int pin5 = 2;    //1
int pin6 = 14;   //1
int pin7 = 12;   //1
int pin8 = 13;   //1
int pin9 = 15;  // 0

int reading;
int previous = LOW;
int state = HIGH;

unsigned long debounce = 200UL;


void setup() {
  Serial.begin(9600);
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5, INPUT);
  pinMode(pin6, INPUT);
  pinMode(pin7, INPUT);
  pinMode(pin8, INPUT);
  pinMode(pin9, INPUT);

  //reading = digitalRead(pin);
  Serial.println(digitalRead(pin1));
  Serial.println(digitalRead(pin2));
  Serial.println(digitalRead(pin3));
  Serial.println(digitalRead(pin4));
  Serial.println(digitalRead(pin5));
  Serial.println(digitalRead(pin6));
  Serial.println(digitalRead(pin7));
  Serial.println(digitalRead(pin8));
  Serial.println(digitalRead(pin9));
  //if (reading == HIGH ){
  //  Serial.println("Button on");
  //}

}

void loop() {

}
