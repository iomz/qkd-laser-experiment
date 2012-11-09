#include "rs232.h"

#include <avr/io.h>
#include <util/delay.h>

void toggle_PB5( void )
{
  PORTB = ( bit_is_clear(PORTB, PB5) ? (1<<PB5) : (0<<PB5) ); // Toggle LED
}

int main(void)
{
  USART_init(UBRR);
  DDRB = _BV(DDB5);
  PORTB = _BV(PB5);

  ADMUX = _BV(REFS0) | _BV(ADLAR) | _BV(MUX2) | _BV(MUX0); // AVcc ref, use ADC5

  while(1){
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1);
    while(ADCSRA & _BV(ADSC));
    ADCSRA = _BV(ADPS2) | _BV(ADPS1);
    tx_1byte_USART(ADCH);
    tx_string_USART("\n\r");
    _delay_ms(500);
    toggle_PB5();
    _delay_ms(500);
    toggle_PB5();
  }

  return 0;
} 
