#ifndef SPI_H
#define SPI_H

#include <stdint.h>

//Some fixed rules about an spi port
//We use just the lower nibble for data.
//clk is pin 0 of the port
//mosi is pin 1 of the port
//miso is pin 2 of the port

void shiftOut( volatile uint8_t *port,
               const uint8_t data ,
               const bool msb_first = true ) ;

uint8_t shiftIn(volatile uint8_t *port,
                const volatile uint8_t *port_in ,
                const bool msb_first = true ) ;

static constexpr const uint8_t clk_high = 0x02;
static constexpr const uint8_t clk_low = ~clk_high ;

static constexpr const uint8_t mosi_high = 0x02;
static constexpr const uint8_t mosi_low = ~mosi_high ;

static constexpr const uint8_t miso_high = 0x04;
static constexpr const uint8_t miso_low = ~miso_high ;

#endif
