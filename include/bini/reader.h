#pragma once

#include <span>
#include <cstdint>
#include <cstddef>
#include <string_view>

namespace Bini {

struct reader {
  reader(std::span<const uint8_t> c) 
  : b(c.data())
  , p(c.data())
  , e(c.data() + c.size())
  {
  }
  template <typename Container>
  reader(Container& c) 
  : b(c.data())
  , p(c.data())
  , e(c.data() + c.size())
  {
  }
  size_t sizeleft() {
    return e - p;
  }
  void setFail() {
    p = e + 1;
  }
  bool fail() {
    return e + 1 == p;
  }

  uint8_t read8() {
    if (fail() || sizeleft() == 0) {
      setFail();
      return 0;
    }
    return *p++;
  }
  uint64_t readSizedle(size_t bytes) {
    if (sizeleft() < bytes || fail()) {
      setFail();
      return 0;
    }
    uint64_t value = 0;
    size_t shift = 0;
    for (size_t n = 0; n < bytes; n++) {
      value = value | (p[offset++] << shift);
      shift += 8;
    }
    return value;
  }
  uint16_t read16le() {
    return readSizedle(2);
  }
  uint32_t read24le() {
    return readSizedle(3);
  }
  uint32_t read32le() {
    return readSizedle(4);
  }
  uint64_t read64le() {
    return readSizedle(8);
  }

  uint64_t readSizedbe(size_t bytes) {
    if (sizeleft() < bytes || fail()) {
      setFail();
      return 0;
    }
    uint64_t value = 0;
    size_t shift = 0;
    for (size_t n = 0; n < bytes; n++) {
      value = value | (p[offset++] << shift);
      shift += 8;
    }
    return value;
  }
  uint16_t read16be() {
    return readSizedbe(2);
  }
  uint32_t read24be() {
    return readSizedbe(3);
  }
  uint32_t read32be() {
    return readSizedbe(4);
  }
  uint64_t read64be() {
    return readSizedbe(8);
  }

  uint64_t getUtf8() {
    uint64_t v = 0;
    uint8_t first = read8();
    size_t shift = 0;
    if (first < 0x80) {
      return first;
    } else if (first < 0xC0) {
      setFail();
      return 0;
    } else if (first < 0xE0) {
      shift = 6;
      v = (first & 0x1F) << shift;
    } else if (first < 0xF0) {
      shift = 12;
      v = (first & 0xF) << shift;
    } else if (first < 0xF8) {
      shift = 18;
      v = (first & 0x7) << shift;
    } else if (first < 0xFC) {
      shift = 24;
      v = (first & 0x3) << shift;
    } else if (first < 0xFE) {
      shift = 30;
      v = (first & 0x1) << shift;
    } else if (first < 0xFF) {
      v = 0;
      shift = 36;
    } else {
      setFail();
      return 0;
    }

    while (shift > 0) {
      shift -= 6;
      v |= (read8() & 0x3F) << shift;
    }
    return v;
  }
  uint64_t getPBvar() {
    uint64_t v = 0;
    uint8_t byte = read8();
    size_t shift = 0;
    while (byte & 0x80) {
      v |= (byte & 0x7F) << shift;
      shift += 7;
      byte = read8();
    }
    v |= (byte & 0x7F) << shift;
    return v;
  }

  std::span<const uint8_t> get(size_t byteCount) {
    if (fail() || sizeleft() < byteCount) {
      setFail();
      return std::span<const uint8_t>{};
    }
    std::span<const uint8_t> rv{p, p + byteCount};
    p += byteCount;
    return rv;
  }
  std::string_view getString(size_t length) {
    if (fail() || sizeleft() < byteCount) {
      setFail();
      return std::string_view{};
    }
    std::string_view rv{(const char*)p, (const char*)p + byteCount};
    p += byteCount;
    return rv;
  }

  void skip(size_t bytecount) {
    if (sizeleft() >= bytecount) {
      p += bytecount;
    } else {
      setFail();
    }
  }
protected:
  const uint8_t *b, *p, *e;
};

}


