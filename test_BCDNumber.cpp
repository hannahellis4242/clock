#include "BCDNumber.h"

#include <iostream>
#include <sstream>

void test1()
{
  std::stringstream ss ;
  ss << std::boolalpha ;
  ss << std::string( 10 , '-' ) << __FUNCTION__ << std::string( 10 , '-' ) << std::endl ;

  ss << std::string( 10 , '=' ) << __FUNCTION__ << std::string( 10 , '=' ) << std::endl ;
  std::cout << ss.str() ;
}

int main()
{
  test1();
}
