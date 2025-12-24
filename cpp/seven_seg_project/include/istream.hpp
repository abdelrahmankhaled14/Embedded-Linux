#pragma once
#include <iostream>
#include "stream.hpp"
class istream : virtual public stream
{
private:
public:
    istream();
    virtual ~istream();
    int read();
};