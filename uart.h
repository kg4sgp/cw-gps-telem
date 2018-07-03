#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

void uart_init (uint32_t, uint32_t);
void uart_tx_byte (uint8_t);
uint8_t uart_rx_byte (void);
uint8_t uart_rx_flush (void);
uint8_t uart_tx_string (uint8_t* , uint16_t);

#endif
