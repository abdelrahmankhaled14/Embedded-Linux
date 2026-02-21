#include <CommonAPI/CommonAPI.hpp>
#include <v1/bootloader/methods/bootloader2Proxy.hpp>
#include <thread>
#include <iostream>

using namespace v1::bootloader::methods;

int main()
{
    auto runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "bootloader.methods.justSendHi";
    std::string connection = "client-sample";

    auto proxy = runtime->buildProxy<bootloader2Proxy>(domain, instance, connection);

    std::cout << "Waiting for service...\n";

    // Wait until service is available
    while (!proxy->isAvailable())
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::cout << "Service available!\n";

    // Subscribe to event
    proxy->getChangeEvent().subscribe([proxy](const std::string &version)
    {
        std::cout << "Event received: " << version << "\n";

        std::thread([proxy]()
        {
            CommonAPI::CallStatus status;
            bool ready;
            int32_t size;

            proxy->requestDownload(status, ready, size);

            if (status != CommonAPI::CallStatus::SUCCESS || !ready)
            {
                std::cout << "Server not ready\n";
                return;
            }

            std::cout << "File size: " << size << "\n";

            std::string data;
            proxy->requestData(size, status, data);

            std::cout << "Received Data:\n" << data << "\n";

        }).detach();
    });

    // Keep client alive
    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
