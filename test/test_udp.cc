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

static void fillData(char* const buffer, uint32_t size, char seed)
{
  for (uint32_t index = 1; index < size; index++) {
    buffer[index] = index + seed + 0x74;
  }
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

TEST(UDP, Large)
{
  constexpr uint16_t PORT = 5000;
  const std::string IP = "127.0.0.1";
  constexpr uint32_t STATES = 3;
  const vector<uint32_t> lengths = {5176, 164009, 867};

  try {
    future<void> taskServer = async(launch::async, []() {
        UdpServer udpServer = UdpServer(PORT);
        udpServer.bind();

        for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
          vector<char> buffer(lengths[testIndex]);
          fillData(buffer, testIndex);
          const auto sentSize = udpServer.send(buffer);
          EXPECT_EQ(sentSize, lengths[testIndex]);
        }
    });

    future<void> taskClient = async(launch::async, []() {
        UdpClient udpClient = UdpClient(IP, PORT);

        for (uint32_t testIndex = 0; testIndex < lengths.size(); testIndex++) {
          const vector<char> data = udpServer->receive(lengths[testIndex]);
          EXPECT_EQ(data.size(), lengths[testIndex]);
          EXPECT_TRUE(verifyData(data));
        }
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
