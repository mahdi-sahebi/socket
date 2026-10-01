#include <gtest/gtest.h>
#include <future>
#include <thread>
#include "socket/tcp_client.h"
#include "socket/tcp_server.h"

namespace {
const Port port = 5601;
Data payload(size_t n) { Data data(n); for (size_t i=0; i<n; ++i) data[i] = static_cast<char>(i * 31 + 7); return data; }
}

TEST(Tcp, LifecycleAndInvalidPort) {
  TcpServer server;
  EXPECT_FALSE(server.isOpen());
  EXPECT_THROW(server.bind(port), TcpInterface::Exception::Open);
  server.open();
  EXPECT_TRUE(server.isOpen());
  EXPECT_THROW(server.bind(0), TcpInterface::Exception::Port);
  server.close();
  EXPECT_FALSE(server.isOpen());
}

TEST(Tcp, RequestResponseAndLargePayload) {
  const Data request = payload(1024 * 1024 + 19);
  const Data response = payload(8193);
  auto serverTask = std::async(std::launch::async, [&] {
    TcpServer server; server.open(); server.bind(port); server.accept();
    Data received(request.size());
    EXPECT_EQ(server.read(received, 2000000), request.size());
    EXPECT_EQ(received, request);
    EXPECT_EQ(server.write(response), response.size());
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  TcpClient client(Endpoint("127.0.0.1", port)); client.open(); client.connect(Endpoint("127.0.0.1", port));
  EXPECT_EQ(client.write(request), request.size());
  Data received(response.size());
  EXPECT_EQ(client.read(received, 2000000), response.size());
  EXPECT_EQ(received, response);
  client.close(); serverTask.get();
}

TEST(Tcp, TimeoutAndZeroLength) {
  auto serverTask = std::async(std::launch::async, [] {
    TcpServer server; server.open(); server.bind(port); server.accept();
    char buffer[4]{};
    EXPECT_EQ(server.read(buffer, sizeof(buffer), 10000), 0u);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  TcpClient client(Endpoint("127.0.0.1", port)); client.open(); client.connect(Endpoint("127.0.0.1", port));
  EXPECT_EQ(client.write(nullptr, 0), 0u);
  client.close(); serverTask.get();
}
