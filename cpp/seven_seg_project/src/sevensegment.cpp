
#include "gpio.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "sevensegment.hpp"
int map[10] =
{
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};
sevensegment::sevensegment()
    : segA(2, "out"),
      segB(3, "out"),
      segC(4, "out"),
      segD(17, "out"),
      segE(27, "out"),
      segF(22, "out"),
      segG(10, "out"),
      segDp(9, "out")
{
    std::cout << "Seven Segment Display Initialized" << std::endl;
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