#ifndef TCP_SOCKET_INTERFACE_H_
#define TCP_SOCKET_INTERFACE_H_

#include <stdexcept>
#include "socket_interface.h"

class TcpInterface : public SocketInterface
{
public:
  class Exception
  {
  public:
    class Port : public std::invalid_argument { public: explicit Port(const std::string& m) : std::invalid_argument(m) {} };
    class Open : public std::runtime_error { public: explicit Open(const std::string& m) : std::runtime_error(m) {} };
    class Close : public std::runtime_error { public: explicit Close(const std::string& m) : std::runtime_error(m) {} };
    class Bind : public std::runtime_error { public: explicit Bind(const std::string& m) : std::runtime_error(m) {} };
    class Connect : public std::runtime_error { public: explicit Connect(const std::string& m) : std::runtime_error(m) {} };
    class Accept : public std::runtime_error { public: explicit Accept(const std::string& m) : std::runtime_error(m) {} };
    class Io : public std::runtime_error { public: explicit Io(const std::string& m) : std::runtime_error(m) {} };
  };
};

#endif
