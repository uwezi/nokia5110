/*
 * nokia5110.c
 *
 * a small test program for the Nokia5110 library
 *
 * Created: 2017-04-18 09:53:47
 * Author : uwezi
 */ 

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "nokia5110.h"

int main(void)
{
  uint8_t i=0;
  char    buffer[16];
  
  NOKIA_init(0x42);                 // contrast setting for my display 0x42
  NOKIA_clearbuffer();
  
  while (1) 
  {
    sprintf(buffer, "%3d", i);
    
    NOKIA_scroll(8);                // scroll display 8 pixels up
    NOKIA_print( 0, 40, buffer, 1); // print in last row, inverted

    NOKIA_update();                 // update framebuffer to screen
    NOKIA_print( 0, 40, buffer, 0); // overwrite normal in framebuffer
    
    i++;
    _delay_ms(200);
  }
}

