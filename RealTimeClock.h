#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

#include "Array.h"

class RealTimeClock
{
private:
  volatile uint8_t * out_port_ ;
  const volatile uint8_t * in_port_ ;
  uint8_t ce_mask_high_ ;
  uint8_t ce_mask_low_ ;

public:
  RealTimeClock(volatile uint8_t * out_port , const volatile uint8_t * in_port , const uint8_t ce_pin ) ;

  Array< uint8_t , 7 > read() const;
  void write( const Array< uint8_t , 7 > & data ) const;

  uint8_t getControlRegister()const ;

  uint8_t getSecondsByte() const;
  uint8_t getMinutesByte() const;
  uint8_t getHoursByte() const;
  uint8_t getDayByte() const;
  uint8_t getDateByte() const;
  uint8_t getMonthByte() const;
  uint8_t getYearByte() const;

  void setControlRegister( const uint8_t value )const;
};

#endif
