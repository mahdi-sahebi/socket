#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include "tcp_socket_interface.h"

class TcpClient : public TcpInterface
{
public:
  TcpClient();
  explicit TcpClient(Endpoint endpoint);
  ~TcpClient();
  void open() override;
  void close() override;
  bool isOpen() const override;
  uint32_t read(char* const outData, uint32_t size, uint32_t timeoutUS = 0) const override;
  uint32_t write(const char* const inData, uint32_t size) const override;
  void connect(Endpoint endpoint);
  uint32_t read(Data& data, uint32_t timeoutUS = 0) const;
  uint32_t write(const Data& data) const;
private:
  Socket socket_;
  Endpoint endpoint_;
};

#endif
