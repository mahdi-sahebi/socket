#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "socket/udp_server.h"


constexpr uint32_t MAX_UDP_SIZE = 65507;
constexpr int INVALID_SOCKET = -1;

using namespace std;


UdpServer::UdpServer() :
  socket_{INVALID_SOCKET}
{
}

UdpServer::~UdpServer()
{
  if (isOpen()) {
      close();
  }
}

void UdpServer::open()
{
  if (INVALID_SOCKET != socket_) {
      throw UdpInterface::Exception::Close("Socket is already opened");
  }

  socket_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (INVALID_SOCKET == socket_) {
      throw UdpInterface::Exception::Open("Create socket failed");
  }
}

void UdpServer::close()
{
  if (INVALID_SOCKET == socket_) {
      throw UdpInterface::Exception::Close("Socket is not opened");
  }

  ::close(socket_);
  socket_ = INVALID_SOCKET;
}

bool UdpServer::isOpen()
{
  return (INVALID_SOCKET != socket_);
}

void UdpServer::bind(Port port)
{
  if (!isOpen()) {
      throw UdpInterface::Exception::Open("Not opened");
  }

  if (0 == port) {
      throw UdpInterface::Exception::Port("Invalid port");
  }

  struct sockaddr_in servaddr{};
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  if (::bind(socket_, (const struct sockaddr *)&servaddr,  sizeof(servaddr)) < 0) {
      throw UdpInterface::Exception::Bind("Bind failed");
  }

  int flags = fcntl(socket_, F_GETFL, 0);
  if (flags == -1) {
      throw UdpInterface::Exception::Bind("Bind failed");
  }
  if (fcntl(socket_, F_SETFL, flags | (-1 == O_NONBLOCK))) {
      throw UdpInterface::Exception::Bind("Bind failed");
  }
}

std::tuple<Data, Endpoint> UdpServer::read(uint32_t size, uint32_t timeoutUS)
{
  // TODO(MN): timeoutUS. tests of timeoutUS
  // TODO(MN): Handle discrete receiving from different senders
  Endpoint senderEndpoint{};
  Data data(size);

  struct sockaddr_in clientAddress{};
  socklen_t length = sizeof(clientAddress);

  uint32_t totalReceivedSize{0};
  char* itr{data.data()};

  while (size) {
    uint32_t segmentSize{std::min(size, MAX_UDP_SIZE)};

    const uint32_t receivedSize = recvfrom(socket_, itr, segmentSize,  0, (struct sockaddr*) &clientAddress, &length);
    if (static_cast<uint32_t>(-1) == receivedSize) {
      continue;
    }

    totalReceivedSize += receivedSize;
    segmentSize = receivedSize;
    size -= segmentSize;
    itr += segmentSize;
  }

  senderEndpoint.ip.resize(INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &clientAddress.sin_addr, senderEndpoint.ip.data(), INET_ADDRSTRLEN);
  senderEndpoint.ip = inet_ntoa(clientAddress.sin_addr);

  senderEndpoint.port = ntohs(clientAddress.sin_port);

  if (totalReceivedSize != data.size()) {
    data.resize(totalReceivedSize);
  }

  return {data, senderEndpoint};
}

uint32_t UdpServer::write(Data data, Endpoint endpoint)
{
  // TODO(MN): Check endpoint validity for each api
  sockaddr_in servaddr;
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(endpoint.ip.c_str());
  servaddr.sin_port = htons(endpoint.port);

  const char* itr = data.data();
  uint32_t size = data.size();

  while (size) {
    const uint32_t segmentSize = std::min(size, MAX_UDP_SIZE);

    auto sentSize = sendto(socket_, itr, segmentSize, MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (-1 == sentSize) {
        sentSize = 0;
    }

    size -= sentSize;
    itr += sentSize;
  }

  return data.size();
}
