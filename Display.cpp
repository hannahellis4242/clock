#include "Display.h"
#include "spi.h"

static const uint8_t numberLookup[] = {
  //pgfedcba
  0b00111111, //0
  0b00000110, //1
  0b01011011, //2
  0b01001111, //3
  0b01100110, //4
  0b01101101, //5
  0b01111101, //6
  0b00000111, //7
  0b01111111, //8
  0b01100111, //9
  0b01110111, //A
  0b01111100, //b
  0b00111001, //C
  0b01011110, //d
  0b01111001, //E
  0b01110001, //F
};

Display::Display(volatile uint8_t *port ,
                 const uint8_t ce_pin )
        : spi_port_( port ) ,
          ce_mask_high_( 1 << ce_pin ) ,
          ce_mask_low_( ~ce_mask_high_ ){}

void Display::raw(const uint8_t value) const
{
  *spi_port_ = *spi_port_ & ce_mask_low_; //set ce low
  shiftOut(spi_port_,value,true);
  *spi_port_ = *spi_port_ | ce_mask_high_; //set ce high
}

void Display::show(const uint8_t value) const
{
  raw(numberLookup[value]);
}
