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
    rotate_to(90);
  else if (bit == '1' && base == '0')
    rotate_to(0);
  else if (bit == '0' && base == '1')
    rotate_to(135);
  else if (bit == '1' && base == '1')
    rotate_to(45);
  else
    rotate_to(180);
}

/* main */
int main(void){
  unsigned char c, alice_bit, alice_base;
  unsigned short i;
  USART_init(UBRR);
  PWM_init();
  DDRB = _BV(DDB1) |_BV(DDB0);  // Enable PB1(OC1A) and PB0 for the laser

  while(true){
    c = rx_1byte_USART();
    if(c=='c'){
      rotate_polarizer(2,2);
      PORTB |= _BV(PB0); // Turn on the laser
      continue;
    }
    else if(c=='d'){
      PORTB &= ~(_BV(PB0)); // Turn off the laser
      continue;
    }
    else if(c!='s')
      continue;

    PORTB |= _BV(PB0); // Turn on the laser
    for(i=0;i<NBITS;i++){
      toggle_PB4();
      alice_bit = rx_1byte_USART();
      alice_base = rx_1byte_USART();
      rotate_polarizer(alice_bit, alice_base);
      toggle_PB4();
      c = rx_1byte_USART();
      c = rx_1byte_USART();
    }
    PORTB &= ~(_BV(PB0)); // Turn off the laser
  }

  return 0;
}
