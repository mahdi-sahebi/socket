#ifndef SOCKET_WINDOWS_WINSOCK_H_
#define SOCKET_WINDOWS_WINSOCK_H_

#include <memory>

class WinSockManager
{
public:
  static std::shared_ptr<WinSockManager> getInstance();
  ~WinSockManager();

private:
  static std::shared_ptr<WinSockManager> instance_;

  WinSockManager();

};

#endif /* SOCKET_WINDOWS_WINSOCK_H_ */
