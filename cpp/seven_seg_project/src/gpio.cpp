#include "gpio.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

namespace Mcal
{
    namespace Gpio
    {

        Gpio::Gpio()
        {
            std::cout <<"gpio coonstructor"<< std::endl;
        }
        Gpio::Gpio(int pin)
        {
            this->num = pin + 512;
            std::string num_str = std::to_string(num);
            pathExport = "/sys/class/gpio/export";
            fdExport = open(pathExport.c_str(), O_WRONLY);
            if (fdExport < 0)
            {
                perror("failed to open file");
                return;
            }
            write(fdExport, num_str.c_str(), num_str.size());
            close(fdExport);
            sleep(1);
            pathDir = "/sys/class/gpio/gpio" + std::to_string(num) + "/direction";
            fdDir = open(pathDir.c_str(), O_WRONLY);
            if (fdDir < 0)
            {
                perror("failed to open gpio direction");
                return;
            }
            pathVal = "/sys/class/gpio/gpio" + std::to_string(num) + "/value";
            fdVal = open(pathVal.c_str(), O_RDWR);
            if (fdVal < 0)
            {
                perror("failed to open gpio value");
                return;
            }
        }
        Gpio::Gpio(int pin, std::string direction)
        {
            std::cout << "Initializing GPIO " << pin << " as " << direction << std::endl;
            this->num = pin + 512;
            std::string num_str = std::to_string(num);
            pathExport = "/sys/class/gpio/export";
            fdExport = open(pathExport.c_str(), O_WRONLY);
            if (fdExport < 0)
            {
                perror("failed to open file");
                return;
            }
            write(fdExport, num_str.c_str(), num_str.size());
            close(fdExport);
            sleep(3);
            pathDir = "/sys/class/gpio/gpio" + std::to_string(num) + "/direction";
            fdDir = open(pathDir.c_str(), O_WRONLY);
            if (fdDir < 0)
            {
                perror("failed to open gpio direction");
                return;
            }
            write(fdDir, direction.c_str(), direction.size());
            pathVal = "/sys/class/gpio/gpio" + std::to_string(num) + "/value";
            fdVal = open(pathVal.c_str(), O_RDWR);
            if (fdVal < 0)
            {
                perror("failed to open gpio value");
                return;
            }
        }
        int Gpio::Gpio_read(int pin)
        {
            char buffer[2] = {0};
            lseek(fdVal, 0, SEEK_SET);
            ssize_t bytesRead = read(fdVal, buffer, sizeof(buffer) - 1);
            if (bytesRead > 0)
            {
                return atoi(buffer);
            }
            else
            {
                std::cout << "Read failed or empty" << std::endl;
                return -1;
            }
        }
        void Gpio::Gpio_setDirection(std::string direction)
        {
            write(fdDir, direction.c_str(), direction.size());
        }

        void Gpio::Gpio_write(int value)
        {
            std::string val_str = std::to_string(value);
            write(fdVal, val_str.c_str(), val_str.size());
        }
        Gpio::~Gpio()
        {
            if (fdVal >= 0)
            {
                lseek(fdVal, 0, SEEK_SET);
                write(fdVal, "0", 1);
                close(fdVal);
            }

            if (fdDir >= 0)
            {
                lseek(fdDir, 0, SEEK_SET);
                write(fdDir, "in", 2);
                close(fdDir);
            }

            std::cout << "Returned to default (direction=in, value=0)" << std::endl;

            int fd = open("/sys/class/gpio/unexport", O_WRONLY);
            if (fd >= 0)
            {
                std::string num_str = std::to_string(num);
                write(fd, num_str.c_str(), num_str.size());
                close(fd);
                std::cout << "GPIO " << num << " unexported and resources released." << std::endl;
            }
            else
            {
                perror("Failed to unexport GPIO");
            }
        }
        Gpio& Gpio::operator~()
        {
            int current = Gpio_read(this->num);
            Gpio_write(!current);
            return *this;
        }
    }
}