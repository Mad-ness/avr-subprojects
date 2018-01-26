#ifndef __RCTSWITCH_H__
#define __RCTSWITCH_H__

/*
 The code is based on this sources https://github.com/sui77/rc-switch.
 But because the origin supports too many boards it makes very hard
 running it on attiny13 and attiny85. So I just get from there only 
 minimal code for sending and receiving data by protocol 1.
*/

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "common.h"

/*

 Example of usage:
   call RCTSwitch_setup() to configure PCINT0 interrupt
   write in your main.c 
   to get received data run this
   if RCTSwitch_available() {
       uint8_t value = RCTSwitch_getValue();
       RCTSwitch_reset();
   } 

   To configure another interrupt - modify the sources!
 
*/


struct pulse_t {
  uint8_t high;
  uint8_t low;
};

struct protocol_t {
  int pulse_length;
  struct pulse_t sync_factor;
  struct pulse_t zero;
  struct pulse_t one;
  bool inverted;
};

uint8_t RCTSwitch_getValue();
void RCTSwitch_setup();
void RCTSwitch_reset();
void RCTSwitch_interruptHandler();
uint8_t RCTSwitch_available();

#endif // __RCTSWITCH_H__

