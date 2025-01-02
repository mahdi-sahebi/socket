#include <memory>
#include <exception>
#include <iostream>
#include <gtest/gtest.h>
#include "socket/udp.h"


// TODO(MN): Exceptions, creation, send small, receive small, small loop back, large loop back, dynamic size loop back, variadic content loopback.
using namespace std;


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
    unique_ptr<Udp> udp = Udp::Builder()setType(Socket::kServer).build();
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

int main()
{
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
