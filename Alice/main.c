#include "rs232.h"
#include "servo.h"

#include <avr/io.h>
#include <util/delay.h>

#define NBITS 20 


void toggle_PB5( void ){
  PORTB = ( bit_is_clear(PORTB, PB5) ? (1<<PB5) : (0<<PB5) ); // Toggle LED
}

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
}


/* main */
int main(void){
  unsigned char c;
  unsigned short i;
  USART_init(UBRR);
  PWM_init();
  DDRB  = _BV(DDB1) |_BV(DDB5);  // Enable PB1(OC1A) as output and PB5 for LED
  PORTB = _BV(PB5); // Turn on PB5

  for(;;){
    c = rx_1byte_USART();
    if(c!='s')
      continue;

    tx_1byte_USART(c); // Send START signal to Bob module
    for(i=0;i<NBITS;i++){
      unsigned char alice_bit = rx_1byte_USART();
      unsigned char alice_base = rx_1byte_USART();
      unsigned char bob_base = rx_1byte_USART();
      unsigned char eve_base = rx_1byte_USART();

      rotate_polarizer(alice_bit, alice_base);
      tx_1byte_USART(bob_base);
      tx_1byte_USART(eve_base);
    }
  }

  return 0;
}
