/*
 * nokia5110.c
 *
 * Created: 2015-06-09 21:07:02
 * version: 2017-04-17 
 *  Author: uwezi
 */ 

#include <stdlib.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "nokia5110.h"

#ifndef F_CPU
#define F_CPU 20000000UL   // save to assume the fastest clock
#endif

#include <util/delay.h>

/*
** constants/macros   (thanks Peter Fleury)
*/

#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
/* on ATmega64/128 PINF is on port 0x00 and not 0x60 */
#define PIN(x) ( &PORTF==&(x) ? _SFR_IO8(0x00) : (*(&x - 2)) )
#else
#define PIN(x) (*(&x - 2))    /* address of input register of port x          */
#endif


//
// macros for the software SPI
//

#define SetRST     RST_PORT |= (1 << RST) 
#define ClearRST   RST_PORT &=~(1 << RST)
#define SetSCE     SCE_PORT |= (1 << SCE)
#define ClearSCE   SCE_PORT &=~(1 << SCE)
#define SetDC      DC_PORT  |= (1 << DC)
#define ClearDC    DC_PORT  &=~(1 << DC)
#define SetSD      SD_PORT  |= (1 << SD)
#define ClearSD    SD_PORT  &=~(1 << SD)
#define SetSCL     SCL_PORT |= (1 << SCL)
#define ClearSCL   SCL_PORT &=~(1 << SCL)  


//  6x8 font
//    LSB is top
//    MSB is bottom
//
static const uint8_t smallFont[][6] PROGMEM =
#include "font_6x8_iso8859_1.h"

