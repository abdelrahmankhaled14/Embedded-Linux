#include "bootloader2StubImpl.hpp"

#include <sys/inotify.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#define WATCH_FILE "/tmp/firmware.bin"
#define EVENT_BUF_LEN (1024 * (sizeof(struct inotify_event) + 16))

bootloader2StubImpl::bootloader2StubImpl()
{
    watcherThread_ = std::thread(&bootloader2StubImpl::watchFile, this);
}

bootloader2StubImpl::~bootloader2StubImpl()
{
    if (watcherThread_.joinable())
        watcherThread_.join();
}

void bootloader2StubImpl::watchFile()
{
    int fd = inotify_init();
    if (fd < 0) {
        std::cerr << "inotify init failed\n";
        return;
    }

    int wd = inotify_add_watch(fd, WATCH_FILE, IN_MODIFY);
    if (wd < 0) {
        std::cerr << "Cannot watch file: " << WATCH_FILE << "\n";
        return;
    }

    char buffer[EVENT_BUF_LEN];

    while (true)
    {
        int length = read(fd, buffer, EVENT_BUF_LEN);

        if (length < 0)
            continue;

        std::cout << "File changed detected!\n";

        std::ifstream file(WATCH_FILE, std::ios::binary);
        fileData_ = std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );

        ready_ = true;

        fireChangeEvent("file-updated");
    }
}

void bootloader2StubImpl::requestDownload(
    const std::shared_ptr<CommonAPI::ClientId>,
    requestDownloadReply_t reply)
{
    bool done = ready_;
    int32_t size = ready_ ? fileData_.size() : 0;

    std::cout << "Client asked requestDownload. ready=" << done << "\n";

    reply(done, size);
}

void bootloader2StubImpl::requestData(
    const std::shared_ptr<CommonAPI::ClientId>,
    int32_t size,
    requestDataReply_t reply)
{
    if (!ready_)
    {
        reply("");
        return;
    }

    std::cout << "Sending " << size << " bytes\n";

    reply(fileData_.substr(0, size));

    ready_ = false;
}
