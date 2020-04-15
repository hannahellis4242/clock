#include <avr/io.h>
#include <util/delay.h>

#include "Display.h"
#include "spi.h"

class BCDNumber
{
  private:
    uint8_t data_ ;
  public:
    BCDNumber( const uint8_t & data = 0 )
    : data_( data ) {}

    const uint8_t & raw() const
    {
      return data_ ;
    }

    uint8_t units() const
    {
      return data_ & 0b00001111 ;
    }

    uint8_t tens() const
    {
      return ( data_ & 0b11110000 ) >> 4 ;
    }

    uint8_t asByte() const
    {
      return units() + 10 * tens() ;
    }
};

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
    if( value == 0b10000000 )
    {
      for(uint8_t i=0;i<16;++i)
      {
        disp.show(i);
        _delay_ms(200);
      }
      disp.raw(0x00);
    }
    else if( value == 0b01000000 )
    {
      for(uint8_t i=0;i<16;++i)
      {
        disp.show(15-i);
        _delay_ms(200);
      }
      disp.raw(0x00);
    }
    else if( value == 0b00000001 )
    {
      displayTime(rtc.read(),disp);
      disp.raw(0x00);
    }
  }
  return 0;
}
