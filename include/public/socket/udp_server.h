#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include "udp_socket_interface.h"

class UdpServer : public UdpInterface
{
public:
  UdpServer();
  ~UdpServer();

  void open() override;
  void close() override;
  bool isOpen() override;
  void bind(Port port);

  std::tuple<Data, Endpoint> read(uint32_t size, uint32_t timeoutUS = 0) override;
  uint32_t write(Data data, Endpoint endpoint) override;

private:
  int socket_;

};


#endif /* UDP_SERVER_H_ */
