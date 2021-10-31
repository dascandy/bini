#include <catch/catch.hpp>
#include <bini/writer.h>
#include <bini/reader.h>

TEST_CASE("Read 10 3-bit values from a left-shifting bit reader", "[BIT]") {
  std::vector<uint8_t> in = { 0x05, 0x39, 0x77, 0x04 };
  Bini::bitreader rd(in);
  for (size_t n = 0; n < 10; n++) {
    REQUIRE(rd.get(3) == (n & 7));
  }
}

TEST_CASE("Read 10 3-bit values from a right-shifting bit reader", "[BIT]") {
  std::vector<uint8_t> in = { 0x88, 0xc6, 0xfa, 0x08 };
  Bini::bitreader<false> rd(in);
  for (size_t n = 0; n < 10; n++) {
    REQUIRE(rd.get(3) == (n & 7));
  }
}

TEST_CASE("Write and read all 5-bit values", "BIT") {
  Bini::bitwriter w;
  for (size_t n = 0; n < 32; n++) {
    w.add(5, n);
  }
  std::vector<uint8_t> data = std::move(w);
  Bini::bitreader b(data);
  for (size_t n = 0; n < 32; n++) {
    REQUIRE(b.get(5) == n);
  }
}

TEST_CASE("Write 10 3-bit values to a left-shifting bit writer", "[BIT]") {
  Bini::bitwriter wr;
  for (size_t n = 0; n < 10; n++) {
    wr.add(3, n & 0x7);
  }
  std::vector<uint8_t> v = std::move(wr);
  std::vector<uint8_t> target = { 0x05, 0x39, 0x77, 0x04 };
  for (size_t n = 0; n < 4; n++) {
    REQUIRE(v[n] == target[n]);
  }
}

TEST_CASE("Write 10 3-bit values to a right-shifting bit writer", "[BIT]") {
  Bini::bitwriter<false> wr;
  for (size_t n = 0; n < 10; n++) {
    wr.add(3, n & 0x7);
  }
  std::vector<uint8_t> v = std::move(wr);
  std::vector<uint8_t> target = { 0x88, 0xc6, 0xfa, 0x08 };
  for (size_t n = 0; n < 4; n++) {
    REQUIRE(v[n] == target[n]);
  }
}

TEST_CASE("Cornercases", "[BIT]") {
  SECTION("Exact multiple of 8 bits, 32 bits at a time") {
    Bini::bitwriter wr;
    wr.add(32, 0x12345678);
    wr.add(32, 0x9abcdef0);
    std::vector<uint8_t> v = std::move(wr);
    REQUIRE(v.size() == 8);
    Bini::bitreader r(v);
    REQUIRE(r.get(32) == 0x12345678);
    REQUIRE(r.get(32) == 0x9abcdef0);
    REQUIRE(r.bitsleft() == 0);
  }

  SECTION("57 bit value", "[BIT]") {
    Bini::bitwriter wr;
    wr.add(57, 0x13456789abcdef);
    std::vector<uint8_t> v = std::move(wr);
    REQUIRE(v.size() == 8);
    Bini::bitreader r(v);
    REQUIRE(r.get(57) == 0x13456789abcdef);
  }
}



