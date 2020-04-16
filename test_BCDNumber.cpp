#include "BCDNumber.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

class Check
{
public:
  virtual ~Check(){}

  virtual void SetUp() {} ;
  virtual bool check() const = 0 ;
  virtual void TearDown() {} ;

  bool run()
  {
    SetUp();
    if( check() )
    {
      std::cout << "\tPASS" << std::endl ;
      return true ;
    }
    else
    {
      std::cout << "\tFAIL" << std::endl ;
      return false ;
    }
  }

};

class CheckBCDNumber : public Check
{
private:
  const uint8_t in ;
  const uint8_t tens ;
  const uint8_t units ;
  const uint8_t value ;
public:

CheckBCDNumber(const uint8_t in_ ,
               const uint8_t tens_ ,
               const uint8_t units_ ,
               const uint8_t value_ )
              : in( in_ ) , tens( tens_ ) , units( units_ ) , value( value_ ){}

bool check() const final
{
  const auto x = BCDNumber( in ) ;
  if( x.tens() != tens )
  {
    std::cout << "expected tens to be : " << static_cast< unsigned int >( tens ) << std::endl ;
    std::cout << "             actual : " << static_cast< unsigned int >( x.tens() ) << std::endl ;
    return false ;
  }
  if( x.units() != units )
  {
    std::cout << "expected units to be : " << static_cast< unsigned int >( units ) << std::endl ;
    std::cout << "              actual : " << static_cast< unsigned int >( x.units() ) << std::endl ;
    return false ;
  }
  if( x.asByte() != value )
  {
    std::cout << "expected value to be : " << static_cast< unsigned int >( value ) << std::endl ;
    std::cout << "              actual : " << static_cast< unsigned int >( x.asByte() ) << std::endl ;
    return false ;
  }
  return true ;
}
};

bool checkFails( const std::shared_ptr<Check> check )
{
  return !check->run() ;
}

void run_bank( const std::vector<std::shared_ptr<Check>> & checks )
{
  if( std::none_of( checks.begin() , checks.end() , checkFails ) )
  {
    std::cout << "All PASS" << std::endl ;
  }
  else
  {
    std::cout << "FAIL" << std::endl ;
  }
}

int main()
{
  const std::vector<std::shared_ptr<Check>> checks = {
    std::shared_ptr< Check >( new CheckBCDNumber( 0 , 0 , 0 , 0 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 1 , 0 , 1 , 1 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 2 , 0 , 2 , 2 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 3 , 0 , 3 , 3 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 4 , 0 , 4 , 4 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 5 , 0 , 5 , 5 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 6 , 0 , 6 , 6 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 7 , 0 , 7 , 7 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 8 , 0 , 8 , 8 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 9 , 0 , 9 , 9 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x10 , 1 , 0 , 10 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x11 , 1 , 1 , 11 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x12 , 1 , 2 , 12 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x13 , 1 , 3 , 13 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x14 , 1 , 4 , 14 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x15 , 1 , 5 , 15 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x27 , 2 , 7 , 27 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x30 , 3 , 0 , 30 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x39 , 3 , 9 , 39 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x46 , 4 , 6 , 46 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x59 , 5 , 9 , 59 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x62 , 6 , 2 , 62 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x76 , 7 , 6 , 76 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x83 , 8 , 3 , 83 ) ) ,
    std::shared_ptr< Check >( new CheckBCDNumber( 0x99 , 9 , 9 , 99 ) ) ,
   };
  run_bank( checks ) ;
}
