#include "servo.h"
#include <avr/io.h>

/* PWM init */
void PWM_init( void )
{
  /*
  COM1A1: Set OC1A on BOTTOM and clear on Compare match
  WGM13,12,11: Fast WPM, TOP=ICR1, TOP->BOTTOM mode
  CS11: Clock select prescaler as 8
  */
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
  
  TCNT1 = 0x0000;    // Initialize the counter register
  ICR1  = 19999;     // Set TOP as 19999: 8MHz /(8*(1+19999)) = 50Hz 
}

/* Convert degree to pulse width and rotate */
void rotate_to( unsigned short angle)
{
  switch(angle){
    case 0: OCR1A = OCR1A_MIN + 60;break;
    case 45: OCR1A = OCR1A_MIN + 570;break;
    case 90: OCR1A = OCR1A_MIN + 1080;break;
    case 135: OCR1A = OCR1A_MIN + 1590;break;
    default: break;
  }
}
