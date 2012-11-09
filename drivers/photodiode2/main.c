#define F_CPU   8000000UL // 8MHz
#define BAUD    9600
#define UBRR    F_CPU/16/BAUD-1

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/setbaud.h>

void USART_init( unsigned int ubrr )
{
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0B = _BV(TXEN0);     // Enable TX
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);    // asynchronous 8 N 1
}

void tx_1byte( unsigned char data)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data;
}

void tx_string(char * string, int len)
{
  int i;
  for (i=0; i<len; i++){
    tx_1byte(string[i]);
    _NOP();
  }
}

void toggle_PB5( void )
{
  PORTB = ( bit_is_clear(PORTB, PB5) ? (1<<PB5) : (0<<PB5) ); // Toggle LED
}

int main(void)
{
  USART_init(UBRR);
  DDRB = _BV(DDB5);

  ADMUX = _BV(REFS0) | _BV(ADLAR) | _BV(MUX2) | _BV(MUX0); // AVcc ref, use ADC5

  // Free-running, enable ADC and interrupt, prescaler div factor 64
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1);

  sei(); // Enable Global Interrupts

  ADCSRA |= _BV(ADSC); // Start ADC
  
  for(;;){
  }
  return 0;
} 

ISR(ADC_vect) //timer0 overflow interrupt
{
    tx_1byte(ADCH);
    toggle_PB5();
    _delay_ms(250);
}
