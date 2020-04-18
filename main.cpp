#include <avr/io.h>
#include <util/delay.h>

#include "Display.h"
#include "RealTimeClock.h"
#include "RTCData.h"
#include "Buttons.h"
#include "spi.h"

void displayTime(const RTCData & td , const Display & disp )
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
  /*else
  {
    do_current_time_show(buttons,rtc,disp,debug_disp,buttons.read());
  }*/
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
}

int main()
{
  //       cccccmmc
  //       eeeeeiol
  //       43210ssk
  //            oi
  DDRB = 0b11111011;
  PORTB = 0b0010000;
  const auto disp = Display(&PORTB,3);
  const auto buttons = Buttons(&PORTB,&PINB,4);
  const auto rtc = RealTimeClock(&PORTB,&PINB,5);
  const auto debug_disp = Display(&PORTB,6);

  RTCData time ;

  disp.raw(0x00);
  while(true)
  {
    const uint8_t value = buttons.read() ;
    if( value == 0x01 )
    {
      //debug mode
      disp.show(0);
      _delay_ms(1000); //wait a second so the user has time to press which debug option they want
      do_debug( buttons , disp , debug_disp , buttons.read() ) ;
    }
    else if( value == 0x80 )
    {
      //show current time mode
      disp.show(1);
      _delay_ms(1000); //wait a second so the user has time to press wihch option they want
      do_current_time_show(buttons,rtc,disp,debug_disp,buttons.read());
    }
    else if( value == 0x40 )
    {
      disp.show(2);
      //in set data mode
      time = RTCData( rtc.read() ) ;//grab the current time
      time.setSeconds(0); //reset the seconds to zero
      _delay_ms(1000); //give user time to select an option
      time = set_time_mode( time , buttons , debug_disp ) ;
      _delay_ms(1000); //wait a second before allowing more input
    }
    else if( value == 0x20 )
    {
      disp.show(3);
      displayTime(time,disp);
      _delay_ms(1000);
      const auto dat = time.convertToArray() ;
      for( uint8_t i = 0 ; i < dat.size() ; ++i )
      {
        debug_disp.raw(dat[i]);
        _delay_ms(1000);
        debug_disp.raw(0xff);
        _delay_ms(500);
      }
      debug_disp.raw(0xff);
      _delay_ms(1000);

    }
    else if( value == 0x10 )
    {
      disp.show(4);
      debug_disp.raw(0xff);
      rtc.write(time.convertToArray());
      debug_disp.raw(0x00);
    }

    disp.raw(0);
    debug_disp.raw(0);
    /*if( value == 0x80 )
    {
      for(uint8_t i=0;i<16;++i)
      {
        disp.show(i);
        _delay_ms(200);
      }
      disp.raw(0x00);
    }
    else if( value == 0x40 )
    {
      for(uint8_t i=0;i<16;++i)
      {
        disp.show(15-i);
        _delay_ms(200);
      }
      disp.raw(0x00);
    }
    else if( value == 0x01 )
    {
      displayTime(RTCData(rtc.read()),disp);
      disp.raw(0x00);
    }
    else if( value == 0x02 )
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
    else if( value == 0x04 )
    {
      debug_disp.raw(rtc.getSecondsByte());
      debug_disp.raw(0);
    }
    else if( value == 0x08 )
    {
      debug_disp.raw(rtc.getMinutesByte());
      debug_disp.raw(0);
    }
    else if( value == 0x10 )
    {
      debug_disp.raw(rtc.getHoursByte());
      debug_disp.raw(0);
    }
    else if( value == 0x20 )
    {
      rtc.write( Array< uint8_t , 7 >() ) ;
      _delay_ms(1000);
      debug_disp.raw(0xff);
      _delay_ms(100);
      debug_disp.raw(0x00);
    }
    else if( value == 0x40 )
    {
      debug_disp.raw(rtc.getControlRegister());
      debug_disp.raw(0);
    }*/
  }
  return 0;
}
