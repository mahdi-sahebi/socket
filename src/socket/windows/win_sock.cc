#include <stdexcept>
#include <winsock2.h>
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
    WSADATA wsaData;

    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        throw std::runtime_error("WSAStartup failed with error");
    }
}

