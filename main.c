#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "gps.h"

int32_t lat = 1;
int32_t lon = 2;
int16_t alt = 3;
uint8_t fix = 4;

void print_hex(uint8_t * inchar, uint8_t num_len)
{

  /* AVR is little endian, print nibbles from MSB down*/
  for (int8_t cnt = num_len-1; cnt >= 0; cnt--) {

    /* print most significant nibble */
    uint8_t temp_char = (0xf0 & (*(inchar+cnt)))>>4;
    if (temp_char < 10) {
      uart_tx_byte(temp_char+48);
    } else {
      uart_tx_byte(temp_char+87); /* Lower case a-f */
      /* uart_tx_byte(temp_char+55); */ /* Upper case A-F */
    }

    /* print least significant nibble */
    temp_char = 0x0f & (*(inchar+cnt));
    if (temp_char < 10) {
      uart_tx_byte(temp_char+48);
    } else {
      uart_tx_byte(temp_char+87);
      /* uart_tx_byte(temp_char+55); */ /* Upper case A-F */
    }

  }

}



int main(void)
{

  uart_init(F_CPU, 9600);

  /* Power on blink */
  DDRD = _BV(4);
  PORTD = 0x00;
  _delay_ms(50);
  PORTD = _BV(4);

  //uart_tx_string((uint8_t *)output, sizeof(output));

	gps_init();
	gps_enable();
	gps_heartbeat((uint8_t)1);

  for(;;){
		_delay_ms(5000);
		gps_disable();
  	gps_read(&lat, &lon, &alt, &fix);
		lat = 1;
		uart_tx_byte('>');
		print_hex((uint8_t *)&lat, 4);
		uart_tx_byte(',');
		print_hex((uint8_t *)&lon, 4);
		uart_tx_byte(',');
		print_hex((uint8_t *)&alt, 2);
		uart_tx_byte(',');
		print_hex((uint8_t *)&fix, 1);
		uart_tx_byte('\r');
		uart_tx_byte('\n');
		gps_enable();
	}

  return 0;
}
