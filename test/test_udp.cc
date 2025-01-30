#include <memory>
#include <exception>
#include <iostream>
#include <thread>
#include <future>
#include <iterator>
#include <vector>
#include <chrono>
#include <gtest/gtest.h>
#include "socket/udp_client.h"
#include "socket/udp_server.h"


// TODO(MN): Exceptions, creation, send small, receive small, small loop back, large loop back, dynamic size loop back, variadic content loopback.
// TODO(MN: Tests of timeout 0
using namespace std;
using namespace std::this_thread;
using namespace std::chrono;


static vector<char> generateData(uint32_t size, char seed)
{
  vector<char> data(size);

  data[0] = seed;
  for (uint32_t index = 1; index < size; index++) {
    data[index] = static_cast<char>(index + seed + 0x74);
  }

  return data;
}

static bool verifyData(const vector<char>& data, char seed)
{
  if (data[0] != seed) {
      return false;
  }

  for (uint32_t index = 1; index < data.size(); index++) {
    if (data[index] != static_cast<char>(index + seed + 0x74)) {
        return false;
    }
  }

  return true;
}

TEST(Server, ValidCreation)
{
  try {
    UdpServer server;
  } catch (const exception& e) {
    cout << e.what() << endl;
    FAIL();
  }
}

TEST(Server, InvalidPort)
{
  try {
    UdpServer server;
    server.open();
    server.bind(0);
    FAIL();
  } catch (const UdpInterface::Exception::Port& e) {
    SUCCEED();
  } catch (const exception& e) {
    cout << e.what() << endl;
    FAIL();
  }
}

TEST(Server, BindNotOpened)
{
  try {
    UdpServer server;
    server.bind(500);
    FAIL();
  } catch (const UdpInterface::Exception::Open& e) {
    SUCCEED();
  } catch (const exception& e) {
    cout << e.what() << endl;
    FAIL();
  }
}

TEST(Server, WriteZero)
{
  try {
    UdpServer server;
    server.open();
    server.bind(5000);
    const auto sentSize = server.write({}, Endpoint("127.0.0.1", 5000));
    EXPECT_EQ(sentSize, 0);
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Server, WriteSmall)
{
  try {
    UdpServer server;
    server.open();
    server.bind(5000);
    const string text = "This is example";
    const vector<char> data(begin(text), end(text));
    const auto sentSize = server.write(data, Endpoint("127.0.0.1", 5000));
    EXPECT_EQ(sentSize, text.size());
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Server, Write)
{
  try {
    UdpServer server;
    server.open();
    server.bind(5000);

    vector<char> data;
    for (uint32_t index = 0; index < 2400; index++) {
      data.push_back('g');
    }

    const auto sentSize = server.write(data, Endpoint("127.0.0.1", 5000));
    EXPECT_EQ(sentSize, data.size());
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Server, LargeReceive1)
{
  const Port SERVER_PORT = 5000;
  const IP SERVER_IP = "127.0.0.1";
  const vector<uint32_t> lengths = {176, 1865507, 867, 4705, 8, 98985};

  try {
    future<void> taskServer = async(launch::async, [SERVER_IP, lengths]() {
        UdpServer server;
        server.open();
        server.bind(SERVER_PORT);

        for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
          tuple<Data, Endpoint> result;
          do {
            result = server.read(lengths[testIndex], 200000);
          } while (std::get<0>(result).size() == 0);

          const auto& [data, endpoint] = result;
          EXPECT_EQ(data.size(), lengths[testIndex]);
          EXPECT_TRUE(verifyData(data, testIndex));// TODO(MN): check endpoint
        }

        server.close();
    });

    future<void> taskClient = async(launch::async, [SERVER_IP, SERVER_PORT, lengths]() {
        UdpClient client;
        client.open();

        sleep_for(milliseconds(100));

        for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
          const vector<char> buffer = generateData(lengths[testIndex], testIndex);
          const auto sentSize = client.write(buffer, Endpoint(SERVER_IP, SERVER_PORT));
          EXPECT_EQ(sentSize, lengths[testIndex]);
        }

        client.close();
    });

    taskServer.get();
    taskClient.get();
  } catch (const exception& excp) {
    std::cout << excp.what() << std::endl;
    FAIL();
  } catch (...) {
    std::cout << "Unknown exception" << std::endl;
  }
}

TEST(Server, LargeReceive2)
{
  const Port SERVER_PORT = 5000;
  const IP SERVER_IP = "127.0.0.1";
  const vector<uint32_t> lengths = {176, 865507, 867, 4705, 8, 98985};

  try {
    future<void> taskServer = async(launch::async, [SERVER_IP, lengths]() {
      UdpServer server;
      server.open();
      server.bind(SERVER_PORT);

      for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
        Data data(lengths[testIndex], 0);
        const auto receivedSize = server.read(data.data(), lengths[testIndex], 200000);
        EXPECT_EQ(receivedSize, lengths[testIndex]);
        EXPECT_EQ(data.size(), lengths[testIndex]);
        EXPECT_TRUE(verifyData(data, testIndex));// TODO(MN): check endpoint
      }

      server.close();
    });

    future<void> taskClient = async(launch::async, [SERVER_IP, SERVER_PORT, lengths]() {
        UdpClient client(Endpoint(SERVER_IP, SERVER_PORT));
        client.open();

        sleep_for(milliseconds(100));

        for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
          const vector<char> buffer = generateData(lengths[testIndex], testIndex);
          const auto sentSize = client.write(buffer.data(), buffer.size());
          EXPECT_EQ(sentSize, lengths[testIndex]);
        }

        client.close();
    });

    taskServer.get();
    taskClient.get();
  } catch (const exception& excp) {
    std::cout << excp.what() << std::endl;
    FAIL();
  } catch (...) {
    std::cout << "Unknown exception" << std::endl;
  }
}

int main()
{
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
