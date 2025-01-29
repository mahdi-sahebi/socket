#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include "socket/udp_client.h"


constexpr uint32_t MAX_UDP_SIZE = 65507;
constexpr int INVALID_SOCKET = -1;// TODO(MN): Move into the common private header

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;


UdpClient::UdpClient() :
  socket_{INVALID_SOCKET}
{
}

UdpClient::~UdpClient()
{
  if (isOpen()) {
      close();
  }
}

void UdpClient::open()
{
  if (INVALID_SOCKET != socket_) {
      throw UdpInterface::Exception::Close("Socket is already opened");
  }

  socket_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (INVALID_SOCKET == socket_) {
      throw UdpInterface::Exception::Open("Create socket failed");
  }

  int flags = fcntl(socket_, F_GETFL, 0);
  if (flags == -1) {
      throw UdpInterface::Exception::Bind("Bind failed");
  }
  if (fcntl(socket_, F_SETFL, flags | O_NONBLOCK)) {
      throw UdpInterface::Exception::Bind("Bind failed");
  }
}

void UdpClient::close()
{
  if (INVALID_SOCKET == socket_) {
      throw UdpInterface::Exception::Close("Socket is not opened");
  }

  ::close(socket_);
  socket_ = INVALID_SOCKET;
}

bool UdpClient::isOpen()
{
  return (INVALID_SOCKET != socket_);
}

std::tuple<Data, Endpoint> UdpClient::read(uint32_t size, uint32_t timeoutUS)
{
  // TODO(MN): Handle discrete receiving from different senders
  Endpoint senderEndpoint{};
  Data data(size);

  const auto endTime{chrono::high_resolution_clock::now() + milliseconds(timeoutUS)};

  struct sockaddr_in clientAddress{};
  socklen_t length = sizeof(clientAddress);

  uint32_t totalReceivedSize{0};
  char* itr{data.data()};

  do {
    uint32_t segmentSize{std::min(size, MAX_UDP_SIZE)};

    const uint32_t receivedSize = recvfrom(socket_, itr, segmentSize,  0, (struct sockaddr*) &clientAddress, &length);
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

uint32_t UdpClient::write(Data data, Endpoint endpoint)
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

