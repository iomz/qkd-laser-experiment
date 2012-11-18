#include "rs232.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define NBITS 20 

typedef unsigned short bool;
#define true 1
#define false 0

ISR(ADC_vect){ // ADC complete interrupt
  int val = ADCH;
  char str[8] = {0};
  sprintf(str, "%d", val);
  tx_string_USART(str);
  tx_1byte_USART('\n');
  _delay_ms(200);
}

void adc_init(void){
  // AVcc ref, use ADC5
  ADMUX = _BV(REFS0) | _BV(ADLAR) | _BV(MUX2) | _BV(MUX0);

  // Free-running, enable ADC and interrupt, prescaler div factor 64
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1);
}

/* main */
int main(void){
  USART_init(UBRR);
  //timer_init();
  adc_init();

  sei(); // Enable Global Interrupts
 
  ADCSRA |= _BV(ADSC);
  while(true){
  }
  return 0;
}
