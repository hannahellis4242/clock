#include "RTCData.h"


RTCData::RTCData(const uint8_t (&xs)[7])
: RTCData( Array< uint8_t , 7 >( xs ) ){}

RTCData::RTCData(const Array<uint8_t,7> & xs)
: seconds_( xs[0] ) ,
  minutes_( xs[1] ) ,
  day_( xs[3] ) ,
  date_( xs[4] ) ,
  month_( xs[5] ),
  year_( xs[6] )
{
  twentyFourHour_ = (0x40 & xs[2] ) == 0 ;
  if( twentyFourHour_ )
  {
    pm_ = false ;
  }
  else
  {
    pm_ = ( 0x20 & xs[2] ) != 0 ;
  }
  const uint8_t hourBCD = xs[2] & (twentyFourHour_ ? 0x3f : 0x1f );
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

void RTCData::setSeconds(const uint8_t x )  { seconds_ = byteToBCDNumber( x ) ;}
void RTCData::setMinutes(const uint8_t x )  { minutes_ = byteToBCDNumber( x ) ;}
void RTCData::setHours(const uint8_t x)  { hours_ = byteToBCDNumber(x) ;}
void RTCData::setPm(const bool x)  { pm_ = x ; }
void RTCData::set24(const bool x)  { twentyFourHour_ = x ;}
void RTCData::setDay(const uint8_t x){ day_ = x ; }
void RTCData::setDate(const uint8_t x) { date_ = byteToBCDNumber( x ) ;}
void RTCData::setMonth(const uint8_t x) { month_ = byteToBCDNumber( x ) ; }
void RTCData::setYear(const uint8_t x) { year_ = byteToBCDNumber( x ) ; }

Array< uint8_t , 7 > RTCData::convertToArray() const
{
  Array< uint8_t , 7 > out ;
  out[0] = seconds_.raw() ;
  out[1] = minutes_.raw() ;
  //hours requires a little more work
  {
    uint8_t hour = hours_.raw();
    if( twentyFourHour_ )
    {
      //in 24 hour mode, so we need to set the 6th bit low
      hour = hour & 0b10111111 ;
    }
    else
    {
      //in 12 hour mode, so we need to set the 6th bit high
      hour = hour | 0b01000000 ;
      if( pm_ )
      {
        //if pm set the 5th bit high
        hour = hour | 0b00100000 ;
      }
      else
      {
        //if it's am set the 5th bit low
        hour = hour & 0b11011111 ;
      }
    }
  }
  out[3] = day_ ;
  out[4] = date_.raw() ;
  out[5] = month_.raw() ;
  out[6] = year_.raw() ;

  return out ;
}
