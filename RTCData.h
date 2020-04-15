#ifndef RTCDATA_H
#define RTCDATA_H

#include <stdint.h>
#include "BCDNumber.h"

class RTCData
{
private:
    BCDNumber seconds_ ;
    BCDNumber minutes_ ;
    BCDNumber hours_ ;
    bool twentyFourHour_ ;
    bool pm_ ;
    uint8_t day_ ;
    BCDNumber date_ ;
    BCDNumber month_ ;
    BCDNumber year_ ;
  public:
    RTCData(const uint8_t (&xs)[7]);

    const BCDNumber & seconds() const;
    const BCDNumber & minutes() const;
    const BCDNumber & hours() const;

    bool twentyFourHour() const;
    bool pm() const;
    uint8_t day() const;
    const BCDNumber & date() const ;
    const BCDNumber & month() const ;
    const BCDNumber & year() const ;
};
#endif
