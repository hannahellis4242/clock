#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

#include "RTCData.h"

class RealTimeClock
{
private:
  volatile uint8_t * out_port_ ;
  const volatile uint8_t * in_port_ ;
  uint8_t ce_mask_high_ ;
  uint8_t ce_mask_low_ ;

public:
  RealTimeClock(volatile uint8_t * out_port , const volatile uint8_t * in_port , const uint8_t ce_pin ) ;

  RTCData read() const;

  uint8_t getSecondsByte() const;
  uint8_t getMinutesByte() const;
  uint8_t getHoursByte() const;
  uint8_t getDayByte() const;
  uint8_t getDateByte() const;
  uint8_t getMonthByte() const;
  uint8_t getYearByte() const;
};

#endif
