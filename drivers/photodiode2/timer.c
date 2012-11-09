#include "rs232.h"

#include <avr/io.h>
#include <avr/interrupt.h>

void toggle_PB5( void )
{
  PORTB = ( bit_is_clear(PORTB, PB5) ? (1<<PB5) : (0<<PB5) ); // Toggle LED
}

int main(void)
{
  unsigned short i,value;
  USART_init(UBRR);
  DDRB = _BV(DDB5);
  PORTB = _BV(PB5);

  ADMUX = _BV(REFS0) | _BV(ADLAR) | _BV(MUX2) | _BV(MUX0); // AVcc ref, use ADC5

  TCCR0A |= _BV(WGM01);  // Set the Timer Mode to CTC
  OCR0A = 0xFF;          // Value to count to
  TIMSK0 |= _BV(OCIE0A); // Set the ISR COMPA vect
  sei(); // enable interrupts
  TCCR0B |= _BV(CS02) | _BV(CS00);   // Set prescaler to 1024

  // Free-running, enable ADC and interrupt, prescaler div factor 64
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1);

  sei(); // Enable Global Interrupts


  while(1){
      while(ADCSRA & _BV(ADSC));
      value += ADC;
      ADCSRA = _BV(ADEN) | _BV(ADIF) | _BV(ADPS2) | _BV(ADPS1);
    }
    value = value>>3;
    tx_1byte_USART(value);
    toggle_PB5();
    _delay_ms(1000);
  }
  return 0;
} 

ISR(TIMER0_COMPA_Vect) //timer0 overflow interrupt
{
    
}
