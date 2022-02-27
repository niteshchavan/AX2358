#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <IRremote.h>

#define AX2358_address 0b1001010

#define btn_delay 300

#define sw_power 13     // Out

// IR HEX code
#define ir_power      0x807F827D    // IR power ON/OFF
#define ir_mute       0x807F42BD    // IR mute
#define ir_in_0       0x807F629D    // IR input BLU
#define ir_in_1       0x807FA25D    // IR input FM
#define ir_in_2       0x807F22DD    // IR input AUD
#define ir_in_3       0x807F20DF    // IR input DVD
#define ir_in_4       0x807F02FD    // IR input PRO
#define ir_vol_i      0x807F906F    // IR vol++
#define ir_vol_d      0x807FA05F    // IR vol-- 
#define ir_fl_i       0x807F40BF    // IR fl++
#define ir_fl_d       0x807FC03F    // IR fl-- 
#define ir_fr_i       0x807F00FF    // IR fr++
#define ir_fr_d       0x807F807F    // IR fr--
#define ir_sl_i       0x807F48B7    // IR sl++
#define ir_sl_d       0x807FC837    // IR sl--
#define ir_sr_i       0x807F08F7    // IR sr++
#define ir_sr_d       0x807F8877    // IR sr--
#define ir_cn_i       0x807F50AF    // IR cn++
#define ir_cn_d       0x807F609F    // IR cn--
#define ir_sub_i      0x807FD02F    // IR sub++
#define ir_sub_d      0x807FE01F    // IR sub-- 
#define ir_sp_mode    0x807F52AD    // IR speaker mode change
#define ir_surr_mode  0x807F18E7    // IR surround ON/OFF
#define ir_mix_mode   0x807FD827    // IR -6dB ON/OFF
#define ir_reset      0x807F1AE5    // IR reset



IRrecv irrecv(8);
decode_results results;

byte custom_num[8][8] = {
  { B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111 },
  { B11111, B11111, B11111, B00000, B00000, B00000, B00000, B00000 },
  { B11100, B11110, B11111, B11111, B11111, B11111, B11111, B11111 },
  { B11111, B11111, B11111, B11111, B11111, B11111, B01111, B00111 },
  { B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111 },
  { B11111, B11111, B11111, B11111, B11111, B11111, B11110, B11100 },
  { B11111, B11111, B11111, B00000, B00000, B00000, B11111, B11111 },
  { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 }
};

const int digit_width = 3;
const char custom_num_top[10][digit_width] = { 0, 1, 2, 1, 2, 32, 6, 6, 2, 6, 6, 2, 3, 4, 7,   7, 6, 6, 0, 6, 6, 1, 1, 2,   0, 6, 2, 0, 6, 2};
const char custom_num_bot[10][digit_width] = { 3, 4, 5, 4, 7, 4,  7, 4, 4, 4, 4, 5, 32, 32, 7, 4, 4, 5, 3, 4, 5, 32, 32, 7, 3, 4, 5, 4, 4, 5};

byte arrow_right[8] = {B00000, B10000, B11000, B11100, B11110, B11100, B11000, B10000};

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // RS,E,D4,D5,D6,D7

unsigned long time;
int in, mute, return_d, surr, mix, a, b, x, power, menu, menu_active, ch_mute, speaker_mode, btn_press, long_press, vol_menu, vol_menu_jup, reset;
int fl, fr, sl, sr, cn, sub, ir_menu, ir_on, mas_vol, fl_vol, fr_vol, sl_vol, sr_vol, cn_vol, sub_vol;

long btn_timer = 0;
long long_press_time = 600;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  irrecv.enableIRIn();

  pinMode(sw_power, OUTPUT); // Out

  digitalWrite(sw_power, HIGH);

  lcd.begin(16, 2);
  
  power = 0;
  eeprom_read();
  start_up();
  power_up();

}

void loop() {
  lcd_update();
  eeprom_update();
  ir_control();
  return_delay();

  if (menu_active == 0) {
    custom_num_shape();
  } else {
    custom_shape();
  }

}

//eeprom -----------------------------------------------------//

