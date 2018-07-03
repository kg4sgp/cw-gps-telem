#ifndef GPS_H_INCLUDED
#define GPS_H_INCLUDED

//extern volatile int32_t gps_latit = 0;
//extern volatile int32_t gps_longi = 0;
//extern volatile int16_t gps_alt = 0;
//extern volatile uint8_t gps_fix = 0;

void gps_read(int32_t *, int32_t *, int16_t *, uint8_t *);
void gps_enable(void);
void gps_disable(void);
void gps_heartbeat(uint8_t);
void gps_init(void);

#endif
