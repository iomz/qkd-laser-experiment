/*
Servo control test for an ATMega 168P
ATMega168 DIP ADC0 (pin23,PC0)
*/
#define F_CPU  8000000UL // 8MHz
#define FOSC    F_CPU 
#define BAUD    9600
#define UBRR    FOSC/16/BAUD-1

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <util/setbaud.h>


/* Initializes the USART (RS232 interface) */
void USART_init( unsigned int ubrr )
{
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);;     // Enable RX, TX & RX interrupt
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);    // asynchronous 8 N 1
}

/* Receive 1 byte Data */
unsigned char rx_1byte_USART( void )
{
  loop_until_bit_is_set(UCSR0A, RXC0);
  return UDR0;
}

/* Send 1 byte data */
void tx_1byte_USART( unsigned char data )
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data;
}

/* Send String */
void tx_string_USART( char *str )
{
  while (*str != 0){
    tx_1byte_USART(*str);
    str++;
    _delay_ms(250);
  }
}

/* Convert integer to a string */
#include <inttypes.h>
char* itoa(uint16_t number, uint8_t base)
{
  if(!number){
    return "0\r\n";
  }
  
  static char buf[16] = {0};
  register char i = 12;
  buf[14] = '\n';
  buf[13] = '\r';
  char m = 0;
  
  if(number < 0){
    number = number * (-1);
    m = 1;
  }
  
  for(; number && i ; --i, number /= base)
    buf[i] = "0123456789ABCDEF"[number % base];
  
  if(m){
    buf[i] = '-';
    return &buf[i];
  }else{
    return &buf[i+1];
  }
}

int main(void)
{
  DDRB = 0xFF; // Set PORTB for output
  USART_init(UBRR);
  ADCSRA |= _BV(ADSC);

  _delay_ms(1000);
  tx_string_USART("Connected!\r\n");

  while(1){
    PORTB=0xFF;
    while(ADCSRA & _BV(ADSC));
    tx_string_USART(itoa(ADC, 10));
    _delay_ms(1000);
    PORTB=0x00;
    _delay_ms(1000);
  }

  return 0;
} 
