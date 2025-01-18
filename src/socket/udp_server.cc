#include "socket/udp_server.h"

using namespace std;

constexpr int INVALID_SOCKET = -1;


UdpServer::UdpServer() :
  socket_{INVALID_SOCKET}
{
}

UdpServer::~UdpServer()
{
}

void UdpServer::open()
{

}

void UdpServer::close()
{

}

bool UdpServer::isOpen()
{
  return false;
}

void UdpServer::bind(Port port)
{
}

std::tuple<Data, Endpoint> UdpServer::read(uint32_t size, uint32_t timeoutUS)
{
  Endpoint senderEndpoint;
  Data data(size);

  return {data, senderEndpoint};
}

uint32_t UdpServer::write(Data data, Endpoint endpoint)
{
  return 0;
}
