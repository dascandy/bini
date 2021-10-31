#pragma once

#include <span>
#include <cstdint>
#include <cstddef>
#include <string_view>

namespace Bini {

struct reader {
  reader(std::span<const uint8_t> c) 
  : p(c.data())
  , e(c.data() + c.size())
  {
  }
  template <typename Container>
  reader(Container& c) 
  : p(c.data())
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
      value = value | ((uint64_t)(*p++) << shift);
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
    for (size_t n = 0; n < bytes; n++) {
      value = (value << 8) | (*p++);
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

  uint64_t getPBvar() {
    uint64_t v = 0;
    uint8_t byte = read8();
    size_t shift = 0;
    while (byte & 0x80) {
      v |= (byte & 0x7F) << shift;
      shift += 7;
      if (shift == 63) {
        setFail();
        return 0;
      }
      byte = read8();
    }
    v |= (byte & 0x7F) << shift;
    return v;
  }
  uint64_t getQuic() {
    uint64_t v = read8();
    uint8_t type = (v & 0xC0) >> 6;
    v = (v & 0x3F);
    switch(type) {
    case 3:
      v = (v << 8) | read8();
      v = (v << 8) | read8();
      v = (v << 8) | read8();
      v = (v << 8) | read8();
    case 2:
      v = (v << 8) | read8();
      v = (v << 8) | read8();
    case 1:
      v = (v << 8) | read8();
    case 0:
      break;
    }
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
    if (fail() || sizeleft() < length) {
      setFail();
      return std::string_view{};
    }
    std::string_view rv{(const char*)p, length};
    p += length;
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
  const uint8_t *p, *e;
};

template <bool shiftLeft = true>
struct bitreader {
  bitreader(std::span<const uint8_t> c)
  : current(0)
  , currentBits(0)
  , p(c.data())
  , e(c.data() + c.size())
  {
  }
  template <typename Container>
  bitreader(Container& c)
  : current(0)
  , currentBits(0)
  , p(c.data())
  , e(c.data() + c.size())
  {
  }
  size_t bitsleft() {
    return 8*(e - p) + currentBits;
  }
  void setFail() {
    p = e + 1;
  }
  bool fail() {
    return e + 1 == p;
  }
  void refill() {
    if (fail()) return;
    while (e != p && currentBits < 57) {
      if constexpr (shiftLeft) {
        current = (current << 8) | (*p++);
      } else {
        current |= ((*p++) << currentBits);
      }
      currentBits += 8;
    }
  }
  uint64_t get(size_t bits) {
    // since our accumulator is 64 bits and we might have a byte with only 1 bit left we cannot guarantee more than 57 bits.
    if (fail() || bits > 57) {
      setFail();
      return 0;
    }
    if (currentBits < bits) {
      refill();
    }
    if (fail() || currentBits < bits) return 0;

    currentBits -= bits;
    if constexpr (shiftLeft) {
      uint64_t rv = (current >> currentBits) & ((1ULL << bits) - 1);
      return rv;
    } else {
      uint64_t rv = current & ((1ULL << (bits)) - 1);
      current >>= bits;
      return rv;
    }
  }
  uint64_t current;
  uint8_t currentBits;
  const uint8_t *p, *e;
};

}


