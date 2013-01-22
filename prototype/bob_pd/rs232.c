#include "rs232.h"
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/setbaud.h>

/* Initializes the USART (RS232 interface) */
void USART_init( unsigned int ubrr )
{
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0B = _BV(TXEN0); // Enable TX
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // asynchronous 8 N 1
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
  }
}
