#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

//Wrapper arround a fixed size array to make it easier to move around
template < class T , uint8_t N >
class Array
{
private:
  T data_[N] ;
public:
  Array()
  {
    for(int i = 0 ; i < N ; ++i)
    {
      data_[i] = T() ;
    }
  }
  Array(const T (&data)[N])
  {
    //for some reason we need to manually copy this over.
    for(int i = 0 ; i < N ; ++i)
    {
      data_[i] = data[i] ;
    }
  }

  uint8_t size() const { return N ; }
  const T & operator[](const uint8_t index) const
  {
    return data_[index];
  }
  T & operator[](const uint8_t index)
  {
    return data_[index];
  }
};

#endif
