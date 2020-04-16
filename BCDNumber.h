#ifndef BCDNUMBER_H
#define BCDNUMBER_H

#include <stdint.h>

class BCDNumber
{
  private:
    uint8_t data_ ;
  public:
    BCDNumber( const uint8_t & data = 0 );

    const uint8_t & raw() const;
    uint8_t units() const;
    uint8_t tens() const;
    uint8_t asByte() const;
};

BCDNumber byteToBCDNumber(const uint8_t);
#endif
