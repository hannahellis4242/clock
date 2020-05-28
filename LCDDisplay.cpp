#include "LCDDisplay.h"
#include "spi.h"

#include <util/delay.h>

LCDDisplay::LCDDisplay(volatile uint8_t *port ,
                       const uint8_t ce_pin )
        : spi_port_( port ) ,
          ce_mask_high_( 1 << ce_pin ) ,
          ce_mask_low_( ~ce_mask_high_ ){}

/*wiring for LCD
7 - nc
6 - nc
5 - RS
4 - en
3 - D7
2 - D6
1 - D5
0 - D4
*/

void LCDDisplay::send_spi( const uint8_t x ) const
{
  *spi_port_ = *spi_port_ & ce_mask_low_; //set ce low
  shiftOut(spi_port_,x,true);
  *spi_port_ = *spi_port_ | ce_mask_high_; //set ce high
}

void LCDDisplay::send_instruction(const uint8_t x) const
{
  //upper
  {
    const uint8_t upper = ( x & 0xf0 ) >> 4 ;
    send_spi(upper);
    _delay_ms(10);
    send_spi(upper|0x10);
    _delay_ms(10);
    send_spi(upper);
    _delay_ms(10);
  }
  //lower
  {
    const uint8_t lower = x & 0x0f ;
    send_spi(lower);
    _delay_ms(10);
    send_spi(lower|0x10);
    _delay_ms(10);
    send_spi(lower);
    _delay_ms(10);
  }

}

void LCDDisplay::init() const
{
  //init will ensure that we're in the correct mode on start up
  {
    const constexpr uint8_t a = 0x03 ; //enable low
    const constexpr uint8_t b = 0x13 ; //enable high
    for(uint8_t i = 0 ; i < 3 ; ++i)
    {
      send_spi(a);
      _delay_ms(10);
      send_spi(b);
      _delay_ms(10);
      send_spi(a);
      _delay_ms(10);
    }
  }
  //should now be in 8 pin mode
  {
    const constexpr uint8_t a = 0x02 ; //enable low
    const constexpr uint8_t b = 0x12 ; //enable high
    send_spi(a);
    _delay_ms(10);
    send_spi(b);
    _delay_ms(10);
    send_spi(a);
    _delay_ms(10);
  }
  //should now be in 4 pin mode
  send_instruction(0x28);
}

void LCDDisplay::clear() const
{
  send_instruction(0x01);
}

void LCDDisplay::home() const
{
  send_instruction(0x02);
}

void LCDDisplay::setEntryMode(const bool increment , const bool display_shift) const
{
  send_instruction( 0x04 | (increment ? 0x02 : 0x00 ) | ( display_shift ? 0x01 : 0x00 ) ) ;
}

void LCDDisplay::setCursor(const bool on, const bool cursor, const bool blink) const
{
  send_instruction( 0x08 | (on ? 0x04 : 0x00 ) | ( cursor ? 0x02 : 0x00 ) | ( blink ? 0x01 : 0x00 ) ) ;
}

void LCDDisplay::shiftCursor(const bool cursor,const bool dir)const
{
  send_instruction( 0x10 | (cursor ? 0x08 : 0x00 ) | ( dir ? 0x04 : 0x00 ) ) ;
}
void LCDDisplay::writeChar(const char c)const
{
  //upper
  const uint8_t upper = (c & 0xf0) >> 4 ;
  send_spi(0x20 | upper ) ;
  //_delay_ms(10);
  send_spi(0x30 | upper ) ;
  //_delay_ms(10);
  send_spi(0x20 | upper ) ;
  //_delay_ms(10);
  //lower
  const uint8_t lower = c & 0x0f ;
  send_spi(0x20 | lower ) ;
  //_delay_ms(10);
  send_spi(0x30 | lower ) ;
  //_delay_ms(10);
  send_spi(0x20 | lower ) ;
  //_delay_ms(10);
}

void LCDDisplay::writeStr(const char *string)const
{
  const char *c = string ;
  while( c && *c != '\0' )
  {
    writeChar(*(c++));
  }
}
