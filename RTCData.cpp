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

bool RTCData::is24() const{ return twentyFourHour_ ; }
bool RTCData::pm() const{ return pm_ ; }
uint8_t RTCData::day() const{ return day_ ; }
const BCDNumber & RTCData::date() const { return date_ ; }
const BCDNumber & RTCData::month() const { return month_ ; }
const BCDNumber & RTCData::year() const { return year_ ; }

void RTCData::setSeconds(const uint8_t x ) const { seconds_ = byteToBCDNumber( x ) ;}
void RTCData::setMinutes(const uint8_t x ) const { minutes_ = byteToBCDNumber( x ) ;}
void RTCData::setHours(const uint8_t x) const { hours_ = byteToBCDNumber(x) ;}
void RTCData::setPm(const bool x) const { pm_ = x ; }
void RTCData::set24(const bool x) const { twentyFourHour_ = x ;}
void RTCData::setDay(const uint8_t x)const{ day_ = x ; }
void RTCData::setDate(const uint8_t x)const { date_ = byteToBCDNumber( x ) ;}
void RTCData::setMonth(const uint8_t x)const { month_ = byteToBCDNumber( x ) ; }
void RTCData::setYear(const uint8_t x)const { year_ = byteToBCDNumber( x ) ; }
