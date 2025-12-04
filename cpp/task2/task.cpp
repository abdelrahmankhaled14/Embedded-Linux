#include <iostream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <initializer_list>
#include <vector>
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
            Gpio(int pin)
            {
                this-> num = pin + 512;
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
            Gpio(int pin, std::string direction)
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
            int Gpio_read(int pin)
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
            void Gpio_setDirection(std::string direction)
            {
                write(fdDir, direction.c_str(), direction.size());
            }
            void Gpio_write(int value)
            {
                std::string val_str = std::to_string(value);
                write(fdVal, val_str.c_str(), val_str.size());
            }
            ~Gpio()
            {
                if (fdVal >= 0)
                {
                    lseek(fdVal, 0, SEEK_SET); // Reset position
                    write(fdVal, "0", 1);      // Write only 1 byte: "0"
                    close(fdVal);
                }

                if (fdDir >= 0)
                {
                    lseek(fdDir, 0, SEEK_SET); // Reset position
                    write(fdDir, "in", 2);     // Write only 2 bytes: "in"
                    close(fdDir);
                }

                std::cout << "Returned to default (direction=in, value=0)" << std::endl;

                // Unexport the GPIO
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
            Gpio &operator~()
            {
                int current = Gpio_read(this->num);
                Gpio_write(!current);
                return *this;
            }
        };
    }

}
int main()
{
    Mcal::Gpio::Gpio gpio23(23, "out");
    Mcal::Gpio::Gpio gpio24(24, "out");
    gpio24.Gpio_write(1);
    gpio23.Gpio_write(1);
    sleep(2);
    gpio24.Gpio_write(0);
    gpio23.Gpio_write(0);
    sleep(2);
    ~gpio23;
    ~gpio24;
    sleep(2);
    int val = gpio23.Gpio_read(23);
    std::cout << "GPIO Value: " << val << std::endl;

    return 0;
}