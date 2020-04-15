#include <avr/io.h>
#include <util/delay.h>

#include "Display.h"
#include "RealTimeClock.h"
#include "Buttons.h"

#include "spi.h"

void displayTime(const RTCData & td , const Display & disp )
{
  disp.raw(0);
  disp.show(td.hours().tens());
  _delay_ms(500);
  disp.raw(0);
  disp.show(td.hours().units());
  _delay_ms(500);
  disp.raw(0);
  _delay_ms(1000);
  disp.raw(0);
  disp.show(td.minutes().tens());
  _delay_ms(500);
  disp.raw(0);
  disp.show(td.minutes().units());
  _delay_ms(1000);
  disp.raw(0);
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
  disp.raw(0x00);
  while(true)
  {
    const uint8_t value = buttons.read() ;
    if( value == 0x80 )
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
      displayTime(rtc.read(),disp);
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
  }
  return 0;
}
