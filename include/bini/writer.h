#pragma once

#if __has_include (<stdfloat>)
#include <stdfloat>
#endif
#include <span>
#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace Bini {

struct writer : std::vector<uint8_t> {
  void add8(uint8_t v) {
    push_back(v);
  }

  void addSizedle(uint64_t v, uint8_t bytes) {
    size_t shift = 0;
    while (bytes-- > 0) {
      push_back((v >> shift) & 0xFF);
      shift += 8;
    }
  }
  void add16le(uint16_t v) {
    addSizedle(v, 2);
  }
  void add24le(uint32_t v) {
    addSizedle(v, 3);
  }
  void add32le(uint32_t v) {
    addSizedle(v, 4);
  }
  void add64le(uint64_t v) {
    addSizedle(v, 8);
  }

  void addSizedbe(uint64_t v, uint8_t bytes) {
    size_t shift = bytes * 8;
    while (shift > 0) {
      shift -= 8;
      push_back((v >> shift) & 0xFF);
    }
  }
  void add16be(uint16_t v) {
    addSizedbe(v, 2);
  }
  void add24be(uint32_t v) {
    addSizedbe(v, 3);
  }
  void add32be(uint32_t v) {
    addSizedbe(v, 4);
  }
  void add64be(uint64_t v) {
    addSizedbe(v, 8);
  }

#ifdef __STDCPP_BFLOAT16_T__
  void addbfp16le(bfloat16_t val) {
    uint16_t v;
    memcpy(&v, &val, sizeof(v));
    add16le(v);
  }
  void addbfp16be(bfloat16_t val) {
    uint16_t v;
    memcpy(&v, &val, sizeof(v));
    add16be(v);
  }
#else
  void addbfp16le(float val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add16le(v >> 16);
  }
  void addbfp16be(float val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add16be(v >> 16);
  }
#endif

#ifdef __STDCPP_FLOAT16_T__
  void addfp16le(float16_t val) {
    uint16_t v;
    memcpy(&v, &val, sizeof(val));
    add16le(v);
  }
  void addfp16be(float16_t val) {
    uint16_t v;
    memcpy(&v, &val, sizeof(val));
    add16be(v);
  }
#else
  void addfp16le(float val) {
    add16le(floattofp16(val));
  }
  void addfp16be(float val) {
    add16be(floattofp16(val));
  }
  uint16_t floattofp16(float val) {
    uint32_t rawval;
    memcpy(&rawval, &val, sizeof(val));
    uint32_t sign = (rawval & 0x80000000) >> 16;
    uint32_t exponent = (rawval & 0x7f800000) >> 23;
    uint32_t mantissa = (rawval & 0x007FFFFF) >> 13;
    if (exponent < 102) {
      // Zero, or a number too small to represent (so round to zero).
      mantissa = 0;
      exponent = 0;
    } else if (exponent <= 112) {
      // Make denormal
      mantissa |= 0x400;
      while (exponent <= 112) {
        mantissa >>= 1;
        exponent++;
      }
      exponent -= 113;
    } else if (exponent < 142) {
      exponent -= 112;
    } else if (exponent == 255) {
      // Infinity or NaN
      exponent = 31;
    } else {
      // number too large to represent, round up to infinity
      exponent = 31;
      mantissa = 0;
    }
    return sign | (exponent << 10) | mantissa;
  }
#endif

#ifdef __STDCPP_FLOAT32_T__
  void addfp32le(float32_t val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add32le(v);
  }
  void addfp32be(float32_t val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add32be(v);
  }
#else
  void addfp32le(float val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add32le(v);
  }
  void addfp32be(float val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add32be(v);
  }
#endif

  static_assert(sizeof(float) == 4);
  void addfloatle(float val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add32le(v);
  }
  void addfloatbe(float val) {
    uint32_t v;
    memcpy(&v, &val, sizeof(v));
    add32be(v);
  }

#ifdef __STDCPP_FLOAT64_T__
  void addfp64le(float64_t val) {
    uint64_t v;
    memcpy(&v, &val, sizeof(v));
    add64le(v);
  }
  void addfp64be(float64_t val) {
    uint64_t v;
    memcpy(&v, &val, sizeof(v));
    add64be(v);
  }
#else
  void addfp64le(double val) {
    uint64_t v;
    memcpy(&v, &val, sizeof(v));
    add64le(v);
  }
  void addfp64be(double val) {
    uint64_t v;
    memcpy(&v, &val, sizeof(v));
    add64be(v);
  }
#endif

  static_assert(sizeof(double) == 8);
  void adddoublele(double val) {
    uint64_t v;
    memcpy(&v, &val, sizeof(v));
    add64le(v);
  }
  void adddoublebe(double val) {
    uint64_t v;
    memcpy(&v, &val, sizeof(v));
    add64be(v);
  }

  template <size_t N>
  void add(const std::array<uint8_t, N>& str) {
    insert(end(), str.begin(), str.end());
  }
  void add(std::span<const uint8_t> str) {
    insert(end(), str.begin(), str.end());
  }
  void add(std::string_view str) {
    insert(end(), str.begin(), str.end());
  }
  void addNT(std::string_view str) {
    insert(end(), str.begin(), str.end());
    push_back('\0');
  }

  void addPB(uint64_t v) {
    while (v > 0x7F) {
      push_back((v & 0x7F) | 0x80);
      v >>= 7;
    }
    push_back(v & 0x7F);
  }
  void addQuic(uint64_t v) {
    if (v < 16384) {
      if (v < 64) {
        push_back(v);
      } else {
        push_back(0x40 | (v >> 8));
        push_back(v & 0xFF);
      }
    } else {
      if (v < (1 << 30)) {
        push_back(0x80 | (v >> 24));
        push_back((v >> 16) & 0xFF);
        push_back((v >> 8) & 0xFF);
        push_back((v >> 0) & 0xFF);
      } else {
        push_back(0xC0 | (v >> 56));
        push_back((v >> 48) & 0xFF);
        push_back((v >> 40) & 0xFF);
        push_back((v >> 32) & 0xFF);
        push_back((v >> 24) & 0xFF);
        push_back((v >> 16) & 0xFF);
        push_back((v >> 8) & 0xFF);
        push_back((v >> 0) & 0xFF);
      }
    }
  }

  void addpadding(size_t count, uint8_t pad) {
    for (size_t n = 0; n < count; n++) push_back(pad);
  }
};

template <bool shiftLeft = true>
struct bitwriter {
  bitwriter() 
  : current(0)
  , currentBits(0)
  {
  }
  operator std::vector<uint8_t>() && {
    finalFlush();
    return std::move(v);
  }
  void add(size_t bits, uint64_t value) {
    if (bits > 57) {
      return;
    }
    if (currentBits + bits > 64) {
      flush();
    }

    if constexpr (shiftLeft) {
      current = (current << bits) | value;
    } else {
      current |= (value << currentBits);
    }
    currentBits += bits;
  }
private:
  void finalFlush() {
    if (currentBits & 7) {
      add(8 - (currentBits & 7), 0);
    }
    if (currentBits)
      flush();
  }
  void flush() {
    while (currentBits >= 8) {
      if constexpr (shiftLeft) {
        v.push_back((current >> (currentBits - 8)) & 0xFF);
      } else {
        v.push_back(current & 0xFF);
        current >>= 8;
      }
      currentBits -= 8;
    }
  }
  uint64_t current;
  uint8_t currentBits;
  std::vector<uint8_t> v;
};

}


