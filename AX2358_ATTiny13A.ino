#include <util/delay.h>

#define SDA_PIN 4  // Adjust these values based on your actual pin connections
#define SCL_PIN 3
#define AX2358_address 0b1001010
#define Surr_On 0b11000000
#define Surr_Off 0b11000001
#define Mix_On 0b11000010

int gain = 0b11010000;
int level = 0b11100000;

void i2c_start() {
  PORTB |= (1 << SDA_PIN) | (1 << SCL_PIN);
  _delay_us(4);
  PORTB &= ~(1 << SDA_PIN);
  _delay_us(4);
  PORTB &= ~(1 << SCL_PIN);
}

void i2c_stop() {
  PORTB &= ~(1 << SDA_PIN);
  PORTB |= (1 << SCL_PIN);
  _delay_us(4);
  PORTB |= (1 << SDA_PIN);
  _delay_us(4);
}

void i2c_write_byte(uint8_t byte) {
  for (uint8_t i = 0; i < 8; i++) {
    if (byte & 0x80) {
      PORTB |= (1 << SDA_PIN);
    } else {
      PORTB &= ~(1 << SDA_PIN);
    }
    byte <<= 1;
    _delay_us(2);
    PORTB |= (1 << SCL_PIN);
    _delay_us(4);
    PORTB &= ~(1 << SCL_PIN);
    _delay_us(2);
  }
  // Receive ACK
  PORTB |= (1 << SDA_PIN);
  _delay_us(2);
  PORTB |= (1 << SCL_PIN);
  _delay_us(2);
  if (PINB & (1 << SDA_PIN)) {
    // Not ACK received
    // Handle the error or implement retries
  }
  PORTB &= ~(1 << SCL_PIN);
  _delay_us(4);
}

void setup() {
  DDRB |= (1 << SDA_PIN) | (1 << SCL_PIN);  // Set SDA and SCL pins as output
  i2c_start();
  i2c_write_byte(AX2358_address << 1);  // Write address
  i2c_write_byte(0b11000100);  // Clear register code
  i2c_stop();
  _delay_ms(300);

  i2c_start();
  i2c_write_byte(AX2358_address << 1);  // Write address
  i2c_write_byte(0b11001011);  // Input selection
  i2c_stop();
  _delay_ms(300);

  i2c_start();
  i2c_write_byte(AX2358_address << 1);  // Write address
  i2c_write_byte(0b11000100);
  i2c_write_byte(gain);
  i2c_write_byte(level);
  i2c_stop();

  i2c_start();
  i2c_write_byte(AX2358_address << 1);  // Write address
  i2c_write_byte(Surr_On);  // Surrun selection
  i2c_stop();
  _delay_ms(300);

  i2c_start();
  i2c_write_byte(AX2358_address << 1);  // Write address
  i2c_write_byte(Mix_On);  // Mixon selection
  i2c_stop();
  _delay_ms(300);

}

void loop() {}