//
//  define a local copy of the display memory
//
uint8_t framebuffer[NOKIASIZEX*NOKIASIZEY/8];

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_writeCommand
  Description  :  Sends command to display controller.
  Argument(s)  :  command -> command to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_writeCommand (uint8_t command )
{
  uint8_t i;
  // the bit-banging routines may not be interrupted!
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    ClearSCE;       //enable LCD
    ClearDC;        // set LCD into command mode
    ClearSCL;
    for (i=0; i<8; i++)
    {
      if (command & 0b10000000)
      {
        SetSD;
      }
      else
      {
        ClearSD;
      }
      SetSCL;       // minimum 100 ns
      ClearSCL;     // minimum 100 ns
      command <<= 1;
    }
    SetSCE;         // disable LCD
  }  
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_writeData
  Description  :  Sends data to display controller.
  Argument(s)  :  data -> data to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_writeData (uint8_t data )
{
    uint8_t i;
    // the bit-banging routines may not be interrupted!
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      ClearSCE;       // enable LCD
      SetDC;          // set LCD in data mode
      ClearSCL;
#ifdef NOKIAROTATE
      for (i=0; i<8; i++)
      {
        if (data & 0b00000001)
        {
          SetSD;
        }
        else
        {
          ClearSD;
        }
        SetSCL;       // minimum 100 ns
        ClearSCL;     // minimum 100 ns
        data >>= 1;
      }
#else
      for (i=0; i<8; i++)
      {
        if (data & 0b10000000)
        {
          SetSD;
        }
        else
        {
          ClearSD;
        }
        SetSCL;       // minimum 100 ns
        ClearSCL;     // minimum 100 ns
        data <<= 1;
      }
#endif
      SetSCE;         // disable LCD
    }      
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_gotoXY
  Description  :  Sets cursor location to xy location corresponding to basic font.
  Argument(s)  :  x - range: 0 to 84
                  y -> range: 0 to 5
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_gotoXY ( uint8_t x, uint8_t y )
{
    NOKIA_writeCommand (0x80 | x);   //column
    NOKIA_writeCommand (0x40 | y);   //row
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_clearbuffer
  Description  :  Clears the framebuffer but does not transfer to LCD.
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_clearbuffer(void)
{
  memset(framebuffer, 0x00, NOKIASIZEX*NOKIASIZEY/8);
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_update
  Description  :  transfers the local framebuffer to the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_update (void)
{
  uint16_t i;

  NOKIA_gotoXY(0,0);      // start with (0,0) position
  for(i=0; i<(NOKIASIZEX*NOKIASIZEY/8); i++)
  {
#ifdef NOKIAROTATE
    NOKIA_writeData(framebuffer[NOKIASIZEX*NOKIASIZEY/8-i-1]);
#else
    NOKIA_writeData(framebuffer[i]);
#endif
  }
  NOKIA_gotoXY(0,0);      // bring the XY position back to (0,0)
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_clear
  Description  :  Clears the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_clear ( void )
{
    NOKIA_clearbuffer();
    NOKIA_update();
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_init
  Description  :  LCD controller initialization.
  Argument(s)  :  contrast value VOP
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_init (uint8_t vop)
{

  DDR(SCE_PORT) |= (1 << SCE);
  DDR(RST_PORT) |= (1 << RST);
  DDR(SCL_PORT) |= (1 << SCL);
  DDR(DC_PORT)  |= (1 << DC);
  DDR(SD_PORT)  |= (1 << SD);

  _delay_ms(100);

  // the bit-banging routines may not be interrupted!
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    ClearSCE;                  // Enable LCD
    ClearRST;                  // reset LCD
    
    _delay_ms(100);
    SetRST;
    SetSCE;                    //disable LCD
  }  
  
  NOKIA_writeCommand( 0x21 );  // LCD Extended Commands.
  NOKIA_writeCommand( 0x80 | vop );  // Set LCD Vop (Contrast).
  NOKIA_writeCommand( 0x04 );  // Set Temp coefficent.
  NOKIA_writeCommand( 0x13 );  // LCD bias mode 1:48.
  NOKIA_writeCommand( 0x20 );  // LCD Standard Commands, Horizontal addressing mode.
  NOKIA_writeCommand( 0x0c );  // LCD in normal mode.

  NOKIA_clear();
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_setVop
  Description  :  Sets the contrast voltage
  Argument(s)  :  contrast value VOP 0 to 127
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_setVop(uint8_t vop)
{
    NOKIA_writeCommand( 0x21 );  // LCD Extended Commands.
    NOKIA_writeCommand( 0x80 | vop );  // Set LCD Vop (Contrast).
    NOKIA_writeCommand( 0x20 );  // LCD Standard Commands, Horizontal addressing mode.
    NOKIA_writeCommand( 0x0c );  // LCD in normal mode.
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_setpixel
  Description  :  Sets the pixel at xy location
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_setpixel(uint8_t x, uint8_t y)
{
  if ((x < NOKIASIZEX) && (y < NOKIASIZEY))
  {
    framebuffer[(uint16_t) x+NOKIASIZEX*(y/8)] |= (1 << (y % 8));
  }
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_clearpixel
  Description  :  Clears the pixel at xy location
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_clearpixel(uint8_t x, uint8_t y)
{
  if ((x < NOKIASIZEX) && (y < NOKIASIZEY))
  {
    framebuffer[(uint16_t) x+NOKIASIZEX*(y/8)] &= ~(1 << (y % 8));
  }
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_putchar
  Description  :  puts a single character onto LCD
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
                  ch - character
                  attr - attribute 0-normal, 1-inverse, 2-underline
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_putchar(uint8_t x0, uint8_t y0, char ch, uint8_t attr)
{
  uint8_t yd, ym, i, fontbyte;
  uint16_t m;
  yd = y0/8;
  ym = y0%8;
  for (i=0; i<6; i++)
  {
    fontbyte = pgm_read_byte(&smallFont[(uint8_t)ch][i]);
    switch (attr)
    {
      case  0:
          break;
      case  1: 
          fontbyte ^= 0xff;
          break;
      case  2: 
          fontbyte |= 0b10000000;
          break;
    }

    if ((x0+i)<NOKIASIZEX)
    {
      m = (uint16_t) x0+i+NOKIASIZEX*(yd);
      framebuffer[m] &= ~(0xff << ym);
      framebuffer[m] |= (fontbyte << ym);
      if ((y0<(NOKIASIZEY-8)) && (ym != 0))
      {
        m = (uint16_t) x0+i+NOKIASIZEX*(yd+1);
        framebuffer[m] &= ~(0xff >> (8-ym));
        framebuffer[m] |= (fontbyte >> (8-ym));
      }
    }
  }
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_print
  Description  :  prints a string
  Argument(s)  :  x - range: 0 to 83
                  y - range: 0 to 47
                  *ch - pointer t string
                  attr - attribute 0-normal, 1-inverse, 2-underline
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_print(uint8_t x, uint8_t y, char *ch,uint8_t attr)
{
  while (*ch)
  {
    NOKIA_putchar(x, y, *ch, attr);
    ch++;
    x += 6;
  }
}

void NOKIA_print_p(uint8_t x, uint8_t y, const char *ch,uint8_t attr)
{
  char c;
  while ((c = pgm_read_byte(ch)))
  {
    NOKIA_putchar(x, y, c, attr);
    ch++;
    x += 6;
  }
}

/*--------------------------------------------------------------------------------------------------
  Name         :  NOKIA_scroll
  Description  :  softscrolls the framebuffer
  Argument(s)  :  dy - range: 0 to +/-47
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void NOKIA_scroll(int8_t dy)
{
  int8_t y1;
  uint8_t  x, y, dy1, dy8, b1, b2;
  if (dy>0)
  {
    dy8 = dy/8;
    dy1 = dy%8;
    for (x=0; x<NOKIASIZEX; x++)
    {
      for (y=0; y<(NOKIASIZEY/8); y++)
      {
        y1=y+dy8;
        if (y1<(NOKIASIZEY/8))
        {
          b1 = framebuffer[x + NOKIASIZEX*y1];
        }
        else
        {
          b1=0;
        }
        if ((y1+1)<(NOKIASIZEY/8))
        {
          b2 = framebuffer[x + NOKIASIZEX*(y1+1)];
        }
        else
        {
          b2=0;
        }
        framebuffer[x + NOKIASIZEX*(y)] = (b1 >> dy1) | (b2 << (8-dy1));
      }
    }
  } 
  else
  {
    dy8 = abs(dy)/8;
    dy1 = abs(dy)%8;
    for (x=0; x<NOKIASIZEX; x++)
    {
      for (y=0; y<(NOKIASIZEY/8); y++)
      {
        y1=(NOKIASIZEY/8)-y-dy8-1;
        if (y1>=0)
        {
          b1 = framebuffer[x + NOKIASIZEX*y1];
        }
        else
        {
          b1=0;
        }
        if ((y1-1)>=0)
        {
          b2 = framebuffer[x + NOKIASIZEX*(y1-1)];
        }
        else
        {
          b2=0;
        }
        framebuffer[x + NOKIASIZEX*((NOKIASIZEY/8)-y-1)] = (b1 << dy1) | (b2 >> (8-dy1));
      }
    }
  }
}
