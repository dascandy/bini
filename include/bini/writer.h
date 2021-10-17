#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <span>
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
  void add64le(uint32_t v) {
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

  template <size_t N>
  void add(const std::array<uint8_t, N>& str) {
    insert(end(), str.begin(), str.end());
  }
  void add(std::span<const uint8_t> str) {
    insert(end(), str.begin(), str.end());
  }
  void add(const std::string& str) {
    insert(end(), str.begin(), str.end());
  }

  void addPBvar(uint64_t v) {
    while (v > 0x7F) {
      push_back((v & 0x7F) | 0x80);
      v >>= 7;
    }
    push_back(v & 0x7F);
  }
  void addUtf8(char32_t c) {
    if (c <= 0x7f) {
      push_back(c);
    } else {
      size_t shift = 0;
      if (c > 0x3ffffff) {
        shift = 30;
        push_back(0xFC | (c >> shift));
      } else if (c > 0x1fffff) {
        shift = 24;
        push_back(0xF8 | (c >> shift));
      } else if (c > 0xffff) {
        shift = 18;
        push_back(0xF0 | (c >> shift));
      } else if (c > 0x7ff) {
        shift = 12;
        push_back(0xE0 | (c >> shift));
      } else {
        shift = 6;
        push_back(0xC0 | (c >> shift));
      }
      while (shift > 0) {
        shift -= 6;
        push_back(0x80 | ((c >> shift) & 0x3F));
      }
    }
  }
  void addpadding(size_t count, uint8_t pad) {
    for (size_t n = 0; n < count; n++) push_back(pad);
  }
};

}


