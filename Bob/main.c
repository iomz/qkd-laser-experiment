#include "rs232.h"
#include "servo.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define NBITS 20 

void toggle_PB5( void ){
  PORTB = ( bit_is_clear(PORTB, PB5) ? (1<<PB5) : (0<<PB5) ); // Toggle LED
}

// Precondition: both bit and base must be 0 or 1
void rotate_polarizer(unsigned char base){
  if (base == '0')
    rotate_to(0);
  else if (base == '1')
    rotate_to(45);
}

/* main */
int main(void){
  unsigned char c;
  unsigned short i;
  USART_init(UBRR);
  PWM_init();
  DDRB  = _BV(DDB1) |_BV(DDB5);  // Enable PB1(OC1A) as output and PB5 for LED
  PORTB = _BV(PB5); // Turn on PB5


  /* Timer setup */
  TCCR0A |= _BV(WGM01);  // Set the Timer Mode to CTC
  OCR0A = 0xFF;          // Value to count to
  TIMSK0 |= _BV(OCIE0A); // Set the ISR COMPA vect
  sei(); // enable interrupts
  TCCR0B |= _BV(CS02) | _BV(CS00);   // Set prescaler to 1024

  /* ADC setup */
  // AVcc ref, use ADC5
  ADMUX = _BV(REFS0) | _BV(ADLAR) | _BV(MUX2) | _BV(MUX0);
  // Free-running, enable ADC and interrupt, prescaler div factor 64
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1);

  for(;;){
    c = rx_1byte_USART();
    if(c!='s')
      continue;

    sei(); // Enable Global Interrupts
    for(i=0;i<NBITS;i++){
      unsigned char bob_base = rx_1byte_USART();
      unsigned char eve_base = rx_1byte_USART();
      tx_1byte_USART('E');
      rotate_polarizer(eve_base);
      rotate_polarizer(bob_base);
      tx_1byte_USART('S'); 
    }
    cli(); // Disable Global Interrupts
  }

  return 0;
}

ISR(TIMER0_COMPA_vect) //timer0 overflow interrupt
{
  ADCSRA |= _BV(ADSC); // Start ADC
}

ISR(ADC_vect) //timer0 overflow interrupt
{
  tx_1byte_USART(ADCH);
  tx_1byte_USART('\n');
  ADCSRA |= (0<<ADSC); // End ADC
}
