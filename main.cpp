#include <avr/io.h>
#include <util/delay.h>

#include "LCDDisplay.h"
#include "RealTimeClock.h"
#include "RTCData.h"
#include "Buttons.h"
#include "spi.h"

/*void displayTime(const RTCData & td , const Display & disp )
{
  disp.raw(0);
  disp.show(td.hours().tens());
  _delay_ms(500);
  disp.raw(0);
  _delay_ms(200);
  disp.show(td.hours().units());
  _delay_ms(500);
  disp.raw(0);
  _delay_ms(1000);
  disp.raw(0);
  _delay_ms(200);
  disp.show(td.minutes().tens());
  _delay_ms(500);
  disp.raw(0);
  _delay_ms(200);
  disp.show(td.minutes().units());
  _delay_ms(1000);
  disp.raw(0);
}

void do_current_time_show( const Buttons & buttons ,
                           const RealTimeClock & rtc ,
                           const Display & disp ,
                           const Display & debug_disp ,
                           uint8_t value)
{
  //debug_disp.raw(value);
  //_delay_ms(200);
  while( value == 0x00 )
  {
    value = buttons.read();
  }

  if( value != 0x00 )
  {
    if( value == 0x01 )
    {
      disp.show(1);
      debug_disp.raw(rtc.getSecondsByte());
      _delay_ms(1000);
    }
    else if( value == 0x02 )
    {
      disp.show(2);
      debug_disp.raw(rtc.getMinutesByte());
      _delay_ms(1000);
    }
    else if( value == 0x04 )
    {
      disp.show(3);
      debug_disp.raw(rtc.getHoursByte());
      _delay_ms(1000);
    }
    else if( value == 0x08 )
    {
      disp.show(4);
      debug_disp.raw(rtc.getDayByte());
      _delay_ms(1000);
    }
    else if( value == 0x10 )
    {
      disp.show(5);
      debug_disp.raw(rtc.getDateByte());
      _delay_ms(1000);
    }
    else if( value == 0x20 )
    {
      disp.show(6);
      debug_disp.raw(rtc.getMonthByte());
      _delay_ms(1000);
    }
    else if( value == 0x40 )
    {
      disp.show(7);
      debug_disp.raw(rtc.getYearByte());
      _delay_ms(1000);
    }
    else if( value == 0x80 )
    {
      debug_disp.raw(0xff);
      displayTime(RTCData(rtc.read()),disp);
      debug_disp.raw(0x00);
    }
  }
}

uint8_t getManualBinaryValue(const Display & debug_disp,
                       const Buttons & buttons ,
                       const uint8_t start = 0 )
{
  uint8_t value = start ;
  bool done = false ;
  while( !done )
  {
    debug_disp.raw(value);
    const uint8_t x = buttons.read() ;
    if( x == 0x01 )
    {
      value-- ;
      _delay_ms(200);
    }
    else if( x == 0x02 )
    {
      value++ ;
      _delay_ms(200);
    }
    else if( x == 0x80 )
    {
      done = true ;
    }
  }
  return value ;
}

BCDNumber getManualBCDValue(const Display & debug_disp,
                            const Buttons & buttons ,
                            const uint8_t max ,
                            const uint8_t start = 0 )
{
  uint8_t value = start ;
  bool done = false ;
  while( !done )
  {
    debug_disp.raw(byteToBCDNumber( value ).raw());
    const uint8_t x = buttons.read() ;
    if( x == 0x01 )
    {
      value = ( value == 0 ) ? max : value - 1;
      _delay_ms(200);
    }
    else if( x == 0x02 )
    {
      value = ( value == max ) ? 0 : value + 1 ;
      _delay_ms(200);
    }
    else if( x == 0x80 )
    {
      done = true ;
    }
  }
  return byteToBCDNumber( value ) ;
}

void do_debug( const Buttons & buttons ,
               const Display & disp ,
               const Display & debug_disp ,
               uint8_t value )
{
  while(value == 0)
  {
    value = buttons.read();
  }

  if( value == 0x01 )
    {
      for(uint8_t i=0;i<16;++i)
      {
        disp.show(i);
        _delay_ms(200);
      }
      disp.raw(0x00);
    }
    else if( value == 0x02 )
    {
      for(uint8_t i=0;i<16;++i)
      {
        disp.show(15-i);
        _delay_ms(200);
      }
      disp.raw(0x00);
    }
    else if( value == 0x04 )
    {
      uint8_t value = 1;
      while( value != 0 )
      {
        debug_disp.raw(value);
        _delay_ms(500);
        value = value << 1;
      }
      debug_disp.raw(0);
    }
    else if( value == 0x08 )
    {
      //test manual set
      const uint8_t x = getManualBinaryValue( debug_disp , buttons , 0 ) ;
      _delay_ms(1000);
      debug_disp.raw(0);
      _delay_ms(500);
      debug_disp.raw(x);
      _delay_ms(1000);
      debug_disp.raw(0);
    }
    else if( value == 0x10 )
    {
      //test manual set
      const BCDNumber x = getManualBCDValue( debug_disp , buttons , 59 , 0 ) ;
      _delay_ms(1000);
      debug_disp.raw(0);
      _delay_ms(500);
      debug_disp.raw(x.raw());
      _delay_ms(1000);
      debug_disp.raw(0);
    }
}

RTCData & set_time_mode( RTCData & in ,
                         const Buttons & buttons ,
                         const Display & disp )
{
  uint8_t value = buttons.read();
  while( value == 0 )
  {
    value = buttons.read() ;
  }

  if( value == 0x01 )
  {
    //set minutes
    const BCDNumber mins = getManualBCDValue( disp , buttons , 59 , in.minutes().asByte() ) ;
    in.setMinutes( mins.asByte() ) ;
    return in ;
  }

  if( value == 0x02 )
  {
    //set hours
    const BCDNumber hours = in.is24() ?
        getManualBCDValue( disp , buttons , 23 , in.hours().asByte() ) :
        getManualBCDValue( disp , buttons , 12 , in.hours().asByte() ) ;
    in.setHours( hours.asByte() ) ;
    return in ;
  }

  if( value == 0x04 )
  {
    //set 24 hours
    in.set24( true ) ;
    return in ;
  }

  if( value == 0x08 )
  {
    //set 12 hours
    in.set24( false ) ;
    return in ;
  }
  return in ;
}*/