void eeprom_update() {
  EEPROM.update(0, in);
  EEPROM.update(1, mas_vol);
  EEPROM.update(2, fl_vol + 10);
  EEPROM.update(3, fr_vol + 10);
  EEPROM.update(4, sl_vol + 10);
  EEPROM.update(5, sr_vol + 10);
  EEPROM.update(6, cn_vol + 10);
  EEPROM.update(7, sub_vol + 10);
  EEPROM.update(8, surr);
  EEPROM.update(9, speaker_mode);
  EEPROM.update(10, mix);
}

void eeprom_read() {
  in = EEPROM.read(0);
  mas_vol = EEPROM.read(1);
  fl_vol = EEPROM.read(2) - 10;
  fr_vol = EEPROM.read(3) - 10;
  sl_vol = EEPROM.read(4) - 10;
  sr_vol = EEPROM.read(5) - 10;
  cn_vol = EEPROM.read(6) - 10;
  sub_vol = EEPROM.read(7) - 10;
  surr = EEPROM.read(8);
  speaker_mode = EEPROM.read(9);
  mix = EEPROM.read(10);
}


void ir_cl() {
  time = millis();
  return_d = 1;
}
void return_delay() {
  if (millis() - time > 5000 && return_d == 1 && mute == 0 && menu_active != 0) {
    menu_active = 0;
    vol_menu = 0;
    reset = 0;
    return_d = 0;
    lcd.clear();
  } else if (millis() - time > 5000 && return_d == 1 && mute == 0 && menu_active == 0) {
    vol_menu = 0;
    return_d = 0;
  }
}

//power up -----------------------------------------------------//

void power_up() {
  if (power == 1) {
    lcd.clear();
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("   LOADING...   ");
    delay(1000);
    lcd.clear();
    if (mas_vol > 19) {
      mute = 0;
    }
    set_mute();
    vol_menu = 0;
    menu_active = 0;
    delay(300);
    ir_on = 1;
    vol_menu_jup = 0;
    digitalWrite(sw_power, HIGH);

  } else {

    digitalWrite(sw_power, LOW);
    mute = 1;
    set_mute();
    delay(100);
    menu_active = 100;
    ir_on = 0;
  }
}

void start_up() {
  mute = 1;
  set_mute();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("    Nitesh     ");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("   5.1 SYSTEM   ");
  delay(1000);
  lcd.clear();
  delay(300);
  lcd.setCursor(0, 1);
  lcd.print("   LOADING...   ");
  delay(1500);
  lcd.clear();
  delay(300);
  AX2358();
  set_in();
  set_surr();
  set_mix();
  set_fl();
  set_fr();
  set_sl();
  set_sr();
  set_cn();
  set_sub();
}

//IR control --------------------------------------------------------------------------------//

