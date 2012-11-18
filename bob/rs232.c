#include "rs232.h"
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/setbaud.h>

/* Initializes the USART (RS232 interface) */
void USART_init( unsigned int ubrr )
{
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);     // Enable RX, TX & RX interrupt
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);    // asynchronous 8 N 1
}

/* Receive 1 byte Data */
unsigned char rx_1byte_USART( void )
{
  loop_until_bit_is_set(UCSR0A, RXC0);
  toggle_PB5();
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
    _NOP();
  }
}

void toggle_PB5( void ){
  if(!bit_is_set(DDRB, DDB5))
    DDRB |= _BV(DDB5);
  if(bit_is_clear(PORTB, PB5))
    PORTB |= _BV(PB5);
  else
    PORTB &= ~(_BV(PB5));
}

void toggle_PB4( void ){
  if(!bit_is_set(DDRB, DDB4))
    DDRB |= _BV(DDB4);
  if(bit_is_clear(PORTB, PB4))
    PORTB |= _BV(PB4);
  else
    PORTB &= ~(_BV(PB4));
}
