#include <memory>
#include <exception>
#include <iostream>
#include <thread>
#include <future>
#include <iterator>
#include <vector>
#include <chrono>
#include <gtest/gtest.h>
#include "socket/udp.h"


// TODO(MN): Exceptions, creation, send small, receive small, small loop back, large loop back, dynamic size loop back, variadic content loopback.
using namespace std;
using namespace std::this_thread;
using namespace std::chrono;


template <typename T>
constexpr auto maxList(const T* const list, const uint32_t size)
{
  if (0 == size) {
      throw invalid_argument("Zero length array");
  }

  T max = list[0];
  for (uint32_t index = 1; index < size; index++) {
    if (list[index] > max) {
      max = list[index];
    }
  }

  return max;
}

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
    UdpServer server(5000);
  } catch (const exception& e) {
    cout << e.what() << endl;
    FAIL();
  }
}

TEST(Server, InvalidCreation)
{
  try {
    UdpServer server(0);
    FAIL();
  } catch (const UdpSocket::Exception::Port& e) {
    SUCCEED();
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Server, WriteZero)
{
  try {
    UdpServer server(5000);
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
    UdpServer server(5000);
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
    UdpServer server(5000);

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

TEST(Server, LargeReceive)
{
  const Port SERVER_PORT = 5000;
  const IP SERVER_IP = "127.0.0.1";
  const vector<uint32_t> lengths = {176, 16409, 867, 4705};

  try {
    future<void> taskServer = async(launch::async, [SERVER_IP, lengths]() {
        UdpServer server(SERVER_PORT);
        server.open();

        for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
          const auto [data, clientEndpoint] = server.read(lengths[testIndex]);
          EXPECT_EQ(data.size(), lengths[testIndex]);
          EXPECT_TRUE(verifyData(data, testIndex));
        }

        server.close();
    });

    future<void> taskClient = async(launch::async, [SERVER_IP, SERVER_PORT, lengths]() {
        UdpClient client;

        for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
          vector<char> buffer = generateData(lengths[testIndex], testIndex);
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
  }
}

int main()
{
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
