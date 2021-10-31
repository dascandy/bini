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

TEST_CASE("Reader flags out of bounds", "[VAR]") {
  SECTION("string") {
    std::vector<uint8_t> source = { 0x65, 0x6c, 0x6c, 0x6f };
    Bini::reader r(source);
    REQUIRE(r.getString(5) == "");
    REQUIRE(r.fail());
  }
  SECTION("span") {
    std::vector<uint8_t> source = { 0x65, 0x6c, 0x6c, 0x6f };
    Bini::reader r(source);
    REQUIRE(r.get(5).empty());
    REQUIRE(r.fail());
  }
  SECTION("int value") {
    std::vector<uint8_t> source = { 0x65, 0x6c, 0x6c };
    Bini::reader r(source);
    REQUIRE(r.read32be() == 0);
    REQUIRE(r.fail());
  }
  SECTION("skip beyond end") {
    std::vector<uint8_t> source = { 0x65, 0x6c, 0x6c, 0x6f };
    Bini::reader r(source);
    r.skip(5);
    REQUIRE(r.fail());
  }
  SECTION("unterminated pb") {
    std::vector<uint8_t> source = { 0x95, 0xac, 0xbc, 0xdf };
    Bini::reader r(source);
    REQUIRE(r.getPB() == 0);
    REQUIRE(r.fail());
  }
  SECTION("overlong pb") {
    std::vector<uint8_t> source = { 0x95, 0xac, 0xbc, 0xdf, 0x80, 0x80, 0x80, 0x80, 0x80, 0x42 };
    Bini::reader r(source);
    REQUIRE(r.getPB() == 0);
    REQUIRE(r.fail());
  }
  SECTION("too short quic value") {
    std::vector<uint8_t> source = { 0xc5, 0x6c, 0x6c, 0x6f, 0x42, 0x45, 0x98 };
    Bini::reader r(source);
    REQUIRE(r.getQuic() == 0);
    REQUIRE(r.fail());
  }
}


