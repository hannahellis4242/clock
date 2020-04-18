#include "RealTimeClock.h"
#include "spi.h"

RealTimeClock::RealTimeClock(volatile uint8_t * out_port , const volatile uint8_t * in_port , const uint8_t ce_pin )
: out_port_( out_port ) , in_port_(in_port) , ce_mask_high_( 1 << ce_pin ) , ce_mask_low_( ~ce_mask_high_ ){}

Array< uint8_t , 7 > RealTimeClock::read() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x00,true);
  Array< uint8_t , 7 > xs ;
  for(unsigned int i = 0 ; i < 7 ; ++i)
  {
    xs[i] = shiftIn(out_port_ , in_port_ , true) ;
  }
  //stop set ce low
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return xs ;
}

void RealTimeClock::write( const Array< uint8_t , 7 > & data ) const
{
  const uint8_t contRegs = getControlRegister() ;
  setControlRegister( contRegs & 0b010000 );

  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case write
  shiftOut(out_port_,0x80,true);
  for(unsigned int i = 0 ; i < 7 ; ++i)
  {
    shiftOut(out_port_ , data[i] , true) ;
  }
  //stop set ce low
  *out_port_ = *out_port_ & ce_mask_low_ ;

  //set the controlRegister back again
  setControlRegister( contRegs ) ;
}

uint8_t RealTimeClock::getSecondsByte() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x00,true);
  const uint8_t seconds = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return seconds ;
}

uint8_t RealTimeClock::getMinutesByte() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x01,true);
  const uint8_t minutes = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return minutes ;
}

uint8_t RealTimeClock::getHoursByte() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x02,true);
  const uint8_t hours = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return hours ;
}

uint8_t RealTimeClock::getDayByte() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x03,true);
  const uint8_t day = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return day ;
}

uint8_t RealTimeClock::getDateByte() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x04,true);
  const uint8_t date = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return date ;
}

uint8_t RealTimeClock::getMonthByte() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x05,true);
  const uint8_t month = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return month ;
}

uint8_t RealTimeClock::getYearByte() const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x06,true);
  const uint8_t year = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return year ;
}

uint8_t RealTimeClock::getControlRegister()const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x0f,true);
  const uint8_t x = shiftIn(out_port_ , in_port_ , true) ;
  *out_port_ = *out_port_ & ce_mask_low_ ;
  return x ;
}

void RealTimeClock::setControlRegister( const uint8_t value )const
{
  //start set ce high
  *out_port_ = *out_port_ | ce_mask_high_ ;
  //write out instruction, in this case read
  shiftOut(out_port_,0x8f,true);
  //set the value
  shiftOut(out_port_ , value , true) ;
  //end set ce low
  *out_port_ = *out_port_ & ce_mask_low_ ;
}
