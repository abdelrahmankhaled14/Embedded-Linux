#include "istream.hpp"
#include <limits>
istream::istream()
{
    std::cout << "istream constructor called" << std::endl;
}
istream::~istream()
{
    std::cout << "istream destructor called" << std::endl;
}
int istream::read()
{
    std::string input;

    std::cout << "Please enter a digit (0-9): " << std::flush;
    std::cin >> input;
    if(input == "e")
        return -1;
        
    if (input.size() != 1 || !isdigit(input[0]))
        throw std::invalid_argument(
            "Invalid input. Please enter a single digit between 0 and 9."
        );

    int value = input[0] - '0';

    if (value < 0 || value > 9)
        throw std::out_of_range("Digit out of range");
 

    return value;
}


