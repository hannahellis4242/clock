#ifndef RTCDATA_H
#define RTCDATA_H

#include <stdint.h>
#include "BCDNumber.h"
#include "Array.h"

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
    RTCData() = default ;
    RTCData(const uint8_t (&xs)[7]);
    RTCData(const Array<uint8_t,7> & xs);

    const BCDNumber & seconds() const;
    const BCDNumber & minutes() const;
    const BCDNumber & hours() const;

    bool is24() const;
    bool pm() const;
    uint8_t day() const;
    const BCDNumber & date() const ;
    const BCDNumber & month() const ;
    const BCDNumber & year() const ;

    void setSeconds(const uint8_t) ;
    void setMinutes(const uint8_t) ;
    void setHours(const uint8_t) ;
    void setPm(const bool) ;
    void set24(const bool);
    void setDay(const uint8_t);
    void setDate(const uint8_t);
    void setMonth(const uint8_t);
    void setYear(const uint8_t);

    Array< uint8_t , 7 > convertToArray() const ;
};
#endif
