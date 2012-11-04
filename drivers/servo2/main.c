#include "rs232.h"
#include "servo.h"

#include <avr/io.h>
#include <util/delay.h>

void toggle_PB5( void )
{
  PORTB = ( bit_is_clear(PORTB, PB5) ? (1<<PB5) : (0<<PB5) ); // Toggle LED
}

/* main */
int main(void)
{
  unsigned char c;
  USART_init(UBRR);
  PWM_init();
  DDRB  = _BV(DDB1) | _BV(DDB4) |_BV(DDB5);  // Enable PB1(OC1A) as output and PB4,PB5 for debug
  PORTB = _BV(PB5);

  for(;;){
    c = rx_1byte_USART();
    switch(c){
      case '1':rotate_to(0);break;
      case '2':rotate_to(90);break;
      case '3':rotate_to(45);break;
      case '4':rotate_to(135);break;
      default: toggle_PB5();break;
    }
    _delay_ms(500);
    tx_1byte_USART(c);
    toggle_PB5();
  }

  return 0;
}
