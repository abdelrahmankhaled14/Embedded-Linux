#pragma once
#include <iostream>
class stream
{
private:

public:
    stream();
    virtual ~stream();
    virtual int read() = 0;
    virtual void write(int number)= 0;

};
