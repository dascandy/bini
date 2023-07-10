#pragma once

#if __has_include (<stdfloat>)
#include <stdfloat>
#endif
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
  reader(const reader& r) = default;
  template <typename Container>
  reader(const Container& c) 
  : p(c.data())
  , e(c.data() + c.size())
  {
  }
  size_t sizeleft() {
    return e - p;
  }
  bool empty() 
  {
    // Also checks for fail, so you can use it as "while not empty".
    return e >= p;
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

#ifdef __STDCPP_BFLOAT16_T__
  bfloat16_t readbfp16le() {
    uint16_t val = (uint16_t)read16le();
    bfloat16_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  bfloat16_t readbfp16be() {
    uint16_t val = (uint16_t)read16le();
    bfloat16_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
#else
  float readbfp16le() {
    uint32_t val = (uint32_t)read16le() << 16;
    float rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  float readbfp16be() {
    uint32_t val = (uint32_t)read16be() << 16;
    float rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
#endif

#ifdef __STDCPP_FLOAT16_T__
  float16_t readfp16le() {
    uint16_t val = read16le();
    float16_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  float16_t readfp16be() {
    uint16_t val = read16be();
    float16_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
#else
  float fp16tofloat(uint16_t val) {
    uint32_t sign = (val & 0x8000) << 16;
    uint32_t exponent = ((val & 0x7C00) >> 10) + 112;
    uint32_t mantissa = (uint32_t(val) & 0x03FF) << 13;
    if (exponent == 112) {
      if (mantissa == 0) {
        // Zeroes
        exponent = 0;
      } else {
        // Denormals
        // We need to do more work for this, as numbers that are denormal in fp16 are normal in fp32
        while ((mantissa & 0x800000) == 0) {
          mantissa <<= 1;
          exponent--;
        }
        exponent++;
        mantissa &= 0x7FFFFF;
      }
    } else if (exponent == 143) {
      // Infinities and NANs
      exponent = 255;
    } else {
      // Regular numbers
      // No other work needed
    }
    float rv;
    uint32_t value = sign | (exponent << 23) | mantissa;
    memcpy(&rv, &value, 4);
    return rv;
  }
  float readfp16le() {
    return fp16tofloat(read16le());
  }
  float readfp16be() {
    return fp16tofloat(read16be());
  }
#endif

#ifdef __STDCPP_FLOAT32_T__
  float32_t readfp32le() {
    uint32_t val = read32le();
    float32_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  float32_t readfp32be() {
    uint32_t val = read32be();
    float32_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
#else
  float readfp32le() {
    uint32_t val = read32le();
    float rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  float readfp32be() {
    uint32_t val = read32be();
    float rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
#endif

  float readfloatle() {
    uint32_t val = read32le();
    float rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  float readfloatbe() {
    uint32_t val = read32be();
    float rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }

#ifdef __STDCPP_FLOAT64_T__
  float64_t readfp64le() {
    uint64_t val = read64le();
    float64_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  float64_t readfp64be() {
    uint64_t val = read64be();
    float64_t rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
#else
  double readfp64le() {
    uint64_t val = read64le();
    double rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  double readfp64be() {
    uint64_t val = read64be();
    double rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
#endif

  double readdoublele() {
    uint64_t val = read64le();
    double rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }
  double readdoublebe() {
    uint64_t val = read64be();
    double rv;
    memcpy(&rv, &val, sizeof(rv));
    return rv;
  }

  uint64_t getPB() {
    uint64_t v = 0;
    uint8_t byte = read8();
    size_t shift = 0;
    while (not fail() && byte & 0x80) {
      v |= (uint64_t)(byte & 0x7F) << shift;
      shift += 7;
      if (shift == 63) {
        setFail();
        return 0;
      }
      byte = read8();
    }
    if (fail()) return 0;
    v |= (uint64_t)(byte & 0x7F) << shift;
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
    return fail() ? 0 : v;
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
  template <size_t N>
  std::array<uint8_t, N> getArray() {
    std::array<uint8_t, N> rv;
    if (fail() || sizeleft() < N) {
      setFail();
      return rv;
    }
    memcpy(rv.data(), p, N);
    p += N;
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
  std::string_view getStringNT() {
    if (fail()) {
      return std::string_view{};
    }
    char* end = (char*)memchr((char*)p, '\0', e-p);
    if (not end) {
      setFail();
      return std::string_view{};
    }
    std::string_view rv{(const char*)p, size_t(end - (const char*)p)};
    p = (uint8_t*)end + 1;
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


