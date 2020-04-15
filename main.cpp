#include <avr/io.h>
#include <util/delay.h>

#include "Display.h"
#include "spi.h"
#include "BCDNumber.h"

class RTCData
{
  public:
    BCDNumber seconds ;
    BCDNumber minutes ;
    BCDNumber hours ;
    uint8_t hoursRaw ;
    bool twentyFourHour ;
    bool pm ;
    uint8_t day ;
    BCDNumber date ;
    BCDNumber month ;
    BCDNumber year ;

    RTCData(const uint8_t (&xs)[7])
    : seconds( xs[0] ) ,
      minutes( xs[1] ) ,
      hoursRaw( xs[2] ),
      day( xs[3] ) ,
      date( xs[4] ) ,
      month( xs[5] ),
      year( xs[6] )
    {
      twentyFourHour = (0b01000000 & xs[2] ) != 0 ;
      if( twentyFourHour )
      {
        pm = false ;
      }
      else
      {
        pm = ( 0b00100000 & xs[2] ) != 0 ;
      }
      const uint8_t hourBCD = xs[2] & (twentyFourHour ? 0b00111111 : 0b00011111 );
      hours = BCDNumber( hourBCD ) ;
    }
};

class RealTimeClock
{
private:
  volatile uint8_t * out_port_ ;
  const volatile uint8_t * in_port_ ;
  uint8_t ce_mask_high_ ;
  uint8_t ce_mask_low_ ;

public:
  RealTimeClock(volatile uint8_t * out_port , const volatile uint8_t * in_port , const uint8_t ce_pin )
  : out_port_( out_port ) , in_port_(in_port) , ce_mask_high_( 1 << ce_pin ) , ce_mask_low_( ~ce_mask_high_ ){}

  RTCData read() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0b00000000,true);
    uint8_t xs[7]={0,0,0,0,0,0,0};
    for(unsigned int i = 0 ; i < 7 ; ++i)
    {
      xs[i] = shiftIn(out_port_ , in_port_ , true) ;
    }
    //stop set ce low
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return RTCData( xs ) ;
  }

  uint8_t read_seconds_byte() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0x00,true);
    const uint8_t seconds = shiftIn(out_port_ , in_port_ , true) ;
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return seconds ;
  }

  uint8_t read_minutes_byte() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0x01,true);
    const uint8_t minutes = shiftIn(out_port_ , in_port_ , true) ;
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return minutes ;
  }

  uint8_t read_hours_byte() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0x02,true);
    const uint8_t hours = shiftIn(out_port_ , in_port_ , true) ;
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return hours ;
  }

  uint8_t read_day_byte() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0x03,true);
    const uint8_t day = shiftIn(out_port_ , in_port_ , true) ;
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return day ;
  }

  uint8_t read_date_byte() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0x04,true);
    const uint8_t date = shiftIn(out_port_ , in_port_ , true) ;
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return date ;
  }

  uint8_t read_month_byte() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0x05,true);
    const uint8_t month = shiftIn(out_port_ , in_port_ , true) ;
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return month ;
  }

  uint8_t read_year_byte() const
  {
    //start set ce high
    *out_port_ = *out_port_ | ce_mask_high_ ;
    //write out instruction, in this case read
    shiftOut(out_port_,0x06,true);
    const uint8_t year = shiftIn(out_port_ , in_port_ , true) ;
    *out_port_ = *out_port_ & ce_mask_low_ ;
    return year ;
  }
};

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
  disp.show(td.hours.tens());
  _delay_ms(500);
  disp.raw(0);
  disp.show(td.hours.units());
  _delay_ms(500);
  disp.raw(0);
  _delay_ms(1000);
  disp.raw(0);
  disp.show(td.minutes.tens());
  _delay_ms(500);
  disp.raw(0);
  disp.show(td.minutes.units());
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
    /*else if( value == 0x04 )
    {
      uint8_t value = 0;
      do
      {
        debug_disp(value,&PORTB,6);
        _delay_ms(100);
        value++;
      }while( value != 0 );
      debug_disp(0,&PORTB,6);
    }*/
    else if( value == 0x04 )
    {
      debug_disp(rtc.read_seconds_byte(),&PORTB,6);
      //_delay_ms(2000);
      debug_disp(0,&PORTB,6);
    }
    else if( value == 0x08 )
    {
      debug_disp(rtc.read_minutes_byte(),&PORTB,6);
      //_delay_ms(2000);
      debug_disp(0,&PORTB,6);
    }
    else if( value == 0x10 )
    {
      debug_disp(rtc.read_hours_byte(),&PORTB,6);
      //_delay_ms(2000);
      debug_disp(0,&PORTB,6);
    }
  }
  return 0;
}
