#ifndef SOCKET_INTERFACE_H_
#define SOCKET_INTERFACE_H_

#include <cstdint>
#include <vector>
#include <chrono>
#include <functional>

// TODO(MN): Dervie from IO interface and handle files...
class Socket
{
public:
    enum class Protocol
    {
        UDP = 0,
        TCP
    };

    virtual ~Socket() = default;

    virtual uint32_t send(const std::vector<char> data) = 0;
    virtual std::vector<char> receive(uint32_t size, std::chrono::milliseconds timeout) = 0;

    virtual uint32_t sendAsync(const std::vector<char> data, std::function<void>(void*)) = 0;
    virtual std::vector<char> receiveAsync(uint32_t size, std::chrono::milliseconds timeout, std::function<void>(void*)) = 0;

    virtual Protocol getProtocol() = 0;
};


#endif /* SOCKET_INTERFACE_H_ */
