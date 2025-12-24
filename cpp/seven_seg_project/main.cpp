#include "include/sevensegment.hpp"
#include <iostream>
#include "include/stream.hpp"
#include "include/istream.hpp"
#include "include/ostream.hpp"
#include <unistd.h>
int main()
{
    int x = 0;
    stream *obj = new sevensegment();

        sleep(1);
        while (true)
        {
            try
            {
                int x = obj->read();
                obj->write(x);
                if (x == -1)
                {
                    break;
                }
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << e.what() << std::endl;
                break;
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << e.what() << std::endl;
                break;
            }
        }

    delete obj;
    return 0;
}
