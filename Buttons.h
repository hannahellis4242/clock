#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

class Buttons
{
private:
  volatile uint8_t * out_port_ ;
  const volatile uint8_t * in_port_ ;
  uint8_t ce_mask_high_ ;
  uint8_t ce_mask_low_ ;

public:
  Buttons(volatile uint8_t * out_port , const volatile uint8_t * in_port , const uint8_t ce_pin );
  uint8_t read() const;
};

#endif
