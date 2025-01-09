#include <memory>
#include <exception>
#include <iostream>
#include <thread>
#include <future>
#include <iterator>
#include <vector>
#include <gtest/gtest.h>
#include "socket/udp.h"


// TODO(MN): Exceptions, creation, send small, receive small, small loop back, large loop back, dynamic size loop back, variadic content loopback.
using namespace std;


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

  for (uint32_t index = 1; index < size; index++) {
    data[index] = index + seed + 0x74;
  }

  return data;
}

static bool verifyData(const vector<char>& data, char seed)
{
  for (uint32_t index = 1; index < data.size(); index++) {
    if (data[index] != static_cast<char>(index + seed + 0x74)) {
        return false;
    }
  }

  return true;
}


TEST(Creation, ValidDefault)
{
  try {
    unique_ptr<Udp> udp = Udp::Builder().build();
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Creation, ValidServer)
{
  try {
    unique_ptr<Udp> udp = Udp::Builder().setType(Socket::kServer).build();
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Write, Zero)
{
  try {
    unique_ptr<Udp> udp = Udp::Builder()setType(Socket::kServer).build();
    const auto sentSize = udp->write({});
    EXPECT_EQ(sentSize, 0);
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Write, Small)
{
  try {
    unique_ptr<Udp> udp = Udp::Builder().setType(Socket::kServer).build();
    const auto sentSize = udp->write("This is example");
    EXPECT_EQ(sentSize, 15);
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(Write, Large)
{
  try {
    unique_ptr<Udp> udp = Udp::Builder()setType(Socket::kServer).build();

    vector<char> data;
    for (uint32_t index = 0; index < 2400; index++) {
      data.push_back('g');
    }

    const auto sentSize = udp->write(data);
    EXPECT_EQ(sentSize, data.size());
  } catch (const exception& excp) {
    cout << excp.what() << endl;
    FAIL();
  }
}

TEST(large, server_send)
{
  const Port SERVER_PORT = 5000;
  const IP SERVER_IP = "127.0.0.1";
  const uint32_t STATES = 3;
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
