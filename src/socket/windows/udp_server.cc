#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <winsock2.h>
#include "win_sock.h"
#include "socket/udp_server.h"


constexpr uint32_t MAX_UDP_SIZE = 1460;

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;


UdpServer::UdpServer() :
  socket_{INVALID_SOCKET}
{
  WinSockManager::getInstance();
}

UdpServer::UdpServer(Endpoint sendingEndpoint) :
  socket_{INVALID_SOCKET},
  sendingEndpoint_{sendingEndpoint}
{
  WinSockManager::getInstance();
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

  socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (INVALID_SOCKET == socket_) {
      throw UdpInterface::Exception::Open("Create socket failed");
  }

  int bufferSize = 8 * 1024 * 1024;
  if (SOCKET_ERROR == setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize))) {
      throw UdpInterface::Exception::Open("Set receive buffer size failed");
  }
}

void UdpServer::close()
{
  if (INVALID_SOCKET == socket_) {
      throw UdpInterface::Exception::Close("Socket is not opened");
  }

  ::closesocket(socket_);
  socket_ = INVALID_SOCKET;
}

bool UdpServer::isOpen() const 
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
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
  if (::bind(socket_, (const struct sockaddr *)&servaddr,  sizeof(servaddr))) {
      throw UdpInterface::Exception::Bind("Bind failed");
  }
}

uint32_t UdpServer::read(char* const outData, uint32_t size, uint32_t timeoutUS) const 
{
  const auto endTime{chrono::high_resolution_clock::now() + microseconds(timeoutUS)};

  uint32_t totalReceivedSize{0};
  char* itr{outData};

  do {
    uint32_t segmentSize{std::min(size, MAX_UDP_SIZE)};
    const uint32_t receivedSize = recvfrom(socket_, itr, segmentSize,  0, nullptr, nullptr);
  
    if (static_cast<uint32_t>(-1) == receivedSize) {
      continue;
    }

    totalReceivedSize += receivedSize;
    segmentSize = receivedSize;
    size -= segmentSize;
    itr += segmentSize;
  } while ((chrono::high_resolution_clock::now() <= endTime) && size);
  
  return totalReceivedSize;
}

std::tuple<Data, Endpoint> UdpServer::read(uint32_t size, uint32_t timeoutUS) const 
{
  // TODO(MN): Handle discrete receiving from different senders
  Endpoint senderEndpoint{};
  Data data(size);

  const auto endTime{chrono::high_resolution_clock::now() + microseconds(timeoutUS)};

  struct sockaddr_in clientAddress{};
  int clientAddrSize = sizeof(clientAddress);

  uint32_t totalReceivedSize{0};
  char* itr{data.data()};

  do {
    uint32_t segmentSize{std::min(size, MAX_UDP_SIZE)};
    
    const uint32_t receivedSize = recvfrom(socket_, itr, segmentSize,  0, (struct sockaddr*) &clientAddress, &clientAddrSize);
    if (static_cast<uint32_t>(-1) == receivedSize) {
      continue;
    }

    totalReceivedSize += receivedSize;
    segmentSize = receivedSize;
    size -= segmentSize;
    itr += segmentSize;
  } while ((chrono::high_resolution_clock::now() <= endTime) && size);
  
  senderEndpoint.ip   = inet_ntoa(clientAddress.sin_addr);
  senderEndpoint.port = ntohs(clientAddress.sin_port);

  if (totalReceivedSize != data.size()) {
    data.resize(totalReceivedSize);
  }

  return {data, senderEndpoint};
}

uint32_t UdpServer::write(Data data, Endpoint endpoint) const 
{
  // TODO(MN): Check endpoint validity for each api
  sockaddr_in servaddr = {0};
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(endpoint.ip.c_str());
  servaddr.sin_port = htons(endpoint.port);

  const char* itr = data.data();
  uint32_t size = data.size();

  while (size) {
    const uint32_t segmentSize = std::min(size, MAX_UDP_SIZE);

    auto sentSize = sendto(socket_, itr, segmentSize, 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (-1 == sentSize) {
        sentSize = 0;
    }

    size -= sentSize;
    itr += sentSize;
  }

  return data.size();
}

uint32_t UdpServer::write(const char* const inData, uint32_t size) const 
{
  // TODO(MN): Check endpoint validity for each api
  sockaddr_in servaddr = {0};
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(sendingEndpoint_.ip.c_str());
  servaddr.sin_port = htons(sendingEndpoint_.port);

  const char* itr = inData;
  uint32_t totalSent = 0;

  while (size) {
    const uint32_t segmentSize = std::min(size, MAX_UDP_SIZE);

    auto sentSize = sendto(socket_, itr, segmentSize, 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (-1 == sentSize) {
        sentSize = 0;
    }

    size -= sentSize;
    itr += sentSize;
    totalSent += sentSize;
  }

  return totalSent;
}
