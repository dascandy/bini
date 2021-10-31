#include <catch/catch.hpp>
#include <bini/writer.h>
#include <bini/reader.h>

TEST_CASE("Write PB test values", "[PB]") {
  std::vector<uint64_t> values = { 
    0, 
    127, 
    128, 
    16383, 
    16384, 
    2097151, 
    2097152, 
    268435455, 
    268435456, 
    4294967295 };
  std::vector<uint8_t> target = { 
      0x00,
      0x7F, 
      0x80, 0x01, 
      0xFF, 0x7F, 
      0x80, 0x80, 0x01,
      0xFF, 0xFF, 0x7F,
      0x80, 0x80, 0x80, 0x01,
      0xFF, 0xFF, 0xFF, 0x7F,
      0x80, 0x80, 0x80, 0x80, 0x01,
      0xFF, 0xFF, 0xFF, 0xFF, 0x0F,
  };
  Bini::writer wr;
  for (auto v : values) wr.addPB(v);
  std::vector<uint8_t> v = std::move(wr);
  REQUIRE(v.size() == target.size());
  for (size_t n = 0; n < target.size(); n++) {
    REQUIRE(v[n] == target[n]);
  }
}

TEST_CASE("Read PB test values", "[PB]") {
  std::vector<uint64_t> values = { 0, 127, 128, 16383, 16384, 2097151, 2097152, 268435455, 268435456, 4294967295 };
  std::vector<uint8_t> target = { 
      0x00,
      0x7F, 
      0x80, 0x01, 
      0xFF, 0x7F, 
      0x80, 0x80, 0x01,
      0xFF, 0xFF, 0x7F,
      0x80, 0x80, 0x80, 0x01,
      0xFF, 0xFF, 0xFF, 0x7F,
      0x80, 0x80, 0x80, 0x80, 0x01,
      0xFF, 0xFF, 0xFF, 0xFF, 0x0F,
  };
  Bini::reader r(target);
  for (auto& v : values) {
    REQUIRE(r.getPB() == v);
  }
  REQUIRE(r.sizeleft() == 0);
}

