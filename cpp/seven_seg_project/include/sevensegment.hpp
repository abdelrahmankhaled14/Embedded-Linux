#pragma once
#include <iostream>
#include "stream.hpp"
#include "istream.hpp"
#include "ostream.hpp"
#include "gpio.hpp"
// cppcheck-suppress syntaxError
class sevensegment: virtual public istream, virtual public ostream 
{
private:    
    Mcal::Gpio::Gpio segA;
    Mcal::Gpio::Gpio segB;
    Mcal::Gpio::Gpio segC;
    Mcal::Gpio::Gpio segD;
    Mcal::Gpio::Gpio segE;
    Mcal::Gpio::Gpio segF;
    Mcal::Gpio::Gpio segG;
    Mcal::Gpio::Gpio segDp;

public:
    sevensegment();
    ~sevensegment() = default;
    void write(int number);
};