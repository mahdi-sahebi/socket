#ifndef SOCKET_INTERFACE_H_
#define SOCKET_INTERFACE_H_

#include <cstdint>
#include <vector>
#include <chrono>
#include <functional>
#include <string>


using IP = std::string;
using Port = uint16_t;
using Data = std::vector<char>;

struct Endpoint
{
  Endpoint() :
    ip{"0.0.0.0"},
    port{0}
  {

  }

  Endpoint(IP ip, Port port){
      this->ip = ip;
      this->port = port;
  }

  IP ip;
  Port port;
};

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
