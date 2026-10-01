#include "socket/tcp_server.h"
#include "win_sock.h"
#include <winsock2.h>
#include <ws2tcpip.h>

namespace { constexpr Socket invalidSocket = static_cast<Socket>(INVALID_SOCKET); SOCKET native(Socket s) { return static_cast<SOCKET>(s); }
uint32_t readSocket(Socket s,char* out,uint32_t size,uint32_t timeout){uint32_t total=0;while(total<size){if(timeout){fd_set set;FD_ZERO(&set);FD_SET(native(s),&set);timeval tv{static_cast<long>(timeout/1000000),static_cast<long>(timeout%1000000)};if(select(0,&set,nullptr,nullptr,&tv)<=0)break;}int n=recv(native(s),out+total,size-total,0);if(n<=0)break;total+=static_cast<uint32_t>(n);}return total;}
uint32_t writeSocket(Socket s,const char* data,uint32_t size){uint32_t total=0;while(total<size){int n=send(native(s),data+total,size-total,0);if(n<=0)break;total+=static_cast<uint32_t>(n);}return total;}}
TcpServer::TcpServer():socket_{invalidSocket},clientSocket_{invalidSocket}{WinSockManager::getInstance();}
TcpServer::~TcpServer(){if(isOpen())close();}
void TcpServer::open(){if(isOpen())throw TcpInterface::Exception::Close("Socket is already opened");socket_=static_cast<Socket>(socket(AF_INET,SOCK_STREAM,IPPROTO_TCP));if(socket_==invalidSocket)throw TcpInterface::Exception::Open("Create socket failed");BOOL reuse=TRUE;setsockopt(native(socket_),SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&reuse),sizeof(reuse));}
void TcpServer::bind(Port port){if(!isOpen())throw TcpInterface::Exception::Open("Not opened");if(!port)throw TcpInterface::Exception::Port("Invalid port");sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(INADDR_ANY);a.sin_port=htons(port);if(::bind(native(socket_),reinterpret_cast<sockaddr*>(&a),sizeof(a))==SOCKET_ERROR||listen(native(socket_),1)==SOCKET_ERROR)throw TcpInterface::Exception::Bind("Bind or listen failed");}
void TcpServer::accept(){if(!isOpen())throw TcpInterface::Exception::Open("Not opened");if(clientSocket_!=invalidSocket)closesocket(native(clientSocket_));SOCKET s=::accept(native(socket_),nullptr,nullptr);if(s==INVALID_SOCKET)throw TcpInterface::Exception::Accept("Accept failed");clientSocket_=static_cast<Socket>(s);}
void TcpServer::close(){if(!isOpen())throw TcpInterface::Exception::Close("Socket is not opened");if(clientSocket_!=invalidSocket){closesocket(native(clientSocket_));clientSocket_=invalidSocket;}closesocket(native(socket_));socket_=invalidSocket;}
bool TcpServer::isOpen()const{return socket_!=invalidSocket;}
uint32_t TcpServer::read(char* const out,uint32_t size,uint32_t timeout)const{if(clientSocket_==invalidSocket)throw TcpInterface::Exception::Io("No client is connected");return readSocket(clientSocket_,out,size,timeout);}
uint32_t TcpServer::write(const char* const data,uint32_t size)const{if(clientSocket_==invalidSocket)throw TcpInterface::Exception::Io("No client is connected");return writeSocket(clientSocket_,data,size);}
uint32_t TcpServer::read(Data& d,uint32_t t)const{return read(d.data(),static_cast<uint32_t>(d.size()),t);}uint32_t TcpServer::write(const Data& d)const{return write(d.data(),static_cast<uint32_t>(d.size()));}
