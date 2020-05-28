#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <stdint.h>

class LCDDisplay
{
private:
  volatile uint8_t * spi_port_ ;
  uint8_t ce_mask_high_ ;
  uint8_t ce_mask_low_ ;

  void send_spi( const uint8_t x ) const ;
  void send_instruction( const uint8_t x) const ;
public:
  LCDDisplay(volatile uint8_t * out_port , const uint8_t ce_pin );
  void init() const;
  void clear() const;
  void home() const;
  void setEntryMode(const bool increment , const bool display_shift) const;
  void setCursor(const bool on, const bool cursor, const bool blink) const;
  void shiftCursor(const bool cursor,const bool dir)const;
  void writeChar(const char c)const ;
  void writeStr(const char *string)const;
};

#endif
