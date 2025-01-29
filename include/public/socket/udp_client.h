#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

#include "udp_socket_interface.h"

class UdpClient : public UdpInterface
{
public:
  UdpClient();
  ~UdpClient();

  void open() override;
  void close() override;
  bool isOpen() override;

  std::tuple<Data, Endpoint> read(uint32_t size, uint32_t timeoutUS = 0) override;
  uint32_t write(Data data, Endpoint endpoint) override;

private:
  int socket_;

};


#endif /* UDP_CLIENT_H_ */
