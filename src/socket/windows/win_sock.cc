#include <stdexcept>
#include "win_sock.h"


std::shared_ptr<WinSockManager> WinSockManager::instance_ = nullptr;

std::shared_ptr<WinSockManager> WinSockManager::getInstance()
{
    if (nullptr == instance_) {
        instance_ = std::shared_ptr<WinSockManager>(new WinSockManager());
    }

    return instance_;
}

WinSockManager::~WinSockManager()
{
    WSACleanup();
}

WinSockManager::WinSockManager()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData_)) {
        throw std::runtime_error("WSAStartup failed with error");
    }
}

