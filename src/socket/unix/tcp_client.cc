#include "socket/tcp_client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {
constexpr Socket invalidSocket = static_cast<Socket>(-1);
int fd(Socket s) { return static_cast<int>(s); }
uint32_t readSocket(Socket s, char* out, uint32_t size, uint32_t timeout) {
  uint32_t total = 0;
  while (total < size) {
    if (timeout) { fd_set set; FD_ZERO(&set); FD_SET(fd(s), &set); timeval tv{static_cast<time_t>(timeout / 1000000), static_cast<suseconds_t>(timeout % 1000000)}; if (select(fd(s)+1, &set, nullptr, nullptr, &tv) <= 0) break; }
    const ssize_t n = ::recv(fd(s), out + total, size - total, 0); if (n <= 0) break; total += static_cast<uint32_t>(n);
  }
  return total;
}
uint32_t writeSocket(Socket s, const char* data, uint32_t size) { uint32_t total=0; while(total<size) { const ssize_t n=::send(fd(s), data+total, size-total, 0); if(n<=0) break; total+=static_cast<uint32_t>(n); } return total; }
}
TcpClient::TcpClient() : socket_{invalidSocket} {}
TcpClient::TcpClient(Endpoint endpoint) : socket_{invalidSocket}, endpoint_{endpoint} {}
TcpClient::~TcpClient() { if (isOpen()) close(); }
void TcpClient::open() { if(isOpen()) throw TcpInterface::Exception::Close("Socket is already opened"); socket_=static_cast<Socket>(::socket(AF_INET,SOCK_STREAM,0)); if(socket_==invalidSocket) throw TcpInterface::Exception::Open("Create socket failed"); }
void TcpClient::connect(Endpoint endpoint) {
  if(!isOpen()) throw TcpInterface::Exception::Open("Not opened");
  if(!endpoint.port) throw TcpInterface::Exception::Port("Invalid port");
  endpoint_=endpoint; sockaddr_in address{}; address.sin_family=AF_INET; address.sin_port=htons(endpoint.port);
  if(::inet_pton(AF_INET, endpoint.ip.c_str(), &address.sin_addr) != 1 || ::connect(fd(socket_), reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) throw TcpInterface::Exception::Connect("Connect failed");
}
void TcpClient::close() { if(!isOpen()) throw TcpInterface::Exception::Close("Socket is not opened"); ::close(fd(socket_)); socket_=invalidSocket; }
bool TcpClient::isOpen() const { return socket_!=invalidSocket; }
uint32_t TcpClient::read(char* const out,uint32_t size,uint32_t timeout) const { if(!isOpen()) throw TcpInterface::Exception::Io("Socket is not opened"); return readSocket(socket_,out,size,timeout); }
uint32_t TcpClient::write(const char* const data,uint32_t size) const { if(!isOpen()) throw TcpInterface::Exception::Io("Socket is not opened"); return writeSocket(socket_,data,size); }
uint32_t TcpClient::read(Data& data,uint32_t timeout) const { return read(data.data(),static_cast<uint32_t>(data.size()),timeout); }
uint32_t TcpClient::write(const Data& data) const { return write(data.data(),static_cast<uint32_t>(data.size())); }
