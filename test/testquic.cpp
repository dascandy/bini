#include <catch/catch.hpp>
#include <bini/writer.h>
#include <bini/reader.h>

TEST_CASE("Write quic test values", "[QUIC]") {
  std::vector<uint32_t> values = { 0, 63, 64, 16383, 16384, 1073741823, 1073741824, 4294967295 };
  std::vector<uint8_t> target = { 
      0x00, 
      0x3F, 
      0x40, 0x40, 
      0x7F, 0xFF, 
      0x80, 0x00, 0x40, 0x00, 
      0xBF, 0xFF, 0xFF, 0xFF,
      0xC0, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
      0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
  };
  Bini::writer wr;
  for (auto v : values) wr.addQuic(v);
  std::vector<uint8_t> v = std::move(wr);
  REQUIRE(v.size() == target.size());
  for (size_t n = 0; n < target.size(); n++) {
    REQUIRE(v[n] == target[n]);
  }
}

TEST_CASE("Read quic test values", "[QUIC]") {
  std::vector<uint32_t> values = { 0, 63, 64, 16383, 16384, 1073741823, 1073741824, 4294967295 };
  std::vector<uint8_t> target = { 
      0x00, 
      0x3F, 
      0x40, 0x40, 
      0x7F, 0xFF, 
      0x80, 0x00, 0x40, 0x00, 
      0xBF, 0xFF, 0xFF, 0xFF,
      0xC0, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
      0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
  };
  Bini::reader r(target);
  for (auto& v : values) {
    REQUIRE(r.getQuic() == v);
  }
  REQUIRE(r.sizeleft() == 0);
}

