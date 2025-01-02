#include <memory>
#include <exception>
#include <iostream>
#include <thread>
#include <future>
#include <iterator>
#include <gtest/gtest.h>
#include "socket/udp.h"


// TODO(MN): Exceptions, creation, send small, receive small, small loop back, large loop back, dynamic size loop back, variadic content loopback.
using namespace std;


//TEST(Creation, ValidDefault)
//{
//  try {
//    unique_ptr<Udp> udp = Udp::Builder().build();
//  } catch (const exception& excp) {
//    cout << excp.what() << endl;
//    FAIL();
//  }
//}

//TEST(Creation, ValidServer)
//{
//  try {
//    unique_ptr<Udp> udp = Udp::Builder().setType(Socket::kServer).build();
//  } catch (const exception& excp) {
//    cout << excp.what() << endl;
//    FAIL();
//  }
//}

//TEST(Write, Zero)
//{
//  try {
//    unique_ptr<Udp> udp = Udp::Builder()setType(Socket::kServer).build();
//    const auto sentSize = udp->write({});
//    EXPECT_EQ(sentSize, 0);
//  } catch (const exception& excp) {
//    cout << excp.what() << endl;
//    FAIL();
//  }
//}

//TEST(Write, Small)
//{
//  try {
//    unique_ptr<Udp> udp = Udp::Builder().setType(Socket::kServer).build();
//    const auto sentSize = udp->write("This is example");
//    EXPECT_EQ(sentSize, 15);
//  } catch (const exception& excp) {
//    cout << excp.what() << endl;
//    FAIL();
//  }
//}

//TEST(Write, Large)
//{
//  try {
//    unique_ptr<Udp> udp = Udp::Builder()setType(Socket::kServer).build();

//    vector<char> data;
//    for (uint32_t index = 0; index < 2400; index++) {
//      data.push_back('g');
//    }

//    const auto sentSize = udp->write(data);
//    EXPECT_EQ(sentSize, data.size());
//  } catch (const exception& excp) {
//    cout << excp.what() << endl;
//    FAIL();
//  }
//}

template <typename T>
constexpr auto max(const T* const list, const uint32_t size)
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

static void fill(char* const buffer, uint32_t size, char seed)
{
  for (uint32_t index = 1; index < size; index++) {
    buffer[index] = index + seed;
  }
}

static bool verify(const char* const buffer, uint32_t size, char seed)
{
  for (uint32_t index = 1; index < size; index++) {
    if (buffer[index] != static_cast<char>(index + seed)) {
        return false;
    }
  }

  return true;
}

TEST(UDP, Large)
{
  constexpr uint16_t PORT = 5000;
  constexpr uint32_t LENGTH[] = {5176, 164009, 867};
  constexpr uint32_t LENGTH_COUNT = sizeof(LENGTH) / sizeof(*LENGTH);
  constexpr uint32_t STATES = 3;
  constexpr uint32_t MAX_LENGTH = max(LENGTH, LENGTH_COUNT);

  try {
    future<void> taskServer = async(launch::async, []() {
        UdpServer udpServer = UdpServer::Builder().build();
        udpServer.bind(PORT);

        char buffer[MAX_LENGTH];
        uint32_t testIndex = LENGTH_COUNT;

        while (testIndex--) {
          fill(buffer, LENGTH[testIndex], testIndex + 0x74);
          const auto sentSize = udpServer.send(vector<char>(begin(buffer), begin(buffer) + LENGTH[testIndex]));
          EXPECT_EQ(sentSize, LENGTH[testIndex]);
        }
    });

    future<void> taskClient = async(launch::async, []() {
        UdpClient udpClient = UdpClient::Builder().build();

        uint32_t testIndex = LENGTH_CONT;
        while (testIndex--) {
          const vector<char> data = udpServer->receive(LENGTH[testIndex]);
          EXPECT_EQ(data.size(), LENGTH[testIndex]);
          EXPECT_TRUE(verify(data.data(), data.size()));
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
