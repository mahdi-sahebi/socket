#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

#include "udp_socket_interface.h"

class UdpClient : public UdpInterface
{
public:
  UdpClient();
  UdpClient(Endpoint sendingEndpoint);
  ~UdpClient();

  void open() override;
  void close() override;
  bool isOpen() const override;
  uint32_t read(char* const outData, uint32_t size, uint32_t timeoutUS = 0) const override;
  uint32_t write(const char* const inData, uint32_t size) const override;

  std::tuple<Data, Endpoint> read(uint32_t size, uint32_t timeoutUS = 0) const override;
  uint32_t write(Data data, Endpoint endpoint) const override;

private:
  Socket socket_;
  Endpoint sendingEndpoint_;

};


#endif /* UDP_CLIENT_H_ */
