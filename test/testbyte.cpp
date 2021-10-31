#include <catch/catch.hpp>
#include <bini/writer.h>
#include <bini/reader.h>

TEST_CASE("Write all common big-endian sizes (16, 24, 32, 64)", "[BE]") {
  Bini::writer wr;
  wr.add16be(0x1234);
  wr.add24be(0x123456);
  wr.add32be(0x12345678);
  wr.add64be(0x123456789abcdef0ULL);
  std::vector<uint8_t> v = std::move(wr);
  std::vector<uint8_t> target = { 0x12, 0x34, 0x12, 0x34, 0x56, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };
  for (size_t n = 0; n < target.size(); n++) {
    REQUIRE(v[n] == target[n]);
  }
}

TEST_CASE("Write all common little-endian sizes (16, 24, 32, 64)", "[LE]") {
  Bini::writer wr;
  wr.add16le(0x1234);
  wr.add24le(0x123456);
  wr.add32le(0x12345678);
  wr.add64le(0x123456789abcdef0ULL);
  std::vector<uint8_t> v = std::move(wr);
  std::vector<uint8_t> target = { 0x34, 0x12, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 };
  for (size_t n = 0; n < target.size(); n++) {
    REQUIRE(v[n] == target[n]);
  }
}

TEST_CASE("Read all common big-endian sizes (16, 24, 32, 64)", "[BE]") {
  std::vector<uint8_t> target = { 0x12, 0x34, 0x12, 0x34, 0x56, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };
  Bini::reader r(target);
  REQUIRE(r.read16be() == 0x1234);
  REQUIRE(r.read24be() == 0x123456);
  REQUIRE(r.read32be() == 0x12345678);
  REQUIRE(r.read64be() == 0x123456789abcdef0);
}

TEST_CASE("Read all common little-endian sizes (16, 24, 32, 64)", "[LE]") {
  std::vector<uint8_t> target = { 0x34, 0x12, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 };
  Bini::reader r(target);
  REQUIRE(r.read16le() == 0x1234);
  REQUIRE(r.read24le() == 0x123456);
  REQUIRE(r.read32le() == 0x12345678);
  REQUIRE(r.read64le() == 0x123456789abcdef0);
}

