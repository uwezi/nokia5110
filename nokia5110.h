/*
 * nokia5110.h
 *
 * Created: 2015-06-09 21:07:15
 * version: 2017-04-18 
 *  Author: uwezi
 */ 

#ifndef NOKIA5110_H_
#define NOKIA5110_H_

#include <avr/pgmspace.h>

//
// define your electrical connection to the display here
//

#define RST        PC5
#define RST_PORT   PORTC

#define SCE        PC4
#define SCE_PORT   PORTC

#define SCL        PC1
#define SCL_PORT   PORTC

#define DC         PC3
#define DC_PORT    PORTC

#define SD         PC2
#define SD_PORT    PORTC

#define NOKIA_LED           PD5
#define NOKIA_LED_PORT      PORTD

// rotate the screen 180 degrees
#define NOKIAROTATE      

//
// hardware macros
//

#define DDR(x) (*(&x - 1))      /* address of data direction register of port x */

#define NOKIA_LED_ENABLE()  DDR(NOKIA_LED_PORT)  |=  (1 << NOKIA_LED)
#define NOKIA_LED_DISABLE() DDR(NOKIA_LED_PORT)  &= ~(1 << NOKIA_LED)
#define NOKIA_LED_STATE(x)  NOKIA_LED_PORT &= ~(1 << NOKIA_LED); NOKIA_LED_PORT |= (x << NOKIA_LED)

#define NOKIASIZEX 84
#define NOKIASIZEY 48
extern uint8_t framebuffer[NOKIASIZEX*NOKIASIZEY/8];

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_writeCommand
  Description  :  Sends command to display controller.
  Argument(s)  :  command -> command to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_writeCommand (uint8_t command );

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_writeData
  Description  :  Sends data to display controller.
  Argument(s)  :  data -> data to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_writeData (uint8_t data );

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_gotoXY
  Description  :  Sets cursor location to xy location corresponding to basic font size.
  Argument(s)  :  x - range: 0 to 84
                  y -> range: 0 to 5
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_gotoXY ( uint8_t x, uint8_t y );

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_clearbuffer
  Description  :  Clears the framebuffer but does not transfer to LCD.
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_clearbuffer(void);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_update
  Description  :  transfers the local copy to the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_update (void);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_clear
  Description  :  Clears the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_clear ( void );

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_init
  Description  :  LCD controller initialization.
  Argument(s)  :  contrast value VOP
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_init (uint8_t vop);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_setVop
  Description  :  Sets the contrast voltage level.
  Argument(s)  :  contrast value VOP 0 to 127
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_setVop(uint8_t vop);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_setpixel
  Description  :  Sets the pixel at xy location
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_setpixel(uint8_t x, uint8_t y);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_clearpixel
  Description  :  Clears the pixel at xy location
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_clearpixel(uint8_t x, uint8_t y);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_putchar
  Description  :  puts a single character onto LCD
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
                  ch - character
                  attr - attribute 0-normal, 1-inverse, 2-underline
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_putchar(uint8_t x0, uint8_t y0, char ch, uint8_t attr);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_print
  Description  :  prints a string
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
                  *ch - pointer t string
                  attr - attribute 0-normal, 1-inverse, 2-underline
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
extern void NOKIA_print(uint8_t x, uint8_t y, char *ch, uint8_t attr);

// _p version transferring a string from flash memory
extern void NOKIA_print_p(uint8_t x, uint8_t y, const char *ch, uint8_t attr);

// Macro to automatically put a string constant into program memory
#define NOKIA_print_P(x, y, __s, attr)    NOKIA_print_p(x, y, PSTR(__s), attr);

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_scroll
  Description  :  softscrolls the framebuffer
  Argument(s)  :  dy - range: 0 to +/-47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_scroll(int8_t dy);

#endif /* NOKIA5110_H_ */
