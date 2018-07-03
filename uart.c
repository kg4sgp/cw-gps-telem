/* Code for AVR microcontroller UART communication  */

#include <avr/interrupt.h>
#include "uart.h"

void uart_init (uint32_t fosc, uint32_t baud_rate)
{
  /* disable gloabl interrupt */
  cli();

  /* set baud rate */
  uint16_t baud_prescale = (uint16_t)((fosc / (baud_rate * 16))-1);
  UBRRH = (uint8_t)(baud_prescale >>8);
  UBRRL = (uint8_t)(baud_prescale);

  /* set frame format */
  UCSRC = (1<<USBS)|(3<<UCSZ0); 

  /* enable the tx or rx */
  UCSRB = (1<<RXEN)|(1<<TXEN);

  /* enable global interrupts */
  sei();
}

void uart_tx_byte (uint8_t byte)
{
  /* wait for register to be empty */
  while ( !(UCSRA & (1<<UDRE)) );
  /* transmit the next byte */
  UDR = (uint8_t)byte;
}

uint8_t uart_rx_byte (void)
{
  /* wait for data in the rx register */
  while ( !(UCSRA & (1<<RXC)) );
  /* return a recieved byte */
  return (uint8_t)UDR;
}

uint8_t uart_rx_flush (void)
{
  uint8_t dummy = 0;
  while ( UCSRA & (1<<RXC) ) dummy = UDR;
  return dummy;
}

uint8_t uart_tx_string (uint8_t * string_ptr, uint16_t string_sz)
{
  uint8_t i = 0;
  for(i = 0; i < string_sz; i++){
    uart_tx_byte(string_ptr[i]);
  }
  return i;
}

