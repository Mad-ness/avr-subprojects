#ifndef __DS1302_H__
#define __DS1302_H__
#include <stdint.h>
#include <stdbool.h>

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1U << (bit)))
#define bitClear(value, bit) ((value) &= ~(1U << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define sbi( reg, bit ) ( reg |= ( 1 << bit ))
#define cbi( reg, bit ) ( reg &= ~( 1 << bit ))

/*
typedef struct { 
   uint8_t Second; 
   uint8_t Minute; 
   uint8_t Hour; 
   uint8_t Wday;   // day of week, sunday is day 1
   uint8_t Day;
   uint8_t Month; 
   uint8_t Year;   // offset from 1970; 
} tmElements_t;
*/

typedef struct {
   uint8_t hour;
   uint8_t minute;
   uint8_t second;
} ShortTime_t;

// Adopted from https://github.com/iot-playground/Arduino/blob/master/external_libraries/DS1302RTC/DS1302RTC.cpp
void DS1302Tiny_init(const int ce, const int io, const int sclk);
void DS1302Tiny_readRTC( uint8_t *p );
bool DS1302Tiny_getTime(ShortTime_t *tm);

#endif // __DS1302_H__

