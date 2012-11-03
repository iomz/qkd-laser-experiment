/*
Servo control test for an ATMega 168P
ATMega168 DIP PDM OC1A (pin15,PB1)
*/
#define F_CPU  8000000UL // 8MHz

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

/*
Pulse width can be calculated by
pulse_width = PULSE_MIN + (ANGLE)/(ROTATION_RANGE)*PULSE_RANGE
*/
#define OCR1A_MIN   610

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
    case 0: OCR1A = OCR1A_MIN + 60;PORTB=_BV(PB4)|_BV(PB5);break;
    case 1: OCR1A = OCR1A_MIN + 570;PORTB=_BV(PB4);break;
    case 2: OCR1A = OCR1A_MIN + 1590;PORTB=_BV(PB5);break;
    case 3: OCR1A = OCR1A_MIN + 1080;PORTB=(0<<PB4)|(0<<PB5);;break;
    default: break;
  }
  // PORTB = ( bit_is_clear(PORTB, PB5) ? (1<<PB5) : (0<<PB5) ); // Toggle LED
}

/* main */
int main(void)
{
  unsigned short angle = 0; // Start with 0
  PWM_init();
  DDRB  = _BV(DDB1) | _BV(DDB4) |_BV(DDB5);         // Enable PB1(OC1A) as output and PB4,PB5 for debug
  PORTB = _BV(PB5);

  for(;;){
    _delay_ms(1000);
    rotate_to(angle);
    angle = (angle+1)%4;
  }

  return 0;
}
