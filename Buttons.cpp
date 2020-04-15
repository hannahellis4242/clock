#include "Buttons.h"

#include "spi.h"

Buttons::Buttons(volatile uint8_t * out_port , const volatile uint8_t * in_port , const uint8_t ce_pin )
: out_port_( out_port ) , in_port_(in_port) , ce_mask_high_( 1 << ce_pin ) , ce_mask_low_( ~ce_mask_high_ ){}

uint8_t Buttons::read() const
{
  //to enable the chip drop chip enable low
  *out_port_ = *out_port_ & ce_mask_low_; //set ce low
  //to store the current state of the pins, send mosi low then clock once
  *out_port_ = *out_port_ & mosi_low; //set mosi low
  //flash clock up to load the memory
  *out_port_ = *out_port_ | clk_high ; // set clock high
  *out_port_ = *out_port_ & clk_low ; // set clock low
  //data is now loaded for shifing out
  //put into shift out mode
  *out_port_ = *out_port_ | mosi_high ; //set mosi high
  //now shift in
  const uint8_t x = shiftIn(out_port_,in_port_);
  //stop
  *out_port_ = *out_port_ | ce_mask_high_;
  return x ;
}
