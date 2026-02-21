#pragma once

#include <v1/bootloader/methods/bootloader2StubDefault.hpp>
#include <atomic>
#include <thread>
#include <string>

class bootloader2StubImpl :
    public v1::bootloader::methods::bootloader2StubDefault
{
public:
    bootloader2StubImpl();
    virtual ~bootloader2StubImpl();

    void requestDownload(
        const std::shared_ptr<CommonAPI::ClientId> client,
        requestDownloadReply_t reply) override;

    void requestData(
        const std::shared_ptr<CommonAPI::ClientId> client,
        int32_t size,
        requestDataReply_t reply) override;

private:
    void watchFile();

    std::atomic<bool> ready_{false};
    std::string fileData_;
    std::thread watcherThread_;
};
