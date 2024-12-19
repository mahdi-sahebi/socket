#ifndef UDP_H_
#define UDP_H_

#include "socket_interface.h"

class Udp : public Socket
{
public:
    class Builder;

    Udp(const Udp&) = delete;
    Udp& operator=(const Udp&) = delete;
    Udp(Udp&&);
    Udp& operator=(Udp&&);
    virtual ~Udp() = default;

    uint32_t send(const std::vector<char> data) override;
    std::vector<char> receive(uint32_t size, std::chrono::milliseconds timeout) override;

    uint32_t sendAsync(const std::vector<char> data, std::function<void>(void*)) override;
    std::vector<char> receiveAsync(uint32_t size, std::chrono::milliseconds timeout, std::function<void>(void*)) override;

    Protocol getProtocol() override;

private:
    friend class Builder;

    Socket::Protocol protocol_;

    Udp() = default;
};



class Udp::Builder
{
public:
    Builder()
    {
    }

    Udp build()
    {
        Udp udp_;
        udp_.protocol_ = Socket::Protocol::UDP;
        return udp_;
    }

private:

};



#endif /* UDP_H_ */
