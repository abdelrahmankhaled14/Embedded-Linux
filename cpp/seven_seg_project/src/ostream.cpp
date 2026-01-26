#include <ostream.hpp>

ostream::~ostream()
{
    std::cout << "ostream destructor called" << std::endl;
}
ostream::ostream()
{
    std::cout << "ostream constructor called" << std::endl;
}
