#include <IRremote.h>
#include <Wire.h>

#define IR_RECEIVE_PIN 2
#define AX2358_address 0b1001010
#define Surr_On 0b11000000
#define Surr_Off 0b11000001
#define Mix_On 0b11000010
#define Mix_Off 0b11000011
#define Mute_all 0b11111111
#define UnMute_all 0b11111110

int a, b, fl, fr, sl, sr, sub, fr_vol, fl_vol, sr_vol, sl_vol,sub_vol, mas_vol;
bool isMuted = false; // Mute status flag

void setup() {
  Serial.begin(9600);
  Wire.begin();
  IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData != 0) {
      // Print the hex value of the received IR signal
      Serial.print("Hex Value: 0x");
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

      // Check the received IR code and perform actions based on it
      switch (IrReceiver.decodedIRData.decodedRawData) {
        case 0xBE41FE01:
          Serial.println("Power On");
          poweron();
          break;
        case 0xBA45FE01:
          Serial.println("FM");
          Wire.beginTransmission(AX2358_address);
          Wire.write (0b11001001);
          Wire.endTransmission();
          break;          
        case 0xBF40FE01:
          Serial.println("PRO");
          Wire.beginTransmission(AX2358_address);
          Wire.write (0b11001011);
          Wire.endTransmission();
          break;
        case 0xBB44FE01:
          Serial.println("AUX");
          Wire.beginTransmission(AX2358_address);
          Wire.write (0b11001010);
          Wire.endTransmission();
          break;
        case 0xB649FE01:
          Serial.println("MAS_UP");
          mas_vol++;
          set_mas_vol();
          set_fl();
          set_fr();
          set_sub();
          set_sl();
          set_sr();
          break;
        case 0xF20DFE01:
          Serial.println("MAS_DN");
          mas_vol--;
          set_mas_vol();
          set_fl();
          set_fr();
          set_sub();
          set_sl();
          set_sr();
          break;
        case 0xFC03FE01:
          Serial.println("FL_DN");
          fl_vol--;
          set_fl();
          break;
        case 0xFD02FE01:
          Serial.println("FL_UP");
          fl_vol++;
          set_fl();
          break;
        case 0xFE01FE01:
          Serial.println("FR_DN");
          fr_vol--;
          set_fr();
          break;
        case 0xFF00FE01:
          Serial.println("FR_UP");
          fr_vol++;
          set_fr();
          break;
        case 0xED12FE01:
          Serial.println("SL_UP");
          sl_vol++;
          set_sl();
          break;
        case 0xEC13FE01:
          Serial.println("SL_DN");
          sl_vol--;
          set_sl();
          break;
        case 0xF50AFE01:
          Serial.println("SUB_UP");
          sub_vol++;
          set_sub();
          break;
        case 0xF906FE01:
          Serial.println("SUB_DN");
          sub_vol--;
          set_sub();
          break;
        case 0xEF10FE01:
          Serial.println("SR_UP");
          sr_vol++;
          set_sr();
          break;
        case 0xEE11FE01:
          Serial.println("SR_DN");
          sr_vol--;
          set_sr();
          break;          
        case 0xE51AFE01:
          Serial.println("SUR_ON");
          SurrOn();
          break;
        case 0xE41BFE01:
          Serial.println("SUR_OFF");
          SurrOff();
          break;
        case 0xE619FE01:
          Serial.println("MIX_ON");
          MixOn();
          break;
        case 0xE718FE01:
          Serial.println("MIX_OFF");
          MixOff();
          break;
        case 0xA758FE01:
          if (isMuted) {
            UnMute();
            isMuted = false;
          } else {
            Mute();
            isMuted = true;
          }
          break;
        default:
          // Handle any other unrecognized IR codes here
          break;
      }
    }
    IrReceiver.resume();
  }
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
void set_fl() {
  if (fl_vol > 10) {
    fl_vol = 10;
  }
  if (fl_vol < -10) {
    fl_vol = -10;
  }
  fl = mas_vol + fl_vol;
  int c = 79 - fl;
  a = c / 10;
  b = c - a * 10;
  Serial.print("FL: ");
  Serial.print(a);
  Serial.print(" ");
  Serial.println(b);
  AX2358_vol(0b10000000 + a, 0b10010000 + b);  // CH1
}

void set_fr() {
  if (fr_vol > 10) {
    fr_vol = 10;
  }
  if (fr_vol < -10) {
    fr_vol = -10;
  }
  fr = mas_vol + fr_vol;
  int c = 79 - fr;
  a = c / 10;
  b = c - a * 10;
  Serial.print("FR: ");
  Serial.print(a);
  Serial.print(" ");
  Serial.println(b);
  AX2358_vol(0b01000000 + a, 0b01010000 + b);  // CH2
}

void set_mas_vol() {
  if (mas_vol > 69) {
    mas_vol = 69;
  }
  if (mas_vol < 19) {
    mas_vol = 19;
  }

}

void set_sub() {
  if (sub_vol > 10) {
    sub_vol = 10;
  }
  if (sub_vol < -10) {
    sub_vol = -10;
  }
  sub = mas_vol + sub_vol;
  int c = 79 - sub;
  a = c / 10;
  b = c - a * 10;
  AX2358_vol(0b00100000 + a, 0b00110000 + b);  // CH4
}
void set_sl() {
  if (sl_vol > 10) {
    sl_vol = 10;
  }
  if (sl_vol < -10) {
    sl_vol = -10;
  }
  sl = mas_vol + sl_vol;
  int c = 79 - sl;
  a = c / 10;
  b = c - a * 10;
  AX2358_vol(0b01100000 + a, 0b01110000 + b);  // CH5
}
void set_sr() {
  if (sr_vol > 10) {
    sr_vol = 10;
  }
  if (sr_vol < -10) {
    sr_vol = -10;
  }
  sr = mas_vol + sr_vol;
  int c = 79 - sr;
  a = c / 10;
  b = c - a * 10; 
  AX2358_vol(0b10100000 + a, 0b10110000 + b);  // CH6
}

void poweron() {
  Wire.beginTransmission(AX2358_address);
  Wire.endTransmission();
  delay(300);
  // send clear register code as per documents
  Wire.beginTransmission(AX2358_address);
  Wire.write(0b11000100);
  Wire.endTransmission();
  // Input Selection
  Wire.beginTransmission(AX2358_address);
  Wire.write(0b11001011);
  Wire.endTransmission();
  delay(300);
  SurrOn();
  MixOff();
}

void AX2358_send(char c) {
  Wire.beginTransmission(AX2358_address);
  Wire.write(c);
  Wire.endTransmission();
}

void AX2358_vol(char c, char d) {
  Wire.beginTransmission(AX2358_address);
  Wire.write(c);
  Wire.write(d);
  Wire.endTransmission();
}
void AX2358(int x){
  Serial.println(x);
  Wire.beginTransmission(AX2358_address); // transmit to AX2358 chip
  Wire.write (x);
  Wire.endTransmission();
}
