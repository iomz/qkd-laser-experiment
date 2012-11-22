#include "rs232.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

void adc_init(void){
  // AVcc ref, use ADC5
  ADMUX = _BV(REFS0) | _BV(MUX2) | _BV(MUX0); // | _BV(ADLAR) 

  // Free-running, enable ADC and interrupt, prescaler div factor 16
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2);
}

/* main */
int main(void){
  USART_init(UBRR);
  adc_init();

  sei(); // Enable Global Interrupts
  
  ADCSRA |= _BV(ADSC); // ADC start
 
  for(;;);

  return 0;
}

ISR(ADC_vect){ // ADC complete interrupt
  int val = ADC;
  char str[4] = {0};
  sprintf(str, "%d", val);
  tx_string_USART(str);  
  tx_1byte_USART('\n');
  _delay_ms(300);
}