const constexpr static char digit_lookup[] = {'0','1','2','3','4','5','6','7','8','9'};
const constexpr static char day_lookup[][3] = {{'M','o','n'},{'T','u','e'},{'W','e','d'},{'T','h','u'},{'F','r','i'},{'S','a','t'},{'S','u','n'}};
const constexpr static char month_lookup[2][9][3] = {{{'J','a','n'},{'F','e','b'},{'M','a','r'},{'A','p','r'},{'M','a','y'},{'J','u','n'},{'J','u','l'},{'A','u','s'},{'S','e','p'}},
                                                     {{'O','c','t'},{'N','o','v'},{'D','e','c'},{' ',' ',' '},{' ',' ',' '},{' ',' ',' '},{' ',' ',' '},{' ',' ',' '},{' ',' ',' '}}};


static void show_time( const RTCData & time ,
                       const LCDDisplay & disp )
{
  disp.home();
  //hours
  {
    disp.writeChar(digit_lookup[time.hours().tens()]);
    disp.writeChar(digit_lookup[time.hours().units()]);
  }
  disp.writeChar(':');
  //minutes
  {
    disp.writeChar(digit_lookup[time.minutes().tens()]);
    disp.writeChar(digit_lookup[time.minutes().units()]);
  }
  disp.writeChar(':');
  //seconds
  {
    disp.writeChar(digit_lookup[time.seconds().tens()]);
    disp.writeChar(digit_lookup[time.seconds().units()]);
  }
  if( !time.is24() )
  {
    disp.writeChar(' ') ;
    disp.writeChar(time.pm()?'p' : 'a') ;
    disp.writeChar('m') ;
  }
  const constexpr uint8_t chars_per_row = 40 - 8 ;
  for(uint8_t i = 0 ; i < chars_per_row ; ++i)
  {
    disp.writeChar(' ') ;
  }
  //day of the week
  {
    const uint8_t & day = time.day();
    for(uint8_t i = 0 ; i < 3 ; ++i )
    {
      disp.writeChar(day_lookup[day][i]);
    }
  }
  disp.writeChar(' ');
  //date number
  {
    disp.writeChar(digit_lookup[time.date().tens()]);
    disp.writeChar(digit_lookup[time.date().units()]);
  }
  disp.writeChar(' ');
  //month
  {
    const uint8_t & ten = time.month().tens();
    const uint8_t & unit = time.month().units();
    for(uint8_t i = 0 ; i < 3 ; ++i )
    {
      disp.writeChar(month_lookup[ten][unit][i]);
    }
  }
  disp.writeChar(' ');
  //year
  {
    disp.writeChar('2');
    disp.writeChar('0');
    disp.writeChar(digit_lookup[time.year().tens()]);
    disp.writeChar(digit_lookup[time.year().units()]);
  }
}

int main()
{
  //       cccccmmc
  //       eeeeeiol
  //       43210ssk
  //            oi
  DDRB = 0b11111011;
  PORTB = 0b0001000;
  const auto disp = LCDDisplay(&PORTB,5);
  //const auto buttons = Buttons(&PORTB,&PINB,4);
  const auto rtc = RealTimeClock(&PORTB,&PINB,3);

  disp.init();
  disp.clear();
  disp.setCursor(true,false,false);
  while(true)
  {
    show_time( RTCData(rtc.read()) , disp );
  }

  return 0;
}
