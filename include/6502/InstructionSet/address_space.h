#ifndef SIXFIVE_ADDRESSSPACE_H
#define SIXFIVE_ADDRESSSPACE_H

#include <cstdint>

#include "6502/InstructionSet/instrs.h"

/// Each page occupies 256 bytes
static constexpr word_t PAGE_SZ = 256;

/// The entire address space spans 256 pages.
static constexpr word_t NUM_PAGES = 256;

/// Max representable address.
static constexpr word_t MAX_ADDR = 0xFFFF;

/// Number of bytes in the address space.
static constexpr size_t ADDR_SPACE_SZ = MAX_ADDR + 1;

/// A region is some continous, sized chunk of the adress space. For now all
/// regions are page aligned so we technically we only need two bytes to
/// describe its begin location and size. But it's more convenient to represent
/// the beginning in a full word.
struct Region {
  word_t begin;
  uint8_t num_pages;
};

class Regions {
 public:
  static constexpr const Region ZPG = {0x0000, 1};
  static constexpr const Region STACK = {0x0100, 1};
  static constexpr const Region IO = {0x0200, 1};
  static constexpr const Region DISPLAY = {0x0300, 2};
  static constexpr const word_t BOOTLOADER_ADDR =
      DISPLAY.begin + DISPLAY.num_pages * PAGE_SZ;
};

class Display {
 public:
  static constexpr word_t width = 64;
  static constexpr word_t height = 64;
  /// 0[p7, p6, p5, p4, p3, p2, p1, p0] 1[p15, p14, p13, p12, p11, p10, p9, p8]
  static inline bool read_pix(const uint8_t* mem, uint8_t x, uint8_t y) {
    // Each byte holds 8 pixels.
    constexpr word_t N_BYTE_IN_ROW = width / 8;
    word_t byte_addr =
        static_cast<word_t>(x) / 8 + static_cast<word_t>(y) * N_BYTE_IN_ROW;
    uint8_t bit_test = 1 << (7 - (x % 8));
    return mem[byte_addr] & bit_test;
  }
};

class IO {
 public:
  static constexpr word_t mouse_x = Regions::IO.begin | 0x00;
  static constexpr word_t mouse_y = Regions::IO.begin | 0x01;
};

#endif
