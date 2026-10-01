#include "socket/tcp_server.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>

namespace {
constexpr Socket invalidSocket = static_cast<Socket>(-1);
int fd(Socket s) { return static_cast<int>(s); }
uint32_t readSocket(Socket s, char* out, uint32_t size, uint32_t timeout) {
  uint32_t total = 0;
  while (total < size) {
    if (timeout) {
      fd_set set; FD_ZERO(&set); FD_SET(fd(s), &set);
      timeval tv{static_cast<time_t>(timeout / 1000000), static_cast<suseconds_t>(timeout % 1000000)};
      const int ready = select(fd(s) + 1, &set, nullptr, nullptr, &tv);
      if (ready <= 0) break;
    }
    const ssize_t n = ::recv(fd(s), out + total, size - total, 0);
    if (n <= 0) break;
    total += static_cast<uint32_t>(n);
  }
  return total;
}
uint32_t writeSocket(Socket s, const char* data, uint32_t size) {
  uint32_t total = 0;
  while (total < size) {
    const ssize_t n = ::send(fd(s), data + total, size - total, 0);
    if (n <= 0) break;
    total += static_cast<uint32_t>(n);
  }
  return total;
}
}

TcpServer::TcpServer() : socket_{invalidSocket}, clientSocket_{invalidSocket} {}
TcpServer::~TcpServer() { if (isOpen()) close(); }
void TcpServer::open() {
  if (isOpen()) throw TcpInterface::Exception::Close("Socket is already opened");
  socket_ = static_cast<Socket>(::socket(AF_INET, SOCK_STREAM, 0));
  if (socket_ == invalidSocket) throw TcpInterface::Exception::Open("Create socket failed");
  int reuse = 1;
  setsockopt(fd(socket_), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
}
void TcpServer::bind(Port port) {
  if (!isOpen()) throw TcpInterface::Exception::Open("Not opened");
  if (!port) throw TcpInterface::Exception::Port("Invalid port");
  sockaddr_in address{}; address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_ANY); address.sin_port = htons(port);
  if (::bind(fd(socket_), reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0 || ::listen(fd(socket_), 1) < 0)
    throw TcpInterface::Exception::Bind("Bind or listen failed");
}
void TcpServer::accept() {
  if (!isOpen()) throw TcpInterface::Exception::Open("Not opened");
  if (clientSocket_ != invalidSocket) ::close(fd(clientSocket_));
  const int accepted = ::accept(fd(socket_), nullptr, nullptr);
  if (accepted < 0) throw TcpInterface::Exception::Accept("Accept failed");
  clientSocket_ = static_cast<Socket>(accepted);
}
void TcpServer::close() {
  if (!isOpen()) throw TcpInterface::Exception::Close("Socket is not opened");
  if (clientSocket_ != invalidSocket) { ::close(fd(clientSocket_)); clientSocket_ = invalidSocket; }
  ::close(fd(socket_)); socket_ = invalidSocket;
}
bool TcpServer::isOpen() const { return socket_ != invalidSocket; }
uint32_t TcpServer::read(char* const out, uint32_t size, uint32_t timeout) const {
  if (clientSocket_ == invalidSocket) throw TcpInterface::Exception::Io("No client is connected");
  return readSocket(clientSocket_, out, size, timeout);
}
uint32_t TcpServer::write(const char* const data, uint32_t size) const {
  if (clientSocket_ == invalidSocket) throw TcpInterface::Exception::Io("No client is connected");
  return writeSocket(clientSocket_, data, size);
}
uint32_t TcpServer::read(Data& data, uint32_t timeout) const { return read(data.data(), static_cast<uint32_t>(data.size()), timeout); }
uint32_t TcpServer::write(const Data& data) const { return write(data.data(), static_cast<uint32_t>(data.size())); }