void ir_control() {
  if ( irrecv.decode( &results )) {
    Serial.println(results.value, HEX);
    switch (results.value) {
      //power -------------------------------------------------//
      case ir_power:
        power++;
        if (power > 1) {
          power = 0;
        }
        power_up();
        break;
    }
    if (ir_on == 1) {
      switch (results.value) {
        //mute -------------------------------------------------//
        case ir_mute:
          if (mas_vol != 19) {
            mute++;
            if (mute == 1) {
              menu_active = 99;
            } else {
              menu_active = 0;
            }
            set_mute();
            lcd.clear();  
          }
          
          break;

        //select input -------------------------------------------------//
        case ir_in_0:
          in = 0;
          set_in();
          ir_cl();
          break;

        case ir_in_1:
          in = 1;
          set_in();
          ir_cl();
          break;

        case ir_in_2:
          in = 2;
          set_in();
          ir_cl();
          break;
          
        case ir_in_3:
          in = 3;
          set_in();
          ir_cl();
          break;

        case ir_in_4:
          in = 4;
          set_in();
          ir_cl();
          break;
      }
    }

    if (ir_on == 1 && menu_active == 0) {
      switch (results.value) {
        //VOL -------------------------------------------------//
        case ir_vol_i:
          if (speaker_mode == 0 || speaker_mode == 1) {
            mas_vol++;
            vol_menu = 0;
            set_mas_vol();
            set_fl();
            set_fr();
            set_sub();
            if (speaker_mode == 0) {
              set_sl();
              set_sr();
              set_cn();
            }
          }
          break;

        case ir_vol_d:
          if (speaker_mode == 0 || speaker_mode == 1) {
            mas_vol--;
            vol_menu = 0;
            set_mas_vol();
            set_fl();
            set_fr();
            set_sub();
            if (speaker_mode == 0) {
              set_sl();
              set_sr();
              set_cn();
            }
          }
          break;

        //FL -------------------------------------------------//
        case ir_fl_i:
          if (speaker_mode == 0 || speaker_mode == 1) {
            fl_vol++;
            vol_menu = 1;
            set_fl();
          }
          break;

        case ir_fl_d:
          if (speaker_mode == 0 || speaker_mode == 1) {
            fl_vol--;
            vol_menu = 1;
            set_fl();
          }
          break;

        //FR -------------------------------------------------//
        case ir_fr_i:
          if (speaker_mode == 0 || speaker_mode == 1) {
            fr_vol++;
            vol_menu = 2;
            set_fr();
          }
          break;

        case ir_fr_d:
          if (speaker_mode == 0 || speaker_mode == 1) {
            fr_vol--;
            vol_menu = 2;
            set_fr();
          }
          break;

        //SL -------------------------------------------------//
        case ir_sl_i:
          if (speaker_mode == 0) {
            sl_vol++;
            vol_menu = 3;
            set_sl();
          }
          break;

        case ir_sl_d:
          if (speaker_mode == 0) {
            sl_vol--;
            vol_menu = 3;
            set_sl();
          }
          break;

        //SR -------------------------------------------------//
        case ir_sr_i:
          if (speaker_mode == 0) {
            sr_vol++;
            vol_menu = 4;
            set_sr();
          }
          break;

        case ir_sr_d:
          if (speaker_mode == 0) {
            sr_vol--;
            vol_menu = 4;
            set_sr();
          }
          break;

        //CN -------------------------------------------------//
        case ir_cn_i:
          if (speaker_mode == 0) {
            cn_vol++;
            vol_menu = 5;
            set_cn();
          }
          break;

        case ir_cn_d:
          if (speaker_mode == 0) {
            cn_vol--;
            vol_menu = 5;
            set_cn();
          }
          break;

        //SUB -------------------------------------------------//
        case ir_sub_i:
          if (speaker_mode == 0 || speaker_mode == 1) {
            sub_vol++;
            vol_menu = 6;
            set_sub();
          }
          break;

        case ir_sub_d:
          if (speaker_mode == 0 || speaker_mode == 1) {
            sub_vol--;
            vol_menu = 6;
            set_sub();
          }
          break;

        //speaker mode -------------------------------------------------//
        case ir_sp_mode:
          speaker_mode++;
          vol_menu = 0;
          if (speaker_mode == 1) {
            vol_menu_jup = 0;
          }
          set_speaker_mode();
          break;

        //surround -------------------------------------------------//
        case ir_surr_mode:
          surr++;
          vol_menu = 0;
          set_surr();
          break;

        // -------------------------------------------------//
        case ir_mix_mode:
          mix++;
          vol_menu = 0;
          set_mix();
          break;

        // -------------------------------------------------//
        case ir_reset:
          reset++;
          vol_menu = 0;
          set_reset();
          break;
      }
      ir_cl();
    }
    irrecv.resume();
  }
}

//custom shape --------------------------------------------------------------------------------//

void custom_num_shape() {
  for (int i = 0; i < 8; i++)
    lcd.createChar(i, custom_num[i]);
}

void custom_shape() {
  lcd.createChar(1, arrow_right);
}


//lcd ---------------------------------------------------------//

