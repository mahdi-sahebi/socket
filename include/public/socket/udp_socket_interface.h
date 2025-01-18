#ifndef UDP_SOCKET_INTERFACE_H_
#define UDP_SOCKET_INTERFACE_H_

#include <tuple>
#include "socket_interface.h"


class UdpInterface: SocketInterface
{
public:
  virtual std::tuple<Data, Endpoint> read(uint32_t size, uint32_t timeoutUS = 0) = 0;
  virtual uint32_t write(Data data, Endpoint endpoint) = 0;

};


#endif /* UDP_SOCKET_INTERFACE_H_ */
