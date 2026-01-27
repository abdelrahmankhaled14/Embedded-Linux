#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

int brightness = -1;
bool updated = false;
bool running = true;

std::mutex mtx;
std::condition_variable cv;

void brightnessWatcher()
{
    const char *path = "/sys/class/leds/input3::capslock/brightness";

    int fd = open(path, O_RDONLY);

    int last_value = -1;

    while (running)
    {
        lseek(fd, 0, SEEK_SET);

        char buf[16];
        memset(buf, 0, sizeof(buf));

        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            int new_value = atoi(buf);

            if (new_value != last_value)
            {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    brightness = new_value;
                    updated = true;
                    last_value = new_value;
                }
                cv.notify_one();
            }
        }
    }

    close(fd);
}

void brightnessConsumer()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "WAITING" << std::endl;
        cv.wait(lock, []
                { return updated; });

        int value = brightness;
        updated = false;

        lock.unlock();

        std::cout << "Brightness changed to: "
                  << value << std::endl;
    }
}

int main()
{
    std::thread consumer(brightnessConsumer);
    std::thread producer(brightnessWatcher);

    producer.join();
    consumer.join();

    return 0;
}
