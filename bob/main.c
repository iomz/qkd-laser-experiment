#include "rs232.h"
#include <avr/io.h>
#include <util/delay.h>

#define NBITS 20 
#define OCR1_MIN   610

typedef unsigned short bool;
#define true 1
#define false 0

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
    case '0': OCR1A = OCR1_MIN + 60;break;  // 0 degree
    case '1': OCR1A = OCR1_MIN + 570;break; // 45 degree
    default: OCR1A = OCR1_MIN + 2100;break; // Calibration point
  }
}

void eve_rotate_to( unsigned char angle){
  switch(angle){
    case '0': OCR1B = OCR1_MIN + 60;break;  // 0 degree
    case '1': OCR1B = OCR1_MIN + 570;break; // 45 degree
    default: OCR1B = OCR1_MIN + 2100;break; // Calibration point
  }
}

void calibration_mode(void){
  bob_rotate_to('2'); // Rotate polarizers
  eve_rotate_to('2'); //   to calibration positions
}

void manual_mode(void){
  unsigned char c = rx_1byte_USART();
  c = rx_1byte_USART();
  toggle_PB4();
  unsigned char bob_base = rx_1byte_USART();
  bob_rotate_to(bob_base);
  unsigned char eve_base = rx_1byte_USART();
  eve_rotate_to(eve_base);
  tx_1byte_USART('E');
  toggle_PB4();
}

void zero_mode(void){
  bob_rotate_to('0'); // Rotate polarizers
  eve_rotate_to('0'); //   to zero position
}

/* main */
int main(void){
  unsigned char c, bob_base, eve_base;
  unsigned short i;
  USART_init(UBRR);
  PWM_init();
  DDRB = _BV(DDB1) | _BV(DDB2);  // Enable PB1(OC1A) and PB2(OC1B) as PWM control 

  toggle_PB3(); // Wait lump
  zero_mode();
  while(true){
    c = rx_1byte_USART();
    if(c=='c'){     // Calibration mode
      calibration_mode();
      continue;
    }
    else if(c=='m'){ // Manual mode
      manual_mode();
      continue;
    }
    else if(c=='z'){ // Zero mode
      zero_mode();
      continue;
    }
    else if(c!='s') // Not Start loop
      continue;
   
    zero_mode();  // Initialize positions
    toggle_PB3(); // Turn off wait lump
    _delay_ms(100);
    for(i=0;i<NBITS;i++){
      c = rx_1byte_USART();
      c = rx_1byte_USART();
      toggle_PB4();
      bob_base = rx_1byte_USART();
      bob_rotate_to(bob_base);
      eve_base = rx_1byte_USART();
      eve_rotate_to(eve_base);
      tx_1byte_USART('E');
      toggle_PB4();
    }
    toggle_PB3(); // Turn on wait lump
    _delay_ms(2000);
   }

  return 0;
}
