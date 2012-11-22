#include "rs232.h"
#include "servo.h"

#include <avr/io.h>
#include <util/delay.h>

#define OCR1_MIN   610
#define NBITS 20

typedef unsigned short bool;
#define true 1
#define false 0

/* Convert degree to pulse width and rotate */
void rotate_to( unsigned short angle)
{
  switch(angle){
    case 0: OCR1A = OCR1_MIN + 60;break;
    case 45: OCR1A = OCR1_MIN + 570;break;
    case 90: OCR1A = OCR1_MIN + 1080;break;
    case 135: OCR1A = OCR1_MIN + 1590;break;
    default: OCR1A = OCR1_MIN + 2100;break;
  }
}

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

// Precondition: both bit and base must be 0 or 1
void rotate_polarizer(unsigned char bit, unsigned char base){
  if (bit == '0' && base == '0')
    rotate_to(0);
  else if (bit == '1' && base == '0')
    rotate_to(90);
  else if (bit == '1' && base == '1')
    rotate_to(45);
  else if (bit == '0' && base == '1')
    rotate_to(135);
  else
    rotate_to(180);
}

void on_laser(void){
  PORTB |= _BV(PB0); // Turn on the laser
}

void off_laser(void){
  PORTB &= ~(_BV(PB0)); // Turn off the laser
}

void manual_mode(void){
  toggle_PB4();
  unsigned char alice_bit = rx_1byte_USART();
  unsigned char alice_base = rx_1byte_USART();
  rotate_polarizer(alice_bit, alice_base);
  toggle_PB4();
  unsigned char c = rx_1byte_USART();
  c = rx_1byte_USART();
}

void open_mode(void){
  rotate_polarizer('0','0');
}

/* main */
int main(void){
  unsigned char c, alice_bit, alice_base;
  unsigned short i;
  USART_init(UBRR);
  PWM_init();
  DDRB = _BV(DDB1) | _BV(DDB0);  // Enable PB1(OC1A) and PB0 for the laser

  toggle_PB5(); // Wait on
  open_mode();
  while(true){
    c = rx_1byte_USART();
    if(c=='l'){ // Toggle laser emission
      PORTB ^= _BV(PB0);
      continue;
    }
    else if(c=='m'){ // Manual mode
      manual_mode();
      continue;
    }
    else if(c!='s')  // Not start loop
      continue;

    open_mode();
    on_laser();
    toggle_PB5(); // Wait off
    _delay_ms(100);
    for(i=0;i<NBITS;i++){
      toggle_PB4();
      alice_bit = rx_1byte_USART();
      alice_base = rx_1byte_USART();
      rotate_polarizer(alice_bit, alice_base);
      toggle_PB4();
      c = rx_1byte_USART();
      c = rx_1byte_USART();
    }
    toggle_PB5(); // Wait on
    _delay_ms(2000);
    off_laser();
  }

  return 0;
}
