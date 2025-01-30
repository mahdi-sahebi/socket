#ifndef SOCKET_INTERFACE_H_
#define SOCKET_INTERFACE_H_

#include <cstdint>
#include <vector>
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

  Endpoint(IP ip, Port port)
  {
    this->ip = ip;
    this->port = port;
  }

  IP ip;
  Port port;
};

// TODO(MN): Dervie from IO interface and handle files...
class SocketInterface
{
public:
  virtual void open() = 0;
  virtual void close() = 0;
  virtual bool isOpen() = 0;
  virtual uint32_t read(char* const outData, uint32_t size, uint32_t timeoutUS = 0) = 0;
  virtual uint32_t write(const char* const inData, uint32_t size) = 0;
};


#endif /* SOCKET_INTERFACE_H_ */
