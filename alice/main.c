#include "rs232.h"
#include "servo.h"

#include <avr/io.h>
#include <util/delay.h>

#define NBITS 20

typedef unsigned short bool;
#define true 1
#define false 0

// Precondition: both bit and base must be 0 or 1
void rotate_polarizer(unsigned char bit, unsigned char base){
  if (bit == '0' && base == '0')
    rotate_to(0);
  else if (bit == '1' && base == '0')
    rotate_to(90);
  else if (bit == '0' && base == '1')
    rotate_to(45);
  else if (bit == '1' && base == '1')
    rotate_to(135);
  else
    rotate_to(180);
}

void on_laser(void){
  PORTB |= _BV(PB0); // Turn on the laser
}

void off_laser(void){
  PORTB &= ~(_BV(PB0)); // Turn off the laser
}

void calibration_mode(void){
  rotate_polarizer('2','2');
}

void manual_mode(void){
  toggle_PB4();
  unsigned char alice_bit = rx_1byte_USART();
  unsigned char alice_base = rx_1byte_USART();
  rotate_polarizer(alice_bit, alice_base);
  toggle_PB4();
  unsigned char c = rx_1byte_USART();
  c = rx_1byte_USART();
}

void zero_mode(void){
  rotate_polarizer('0','0');
}

/* main */
int main(void){
  unsigned char c, alice_bit, alice_base;
  unsigned short i;
  USART_init(UBRR);
  PWM_init();
  DDRB = _BV(DDB1) | _BV(DDB0);  // Enable PB1(OC1A) and PB0 for the laser

  toggle_PB3(); // Wait on
  zero_mode();
  while(true){
    c = rx_1byte_USART();
    if(c=='c'){
      calibration_mode(); 
      continue;
    }
    else if(c=='d'){ // Disable laser emission
      off_laser();
      continue;
    }
    else if(c=='e'){ // Enabe laser emission
      on_laser();
      continue;
    }
    else if(c=='m'){ // Manual mode
      manual_mode();
      continue;
    }
    else if(c=='z'){ // Zero mode
      zero_mode();
      continue;
    }
    else if(c!='s') // Not start loop
      continue;

    zero_mode();
    on_laser();
    toggle_PB3(); // Wait off
    _delay_ms(100);
    for(i=0;i<NBITS;i++){
      toggle_PB4();
      alice_bit = rx_1byte_USART();
      alice_base = rx_1byte_USART();
      rotate_polarizer(alice_bit, alice_base);
      toggle_PB4();
      c = rx_1byte_USART();
      c = rx_1byte_USART();
    }
    toggle_PB3(); // Wait on
    _delay_ms(2000);
    off_laser();
  }

  return 0;
}
