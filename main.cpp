#include <avr/io.h>
#include <util/delay.h>

static const uint8_t numberLookup[] = {
  //pgfedcba
  0b00111111, //0
  0b00000110, //1
  0b01011011, //2
  0b01001111, //3
  0b01100110, //4
  0b01101101, //5
  0b01111101, //6
  0b00000111, //7
  0b01111111, //8
  0b01100111, //9
  0b01110111, //A
  0b01111100, //b
  0b00111001, //C
  0b01011110, //d
  0b01111001, //E
  0b01110001, //F
};

/*class Display
{
private:
  const volatile uint8_t * port_;
  uint8_t mosi_;
  uint8_t clk_;
  uint8_t ce_;
public:
  Display(const volatile uint8_t * port ,
          const uint8_t mosi ,
          const uint8_t clk
          const ce )
          : port_(port),
            mosi_(mosi),
            clk_(clk),
            ce_(ce){}

  void raw(const uint8_t &x) const
  {

  }
};*/

//Some fixed rules about an spi port
//We use just the lower nibble for data.
//clk is pin 0 of the port
//mosi is pin 1 of the port
//miso is pin 2 of the port

uint8_t bit_shift_up_one( const uint8_t x )
{
 return x << 1 ;
};

uint8_t bit_shift_down_one( const uint8_t x )
{
 return x >> 1 ;
};

//#define DEBUG_SHIFT_OUT 1
#ifdef DEBUG_SHIFT_OUT
#define DEBUG_DELAY 1000
#endif

static void shiftOut( volatile uint8_t *port,
                      const uint8_t data ,
                      const bool msb_first = true )
{
  uint8_t mask = msb_first ? 0b10000000 : 0b00000001 ;
  uint8_t (*bitShiftFn)(const uint8_t) = msb_first ? bit_shift_down_one : bit_shift_up_one ;
  while( mask )
  {
    *port = ( ( mask & data ) != 0 ) ? ( *port | 0b00000010 ) : ( *port & 0b11111101 ); //set mosi high or low
    #ifdef DEBUG_SHIFT_OUT
    _delay_ms(DEBUG_DELAY);
    #endif
    *port = *port | 0b00000001 ; // set clock high
    #ifdef DEBUG_SHIFT_OUT
    _delay_ms(DEBUG_DELAY);
    #endif
    *port = *port & 0b11111110 ; //set clock low
    #ifdef DEBUG_SHIFT_OUT
    _delay_ms(DEBUG_DELAY);
    #endif
    mask = bitShiftFn( mask ) ; //move mask along
  }
}

static uint8_t shiftIn(volatile uint8_t *port,
                       const volatile uint8_t *port_in ,
                       const bool msb_first = true )
{
  uint8_t mask = msb_first ? 0b10000000 : 0b00000001 ;
  uint8_t (*bitShiftFn)(uint8_t) = msb_first ? bit_shift_down_one : bit_shift_up_one ;
  uint8_t x = 0 ;
  while( mask )
  {
    x = ( ( 0b00000100 & *port_in ) != 0 ) ? ( x | mask ) : x ; //set mosi high or low
    *port = *port | 0b00000001 ; // set clock high
    *port = *port & 0b11111110 ; //set clock low
    mask = bitShiftFn( mask ) ; //move mask along
  }
  return x ;
}

static void show(const uint8_t value)
{
  PORTB = PORTB & 0b11110111; //set ce low
  shiftOut(&PORTB,numberLookup[value],true);
  PORTB = PORTB | 0b00001000; //set ce high
}

static uint8_t readButtons()
{
  //to enable the chip drop chip enable low
  PORTB = PORTB & 0b11101111; //set ce low
  //to store the current state of the pins, send mosi low then clock once
  PORTB = PORTB & 0b11101101; //set mosi low
  //flash clock up to load the memory
  PORTB = PORTB | 0b00000001 ; // set clock high
  PORTB = PORTB & 0b11111110 ; // set clock low
  //data is now loaded for shifing out
  //put into shift out mode
  PORTB = PORTB | 0b00000010 ; //set mosi high
  //now shift in
  const uint8_t x = shiftIn(&PORTB,&PINB);
  //stop
  PORTB = PORTB | 0b00010000;
  return x ;
}

int main()
{
  //       cccccmmc
  //       eeeeeiol
  //       43210ssk
  //            oi
  DDRB = 0b11111011;

  PORTB = 0 ;
  while(true)
  {
    show(readButtons() % 16);
    _delay_ms(1000);
  }
  return 0;
}
