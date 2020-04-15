#include <avr/io.h>
#include <util/delay.h>

#include "Display.h"
#include "spi.h"
#include "RealTimeClock.h"

class Buttons
{
private:
  volatile uint8_t * out_port_ ;
  const volatile uint8_t * in_port_ ;
  uint8_t ce_mask_high_ ;
  uint8_t ce_mask_low_ ;

public:
  Buttons(volatile uint8_t * out_port , const volatile uint8_t * in_port , const uint8_t ce_pin )
  : out_port_( out_port ) , in_port_(in_port) , ce_mask_high_( 1 << ce_pin ) , ce_mask_low_( ~ce_mask_high_ ){}

  uint8_t read() const
  {
    //to enable the chip drop chip enable low
    *out_port_ = *out_port_ & ce_mask_low_; //set ce low
    //to store the current state of the pins, send mosi low then clock once
    *out_port_ = *out_port_ & 0b11101101; //set mosi low
    //flash clock up to load the memory
    *out_port_ = *out_port_ | 0b00000001 ; // set clock high
    *out_port_ = *out_port_ & 0b11111110 ; // set clock low
    //data is now loaded for shifing out
    //put into shift out mode
    *out_port_ = *out_port_ | 0b00000010 ; //set mosi high
    //now shift in
    const uint8_t x = shiftIn(out_port_,in_port_);
    //stop
    *out_port_ = *out_port_ | ce_mask_high_;
    return x ;
  }
};

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

void debug_disp(const uint8_t data ,volatile uint8_t * port,const uint8_t ce_pin)
{
  const uint8_t mask_high = 1 << ce_pin ;
  const uint8_t mask_low = ~mask_high ;

  *port = *port & mask_low ; //ce_low
  shiftOut(port,data,true);
  *port = *port | mask_high ; //ce high
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
      do
      {
        debug_disp(value,&PORTB,6);
        _delay_ms(500);
        value = value << 1;
      }while( value != 0 );
      debug_disp(0,&PORTB,6);
    }
    else if( value == 0x04 )
    {
      debug_disp(rtc.getSecondsByte(),&PORTB,6);
      //_delay_ms(2000);
      debug_disp(0,&PORTB,6);
    }
    else if( value == 0x08 )
    {
      debug_disp(rtc.getMinutesByte(),&PORTB,6);
      //_delay_ms(2000);
      debug_disp(0,&PORTB,6);
    }
    else if( value == 0x10 )
    {
      debug_disp(rtc.getHoursByte(),&PORTB,6);
      //_delay_ms(2000);
      debug_disp(0,&PORTB,6);
    }
  }
  return 0;
}
