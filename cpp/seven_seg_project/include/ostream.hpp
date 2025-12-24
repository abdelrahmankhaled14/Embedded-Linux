#pragma once
#include "stream.hpp"
#include <iostream>
class ostream : virtual public stream
{
private:
public:
    ostream();
    virtual ~ostream() ;
    virtual void write(int number)= 0;

};
