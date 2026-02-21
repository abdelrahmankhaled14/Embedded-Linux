#include <CommonAPI/CommonAPI.hpp>
#include "bootloader2StubImpl.hpp"
#include <vsomeip/vsomeip.hpp>
#include <thread>
#include <iostream>

int main()
{
    // Initialize vsomeip FIRST as routing manager
    auto vsomeip_app = vsomeip::runtime::get()->create_application("ldog_service");
    if (!vsomeip_app) {
        std::cerr << "Failed to create vsomeip application\n";
        return 1;
    }
    
    vsomeip_app->init();
    
    // Start vsomeip in a separate thread (non-blocking)
    std::thread vsomeip_thread([vsomeip_app]() {
        vsomeip_app->start();
    });
    
    // Give vsomeip time to initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Set CommonAPI properties
    CommonAPI::Runtime::setProperty("LogContext", "ldog_service");
    CommonAPI::Runtime::setProperty("LogApplication", "ldog_service");
    CommonAPI::Runtime::setProperty("LibraryBase", "bootloader");
    
    std::cout << "Server starting...\n";
    
    // Create CommonAPI runtime
    auto runtime = CommonAPI::Runtime::get();
    
    // Domain and instance
    std::string domain = "local";
    std::string instance = "bootloader.methods.justSendHi";
    
    // Create service stub
    auto service = std::make_shared<bootloader2StubImpl>();
    
    // Register as service
    while (!runtime->registerService(domain, instance, service))
    {
        std::cout << "Register failed, retrying...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Service registered and routing manager ready!\n";
    
    // Keep server alive
    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(10));
    
    return 0;
}