
#include "gpio.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "sevensegment.hpp"
int  map[10] =
{
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};
sevensegment::sevensegment()
{
    std::cout << "Seven Segment Display Initialized" << std::endl;
    segA = Mcal::Gpio::Gpio(2, "out");
    segB = Mcal::Gpio::Gpio(3, "out");
    segC = Mcal::Gpio::Gpio(4, "out");
    segD = Mcal::Gpio::Gpio(17, "out");
    segE = Mcal::Gpio::Gpio(27, "out");
    segF = Mcal::Gpio::Gpio(22, "out");
    segG = Mcal::Gpio::Gpio(10, "out");
    segDp = Mcal::Gpio::Gpio(9, "out");
}

void sevensegment::write(int number)
{
    std::cout << "Displaying number " << number << " on Seven Segment Display" << std::endl;
    if (number < 0 || number > 9)
    {
        std::cout << "Error: Number out of range (0-9)" << std::endl;
        return;
    }
    int segments = map[number];
    for(int i = 0; i < 8; i++)
    {
        int bit = (segments >> i) & 0x01;
        switch(i)
        {
            case 0: segA.Gpio_write(bit); break;
            case 1: segB.Gpio_write(bit); break;
            case 2: segC.Gpio_write(bit); break;
            case 3: segD.Gpio_write(bit); break;
            case 4: segE.Gpio_write(bit); break;
            case 5: segF.Gpio_write(bit); break;
            case 6: segG.Gpio_write(bit); break;
            case 7: segDp.Gpio_write(bit); break;
        }
    }
}
;