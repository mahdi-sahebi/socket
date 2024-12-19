#include <memory>
#include "socket/udp.h"

using namespace std;

Udp::Udp(Udp&& other)
{
    *this = move(other);
}

Udp& Udp::operator=(Udp&& other)
{
    if (this != &other) {
        // TODO(MN): Implement
    }

    return *this;
}

uint32_t Udp::send(const std::vector<char> data)
{
    return 0;
}
std::vector<char> Udp::receive(uint32_t size, std::chrono::milliseconds timeout)
{
    return {};
}

uint32_t Udp::sendAsync(const std::vector<char> data, std::function<void>(void*))
{
    return 0;
}
std::vector<char> Udp::receiveAsync(uint32_t size, std::chrono::milliseconds timeout, std::function<void>(void*))
{
    return {};
}

Socket::Protocol Udp::getProtocol()
{
    return protocol_;
}
