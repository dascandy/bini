#include <catch/catch.hpp>
#include <bini/writer.h>
#include <bini/reader.h>

std::vector<std::pair<double, std::vector<uint8_t>>> values = {
  { -std::numeric_limits<double>::infinity(), { 0xfc, 0x00, 0xff, 0x80, 0xff, 0x80, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x80, 0xff, 0x00, 0x00, 0x80, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff } },
  { -200, { 0xda, 0x40, 0xc3, 0x48, 0xc3, 0x48, 0x00, 0x00, 0xc0, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xda, 0x48, 0xc3, 0x00, 0x00, 0x48, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x69, 0xc0 } },
  { -0.125, { 0xb0, 0x00, 0xbe, 0x00, 0xbe, 0x00, 0x00, 0x00, 0xbf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x00, 0xbe, 0x00, 0x00, 0x00, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xbf } },
  { -0., { 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 } },
  { 0., { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
  { 0.125, { 0x30, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x3f } },
  { 200, { 0x5a, 0x40, 0x43, 0x48, 0x43, 0x48, 0x00, 0x00, 0x40, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x5a, 0x48, 0x43, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x69, 0x40 } },
  { std::numeric_limits<double>::infinity(), { 0x7c, 0x00, 0x7f, 0x80, 0x7f, 0x80, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x80, 0x7f, 0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7f } },
  { std::numeric_limits<double>::quiet_NaN(), { 0x7e, 0x00, 0x7f, 0xc0, 0x7f, 0xc0, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0xc0, 0x7f, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x7f } },
  { std::numeric_limits<double>::signaling_NaN(), { 0x7f, 0x00, 0x7f, 0xe0, 0x7f, 0xe0, 0x00, 0x00, 0x7f, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xe0, 0x7f, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x7f } },
};

void write_and_read(double value, std::vector<uint8_t> v) {
  {
    Bini::writer wr;
    wr.addfp16be(value);
    wr.addbfp16be(value);
    wr.addfp32be(value);
    wr.addfp64be(value);
    wr.addfp16le(value);
    wr.addbfp16le(value);
    wr.addfp32le(value);
    wr.addfp64le(value);
    CHECK(wr == v);
  }

  {
    Bini::reader r(v);
    decltype(r.readfp16be()) va1 = r.readfp16be(), va2 = value;
    CHECK(memcmp(&va1, &va2, sizeof(va1)) == 0);
    decltype(r.readbfp16be()) vb1 = r.readbfp16be(), vb2 = value;
    CHECK(memcmp(&vb1, &vb2, sizeof(vb1)) == 0);
    decltype(r.readfp32be()) vc1 = r.readfp32be(), vc2 = value;
    CHECK(memcmp(&vc1, &vc2, sizeof(vc1)) == 0);
    decltype(r.readfp64be()) vd1 = r.readfp64be(), vd2 = value;
    CHECK(memcmp(&vd1, &vd2, sizeof(vd1)) == 0);
    decltype(r.readfp16le()) ve1 = r.readfp16le(), ve2 = value;
    CHECK(memcmp(&ve1, &ve2, sizeof(ve1)) == 0);
    decltype(r.readbfp16le()) vf1 = r.readbfp16le(), vf2 = value;
    CHECK(memcmp(&vf1, &vf2, sizeof(vf1)) == 0);
    decltype(r.readfp32le()) vg1 = r.readfp32le(), vg2 = value;
    CHECK(memcmp(&vg1, &vg2, sizeof(vg1)) == 0);
    decltype(r.readfp64le()) vh1 = r.readfp64le(), vh2 = value;
    CHECK(memcmp(&vh1, &vh2, sizeof(vh1)) == 0);
  }
}

TEST_CASE("float test", "[float]") {
  for (auto& [value, binary] : values) {
    write_and_read(value, binary);
  }
}

TEST_CASE("fp16 denormals", "[float]") {
  const float min_denorm_fp16 = 5.9604645e-8;
  const float min_norm_fp16 = 6.1035156e-5;
  Bini::writer wr;
  wr.addfp16be(min_denorm_fp16 / 2);
  wr.addfp16be(min_denorm_fp16);
  wr.addfp16be(min_denorm_fp16 * 2);
  wr.addfp16be(min_norm_fp16);
  std::vector<uint8_t> bytes = { 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x04, 0x00 };
  CHECK(bytes == wr);
  Bini::reader r(bytes);
  CHECK(r.readfp16be() == 0.0f);
  CHECK(r.readfp16be() == min_denorm_fp16);
  CHECK(r.readfp16be() == min_denorm_fp16 * 2);
  CHECK(r.readfp16be() == min_norm_fp16);
}


