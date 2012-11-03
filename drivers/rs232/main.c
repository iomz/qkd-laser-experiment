/*
RS232 Test for an Atmega 168

Atmega168 DIP TX PD1 (pin3)
Atmega168 DIP RX PD0 (pin2)
*/
#define F_CPU   8000000UL // 8MHz
#define FOSC    F_CPU 
#define BAUD    9600
#define UBRR    FOSC/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

/* Pin initialize */
void PIN_init(void )
{
}

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

int main(void)
{
  unsigned char c;
  DDRB = 0xFF; // Set PORTB for output
  USART_init(UBRR);

  PORTB=0xFF;
  _delay_ms(1000);
  PORTB=0x00;
  tx_string_USART("Connected!\r\n");

  while(1){
    c = rx_1byte_USART();
    tx_1byte_USART(c);
  }

  return 0;
} 
