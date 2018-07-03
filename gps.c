/* GPS Parser optimized for memory size Proof of concept */

#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
//#include "uart.h"

/* supporting variables */
volatile uint8_t cksum = 0;
volatile uint8_t gps_buf[16] = { 0 };
volatile uint8_t gps_buf_i = 0;
volatile uint8_t comma = 0;
volatile uint8_t gpgga = 0;
volatile uint8_t ck = 0;
volatile uint8_t ck_string = 0;
volatile uint8_t ck_i = 0;

/* variables gained from GPS reciever and visible to other programs*/
volatile int32_t gps_latit = 4;
volatile int32_t gps_longi = 3;
volatile int16_t gps_alt = 2;
volatile uint8_t gps_fix = 1;
volatile uint8_t gps_en = 1;
volatile uint8_t gps_heart = 0;

void gps_init(void)
{
  UCSRB |= (1<<RXCIE);
  sei();
}

uint8_t hex_to_dec(char inchar) 
{
  uint8_t res = 0;
  if(inchar < 58) {
    res = inchar-48;
  } else if (inchar < 71) {
    res = inchar-55;
  } else if (inchar < 103) {
    res = inchar-87;
  }

  return res;
}

void gps_enable(void)
{
	gps_en = 1;
}

void gps_disable(void)
{
	gps_en = 0;
}

void gps_heartbeat(uint8_t setting)
{

	if (setting == 1) {
		gps_heart = 1;
	} else {
		gps_heart = 0;
	}

}

void gps_read(int32_t * lat, int32_t * lon, int16_t * alt, uint8_t * fix)
{
	/* wait if currently in GPGGA packet */
	while(gpgga == 1);

  /* disable parsing */
	gps_en = 0;

	/* copy over variables */
	*lat = gps_latit;
	*lon = gps_longi;
	*alt = gps_alt;
	*fix = gps_fix;

	/* re-enable parsing */
	gps_en = 1;
}

