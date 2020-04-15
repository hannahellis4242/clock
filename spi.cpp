#include "spi.h"

static uint8_t bit_shift_up_one( const uint8_t x )
{
 return x << 1 ;
};

static uint8_t bit_shift_down_one( const uint8_t x )
{
 return x >> 1 ;
};

void shiftOut( volatile uint8_t *port,
               const uint8_t data ,
               const bool msb_first )
{
  uint8_t mask = msb_first ? 0b10000000 : 0b00000001 ;
  uint8_t (*bitShiftFn)(const uint8_t) = msb_first ? bit_shift_down_one : bit_shift_up_one ;
  while( mask )
  {
    *port = *port & 0b11111110 ; //set clock low
    *port = ( ( mask & data ) != 0 ) ? ( *port | 0b00000010 ) : ( *port & 0b11111101 ); //set mosi high or low
    *port = *port | 0b00000001 ; // set clock high
    mask = bitShiftFn( mask ) ; //move mask along
  }
}

uint8_t shiftIn(volatile uint8_t *port,
                const volatile uint8_t *port_in ,
                const bool msb_first )
{
  uint8_t mask = msb_first ? 0b10000000 : 0b00000001 ;
  uint8_t (*bitShiftFn)(uint8_t) = msb_first ? bit_shift_down_one : bit_shift_up_one ;
  uint8_t x = 0 ;
  while( mask )
  {
    *port = *port & 0b11111110 ; //set clock low
    x = ( ( 0b00000100 & *port_in ) != 0 ) ? ( x | mask ) : x ; //set mosi high or low
    *port = *port | 0b00000001 ; // set clock high
    mask = bitShiftFn( mask ) ; //move mask along
  }
  return x ;
}
