#include <catch/catch.hpp>
#include <bini/writer.h>
#include <bini/reader.h>

TEST_CASE("Write primitive types", "[VAR]") {
  Bini::writer wr;
  std::string hello = "hello";
  std::vector<const uint8_t> data = { 0x8f, 0x45, 0x99, 0x00 };
  wr.add(hello);
  wr.add(data);
  wr.addpadding(0x20, 0x0);
  wr.add8(0x2a);
  std::vector<uint8_t> v = std::move(wr);
  std::vector<uint8_t> target = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x8f, 0x45, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a };
  REQUIRE(v.size() == target.size());
  for (size_t n = 0; n < target.size(); n++) {
    REQUIRE(v[n] == target[n]);
  }
}

TEST_CASE("Read primitive types", "[VAR]") {
  std::vector<uint8_t> target = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x8f, 0x45, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a };
  Bini::reader r(target);
  REQUIRE(r.getString(5) == "hello");
  std::vector<const uint8_t> data = { 0x8f, 0x45, 0x99, 0x00 };
  auto d = r.get(4);
  REQUIRE(d.size() == data.size());
  for (size_t n = 0; n < data.size(); n++) {
    REQUIRE(d[n] == data[n]);
  }
  r.skip(0x20);
  REQUIRE(r.read8() == 0x2a);
}