void lcd_update() {
  int c;
  switch (menu_active) {
    case 0:
      //input -------------------------------------------------//
      lcd.setCursor(0, 0);
      if (in == 0) {
        lcd.print("BLU");
      }
      if (in == 1) {
        lcd.print("FMX");
      }
      if (in == 2) {
        lcd.print("AUX");
      }
      if (in == 3) {
        lcd.print("DVD");
      }
      if (in == 4) {
        lcd.print("PRO");
      }

      //speaker mode ------------------------------------------//
      lcd.setCursor(4, 0);
      if (speaker_mode == 0) {
        lcd.print("5.1");
      }
      if (speaker_mode == 1) {
        lcd.print("2.1");
      }

      //vol ----------------------------------------------//
      switch (vol_menu) {
        case 0:
          lcd.setCursor(0, 1);
          //       ("       ");
          lcd.print("MAS-VOL");
          c = mas_vol - 19;
          break;

        case 1:
          lcd.setCursor(0, 1);
          //       ("       ");
          lcd.print("FL-VOL ");
          c = fl_vol;
          break;

        case 2:
          lcd.setCursor(0, 1);
          //       ("       ");
          lcd.print("FR-VOL ");
          c = fr_vol;
          break;

        case 3:
          lcd.setCursor(0, 1);
          //       ("       ");
          lcd.print("SL-VOL ");
          c = sl_vol;
          break;

        case 4:
          lcd.setCursor(0, 1);
          //       ("       ");
          lcd.print("SR-VOL ");
          c = sr_vol;
          break;

        case 5:
          lcd.setCursor(0, 1);
          //       ("       ");
          lcd.print("CN-VOL ");
          c = cn_vol;
          break;

        case 6:
          lcd.setCursor(0, 1);
          //       ("       ");
          lcd.print("SUB-VOL");
          c = sub_vol;
          break;
      }
      break;

    case 99:
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("      MUTE      ");
      break;

    case 100:
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("    STANDBY     ");
      break;
  }
  if (menu_active == 0) {
    int y;
    if (c < 0) {
      lcd.setCursor(8, 1);
      lcd.print("-");
      y = 10 - (c + 10);
    } else if (c == -10) {
      lcd.setCursor(8, 1);
      lcd.print("-");
      y = 10;
    } else {
      lcd.setCursor(8, 1);
      lcd.print(" ");
      y = c;
    }
    a = y / 10;
    b = y - a * 10;

    lcd.setCursor(9, 0);
    for (int i = 0; i < digit_width; i++)
      lcd.print(custom_num_top[a][i]);

    lcd.setCursor(9, 1);
    for (int i = 0; i < digit_width; i++)
      lcd.print(custom_num_bot[a][i]);

    lcd.setCursor(13, 0);
    for (int i = 0; i < digit_width; i++)
      lcd.print(custom_num_top[b][i]);

    lcd.setCursor(13, 1);
    for (int i = 0; i < digit_width; i++)
      lcd.print(custom_num_bot[b][i]);
  }

}

//all reset --------------------------------------------------------------------------------//

void set_reset() {
  if (reset == 1) {
    in = 0;
    mas_vol = 44;
    fl_vol = 0;
    fr_vol = 0;
    sl_vol = 0;
    sr_vol = 0;
    cn_vol = 0;
    sub_vol = 0;
    speaker_mode = 0;
    surr = 0;
    mix = 0;
    vol_menu = 0;
    menu_active = 0;
    reset = 0;
    lcd.clear();
  }
  set_in();
  set_fl();
  set_fr();
  set_sl();
  set_sr();
  set_cn();
  set_sub();
  set_speaker_mode();
  set_surr();
  set_mix();
}

//speaker mode --------------------------------------------------------------------------------//

void set_speaker_mode() {
  if (speaker_mode > 1) {
    speaker_mode = 0;
  }
  if (speaker_mode < 0) {
    speaker_mode = 1;
  }
  switch (speaker_mode) {
    case 0:                     // 5.1 mode
      ch_mute = 0;
      break;
    case 1:                     // 2.1 mode
      ch_mute = 1;
      break;
  }
  set_sl();
  set_sr();
  set_cn();
}

//AX2358 settings -----------------------------------------------------//

