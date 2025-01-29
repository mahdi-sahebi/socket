#ifndef UDP_SOCKET_INTERFACE_H_
#define UDP_SOCKET_INTERFACE_H_

#include <tuple>
#include <stdexcept>
#include "socket_interface.h"


class UdpInterface: SocketInterface
{
public:
  virtual std::tuple<Data, Endpoint> read(uint32_t size, uint32_t timeoutUS = 0) = 0;
  virtual uint32_t write(Data data, Endpoint endpoint) = 0;

  class Exception
  {
  public:
    class Port : public std::invalid_argument
    {
      public:
        explicit Port(const std::string& message) : std::invalid_argument(message){}
    };

    class Open : public std::runtime_error
    {
    public:
      explicit Open(const std::string& message) : std::runtime_error(message){}
    };

    class Close : public std::runtime_error
    {
    public:
      explicit Close(const std::string& message) : std::runtime_error(message){}
    };

    class Bind : public std::runtime_error
    {
    public:
      explicit Bind(const std::string& message) : std::runtime_error(message){}
    };
  };
};


#endif /* UDP_SOCKET_INTERFACE_H_ */
