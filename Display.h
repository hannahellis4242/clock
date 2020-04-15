#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

class Display
{
private:
  volatile uint8_t * spi_port_ ;
  uint8_t ce_mask_high_ ;
  uint8_t ce_mask_low_ ;
public:
  Display(volatile uint8_t *port , const uint8_t ce_pin );

  void raw(const uint8_t value) const;
  void show(const uint8_t value) const;
};
#endif
