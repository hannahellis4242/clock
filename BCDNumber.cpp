#include "BCDNumber.h"

BCDNumber::BCDNumber( const uint8_t & data )
: data_( data ) {}

const uint8_t & BCDNumber::raw() const
{
  return data_ ;
}

uint8_t BCDNumber::units() const
{
  return data_ & 0x0f ;
}

uint8_t BCDNumber::tens() const
{
  return ( data_ & 0xf0 ) >> 4 ;
}

uint8_t BCDNumber::asByte() const
{
  return units() + 10 * tens() ;
}
