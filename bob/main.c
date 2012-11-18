#include "rs232.h"
#include <avr/io.h>
#include <util/delay.h>

#define NBITS 20 
#define OCR1_MIN   610

/* PWM init */
void PWM_init( void ){
  /*
  COM1A1: Set OC1A/OC1B on BOTTOM and clear on Compare match
  WGM13,12,11: Fast WPM, TOP=ICR1, TOP->BOTTOM mode
  CS11: Clock select prescaler as 8
  */
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) |  _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
  
  TCNT1 = 0x0000;    // Initialize the counter register
  ICR1  = 19999;     // Set TOP as 19999: 8MHz /(8*(1+19999)) = 50Hz 
}

/* Convert degree to pulse width and rotate */
void bob_rotate_to( unsigned char angle){
  switch(angle){
    case '0': OCR1A = OCR1_MIN + 1080;break; // 90 degree
    case '1': OCR1A = OCR1_MIN + 1590;break; // 135 degree
    default: OCR1A = OCR1_MIN + 2100;break;
  }
}

void eve_rotate_to( unsigned char angle){
  switch(angle){
    case '0': OCR1B = OCR1_MIN + 1080;break; // 90 degree
    case '1': OCR1B = OCR1_MIN + 1590;break; // 1355 degree
    default: OCR1B = OCR1_MIN + 2100;break;
  }
}

/* main */
int main(void){
  unsigned char c, bob_base, eve_base;
  unsigned short i;
  USART_init(UBRR);
  PWM_init();
  DDRB = _BV(DDB1) | _BV(DDB2);  // Enable PB1(OC1A) and PB2(OC1B) as PWM control 

  for(;;){
    c = rx_1byte_USART();
    if(c=='c'){
      bob_rotate_to(2);
      eve_rotate_to(2);
      continue;
    }
    else if(c!='s')
      continue;
    
    toggle_PB5();
    for(i=0;i<NBITS;i++){
      c = rx_1byte_USART();
      c = rx_1byte_USART();
      toggle_PB4();
      bob_base = rx_1byte_USART();
      eve_base = rx_1byte_USART();
      bob_rotate_to(bob_base);
      eve_rotate_to(eve_base);
      toggle_PB4();
    }
    toggle_PB5();
  }

  return 0;
}
