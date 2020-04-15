#ifndef SPI_H
#define SPI_H

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
#endif