void set_in() {
  if (in > 4) {
    in = 0;
  }
  switch (in) {
    case 0: a = 0b11001011; break; // 1 input
    case 1: a = 0b11001010; break; // 2 input
    case 2: a = 0b11001001; break; // 3 input
    case 3: a = 0b11001000; break; // 4 input
    case 4: a = 0b11001111; break; // 6 CH input
  }
  AX2358_send(a);
}
void set_surr() {
  if (surr > 1) {
    surr = 0;
  }
  if (surr < 0) {
    surr = 1;
  }
  switch (surr) {
    case 0: a = 0b11000000; Serial.println(surr); lcd.setCursor(0, 1); lcd.print("SUR- ON "); delay(1000); break; // Surround ON
    case 1: a = 0b11000001; Serial.println(surr); lcd.setCursor(0, 1); lcd.print("SUR-OFF "); delay(1000);break; // Surround OFF
  }
  AX2358_send(a);
}
void set_mix() {
  if (mix > 1) {
    mix = 0;
  }
  switch (mix) {
    case 0: a = 0b11000010; Serial.println(mix); lcd.setCursor(0, 1); lcd.print("MIX-OFF "); delay(1000); break; // (-6dB) on
    case 1: a = 0b11000011; Serial.println(mix); lcd.setCursor(0, 1); lcd.print("MIX-ON "); delay(1000); break; // (-6dB) off
  }
  AX2358_send(a);
}

//AX2358 Volume settings ----------------------------------------------//

void set_mas_vol() {
  if (mas_vol > 69) {
    mas_vol = 69;
  }
  if (mas_vol < 19) {
    mas_vol = 19;
  }
  if (mas_vol == 19) {
    mute = 1;
  } else {
    mute = 0;
  }
  set_mute();
}
void set_mute() {
  if (mute > 1) {
    mute = 0;
  }
  switch (mute) {
    case 0:                     // 5.1 mode
      ch_mute = 0;
      break;
    case 1:                     // 2.1 mode
      ch_mute = 1;
      break;
  }
  set_fl();
  set_fr();
  set_sub();
  if (speaker_mode == 0) {
    set_sl();
    set_sr();
    set_cn();
  }
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
  AX2358_vol(0b10000000 + a, 0b10010000 + b);  // CH1
  
  switch (ch_mute) {
    case 0: x = 0b11110000; break; // Mute disabled
    case 1: x = 0b11110001; break; // Mute
  }
  AX2358_send(x);
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
  AX2358_vol(0b01000000 + a, 0b01010000 + b);  // CH2

  switch (ch_mute) {
    case 0: x = 0b11110010; break; // Mute disabled
    case 1: x = 0b11110011; break; // Mute
  }
  AX2358_send(x);
}
void set_cn() {
  if (cn_vol > 10) {
    cn_vol = 10;
  }
  if (cn_vol < -10) {
    cn_vol = -10;
  }
  cn = mas_vol + cn_vol;
  int c = 79 - cn;
  a = c / 10;
  b = c - a * 10;
  AX2358_vol(0b00000000 + a, 0b00010000 + b);  // CH3

  switch (ch_mute) {
    case 0: x = 0b11110100; break; // Mute disabled
    case 1: x = 0b11110101; break; // Mute
  }
  AX2358_send(x);
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

  switch (ch_mute) {
    case 0: x = 0b11110110; break; // Mute disabled
    case 1: x = 0b11110111; break; // Mute
  }
  AX2358_send(x);
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

  switch (ch_mute) {
    case 0: x = 0b11111000; break; // Mute disabled
    case 1: x = 0b11111001; break; // Mute
  }
  AX2358_send(x);
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

  switch (ch_mute) {
    case 0: x = 0b11111010; break; // Mute disabled
    case 1: x = 0b11111011; break; // Mute
  }
  AX2358_send(x);
}

//AX2358 send -----------------------------------------------------//

void AX2358_send(char c) {
  Wire.beginTransmission(AX2358_address);
  Wire.write (c);
  Wire.endTransmission();
}
void AX2358() {
  Wire.beginTransmission(AX2358_address);
  Wire.write (0b11000100);
  Wire.endTransmission();
}
void AX2358_vol(char c, char d) {
  Wire.beginTransmission(AX2358_address);
  Wire.write (c);
  Wire.write (d);
  Wire.endTransmission();
}

//end code