ISR(USART_RX_vect)
{
		//print gps packets instead of parsing them
		//uart_tx_byte(UDR);
	  //return;

		//if (gps_en == 0) return;

    /* read char from serial*/
    gps_buf[gps_buf_i] = UDR;

    /* if this is the start of a NEMA packet '$' set to beginning of buffer */
    if (gps_buf[gps_buf_i] == '$' || gps_en == 0) {
      gps_buf_i = 0;
      cksum = 0;
      ck = 0;
      ck_i = 0;
      comma = 0;
      gpgga = 0;
      ck_string = 0;
      //gps_latit = 0;
      //gps_longi = 0;
      //gps_alt = 0;
      //gps_fix = 0;
      return;
    }

    if (gps_buf[gps_buf_i] == '*') {
      ck = 1;
      return;
    }

    /* Compute checksum, check if end of packet, set flag */
    if (ck != 1) {

      /* increment the checksum */
      cksum ^= gps_buf[gps_buf_i];

    } else if (ck_i == 0) {

      ck_string = hex_to_dec(gps_buf[gps_buf_i])<<4;
      ck_i++;

    } else if (ck_i == 1) {

      ck_i++;
      ck_string += hex_to_dec(gps_buf[gps_buf_i]);

      if (ck_string == cksum) {
       	/* Do commands here for good checksum! */
				/* Tiny blink to show traffic on USART */
				//PORTD &= ~(_BV(4));
				//PORTD |= _BV(4);

	      if(gpgga == 1 && gps_fix != 0) {

	      	/* Blink for GPS fix, GPGGA packet, and good checksum */
					if (gps_heart == 1) {
						PORTD &= ~(_BV(4));
						_delay_ms(1);
						PORTD |= _BV(4);
						_delay_ms(150);
						PORTD &= ~(_BV(4));
						_delay_ms(1);
						PORTD |= _BV(4);
					}

    	  } else if (gpgga == 1) {

					if (gps_heart == 1) {
						PORTD &= ~(_BV(4));
						_delay_ms(1);
						PORTD |= _BV(4);
					}

				}

      } else {

        //uart_tx_byte('b');

      }
     
      return;
    }

    /* keep track of how many commas we are at, and take care of information */
    if (gps_buf[gps_buf_i] == ','){
      comma++;

			/* Determine Sentance */
			if (comma == 1) {

	      if (gps_buf[0] == 'G' &&
          gps_buf[1] == 'P' &&
          gps_buf[2] == 'G' &&
          gps_buf[3] == 'G' &&
          gps_buf[4] == 'A') {
					gpgga = 1;
      		gps_latit = 0;
      		gps_longi = 0;
      		gps_alt = 0;
      		gps_fix = 0;
					return;
				}

			}

      /* do actions on information in buffer */
      /* if GPGGA sentance then take data */
      if (gpgga == 1) {

        if (comma == 3) {

          /* Latitude */
          char degrees[3] = { 0 };
          degrees[0] = gps_buf[0];
          degrees[1] = gps_buf[1];
          degrees[2] = '\0';
          
          char minutes[3] = { 0 };
          minutes[0] = gps_buf[2];
          minutes[1] = gps_buf[3];
          minutes[2] = '\0';
          
          /* posibility that dminutes doesnt go to 5? */
          char dminutes[6] = { 0 };
          dminutes[0] = gps_buf[5];
          dminutes[1] = gps_buf[6];
          dminutes[2] = gps_buf[7];
          dminutes[3] = gps_buf[8];
          dminutes[4] = gps_buf[9];
          dminutes[5] = '\0';
					          
          uint32_t deg, min, dmin;
          /* 1/60 in Q0.32 */
          uint32_t ones = 71582788;
          /* 1/100000 in Q0.32 */
          uint32_t onet = 42950;
          
          /* Fixed point calculation of Latitude */
          /* result in Q8.17 */
          deg =  atol(degrees);
          min =  atol(minutes);
          dmin = atol(dminutes);
          gps_latit = (deg<<17) +  (((min*ones) + ((((uint64_t)dmin)*onet*ones)>>32))>>15);
          
        } else if (comma == 4) {

          /* N or S */
          /* if south bit mask negative on latitude */
					if(gps_buf[0] == 'S') {
						gps_latit = -gps_latit;
					}

        } else if (comma == 5) {
       
 
          /* Longitude */
          char degrees[4] = { 0 };
          degrees[0] = gps_buf[0];
          degrees[1] = gps_buf[1];
          degrees[2] = gps_buf[2];
          degrees[3] = '\0';
          
          char minutes[3] = { 0 };
          minutes[0] = gps_buf[3];
          minutes[1] = gps_buf[4];
          minutes[2] = '\0';
          
          /* posibility that dminutes doesnt go to 5? */
          char dminutes[6] = { 0 };
          dminutes[0] = gps_buf[6];
          dminutes[1] = gps_buf[7];
          dminutes[2] = gps_buf[8];
          dminutes[3] = gps_buf[9];
          dminutes[4] = gps_buf[10];
          dminutes[5] = '\0';
 
          uint32_t deg, min, dmin;
          uint32_t ones = 71582788;
          uint32_t onet = 42950;
          
          /* Fixed point calculation of Longitude */
          /* result in Q8.17 */ 
          deg =  atol(degrees);
          min =  atol(minutes);
          dmin = atol(dminutes);
          gps_longi = (deg<<17) +  (((min*ones) + ((((uint64_t)dmin)*onet*ones)>>32))>>15);

        } else if (comma == 6) {

          /* E or W */
          if(gps_buf[0] == 'W') {
						gps_longi = -gps_longi;
					}
        } else if (comma == 7) {
         
          /* Fix Quality */
          gps_fix = gps_buf[0]-48;

				//} else if (comma == 8) {

					/* Sats in view */
					//uart_tx_byte(gps_buf[0]);
					//uart_tx_byte(gps_buf[1]);

        } else if (comma == 10) {
        
          /* WGS-84 altitude */
          char altitude[5] = { 0 };
          uint8_t i;
          for(i = 0; gps_buf[i] != '.'; i++) {
            altitude[i] = gps_buf[i];
          }
          
          /* scale to 16bits */
          /* 0 - 131072m in 2m increments */
          int32_t alt32 = atol(altitude);
          gps_alt = alt32/2;
        }
      }
      gps_buf_i = 0;
      return;
    }

    gps_buf_i++;
}
