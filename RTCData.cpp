#include "RTCData.h"


RTCData::RTCData(const uint8_t (&xs)[7])
: seconds_( xs[0] ) ,
  minutes_( xs[1] ) ,
  day_( xs[3] ) ,
  date_( xs[4] ) ,
  month_( xs[5] ),
  year_( xs[6] )
{
  twentyFourHour_ = (0b01000000 & xs[2] ) != 0 ;
  if( twentyFourHour_ )
  {
    pm_ = false ;
  }
  else
  {
    pm_ = ( 0b00100000 & xs[2] ) != 0 ;
  }
  const uint8_t hourBCD = xs[2] & (twentyFourHour_ ? 0b00111111 : 0b00011111 );
  hours_ = BCDNumber( hourBCD ) ;
}

const BCDNumber & RTCData::seconds() const { return seconds_ ; }
const BCDNumber & RTCData::minutes() const{ return minutes_ ; }
const BCDNumber & RTCData::hours() const{ return hours_ ; }

bool RTCData::twentyFourHour() const{ return twentyFourHour_ ; }
bool RTCData::pm() const{ return pm_ ; }
uint8_t RTCData::day() const{ return day_ ; }
const BCDNumber & RTCData::date() const { return date_ ; }
const BCDNumber & RTCData::month() const { return month_ ; }
const BCDNumber & RTCData::year() const { return year_ ; }
