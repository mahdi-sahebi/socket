#ifndef SOCKET_WINDOWS_WINSOCK_H_
#define SOCKET_WINDOWS_WINSOCK_H_

#include <memory>
#include <winsock2.h>

class WinSockManager
{
public:
  static std::shared_ptr<WinSockManager> getInstance();
  ~WinSockManager();

private:
  static std::shared_ptr<WinSockManager> instance_;
  WSADATA wsaData_;

  WinSockManager();

};

#endif /* SOCKET_WINDOWS_WINSOCK_H_ */
