#pragma once

#include <string>

namespace Mcal
{
    namespace Gpio
    {
        class Gpio
        {
        private:
            int fdExport;
            int fdDir;
            int fdVal;
            std::string pathExport;
            std::string pathDir;
            std::string pathVal;
            std::string direction;
            int num;
            int value;

        public:
            Gpio();
            Gpio(int pin);
            Gpio(int pin, std::string direction);
            ~Gpio();
            int Gpio_read(int pin);
            void Gpio_setDirection(std::string direction);
            void Gpio_write(int value);
            Gpio& operator~();
        };
    }
}