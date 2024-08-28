#ifndef SIXFIVE_MICROPROCESSOR_H
#define SIXFIVE_MICROPROCESSOR_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <vector>

#include "6502/InstructionSet/instrs.h"

class Processor {
  std::vector<uint8_t> RAM;

  /// Page 0 is used as an addressing mode, and page 1 is used as the processor
  /// stack. Therefore, we expect executable code to begin at the very beginning
  /// of page 2.
  word_t static constexpr BOOTLOADER_ADDR = 0x0200;

  /// Program counter
  word_t PC = BOOTLOADER_ADDR;
  /// Accumulator
  uint8_t AC = 0;
  /// Index register X
  uint8_t X = 0;
  /// Index register Y
  uint8_t Y = 0;
  /// Status register
  struct StatusRegister {
    bool N : 1;  // Negative
    bool V : 1;  // Overflow
    bool _ : 1;  // Unused
    bool B : 1;  // Break
    bool D : 1;  // Decimal (use BCD for arithmetics)
    bool I : 1;  // Interrupt (IRQ disable)
    bool Z : 1;  // Zero
    bool C : 1;  // Carry
  } SR{};
  static_assert(sizeof(StatusRegister) == 1);
  /// Stack pointer. This stores the low-byte of an address into the processor
  /// stack. The processor stack is page 1 of the address space, so the high
  /// byte is always assumed to be 0x01.
  uint8_t SP = 0xFF;

 public:
  Processor(std::vector<uint8_t> &&code);

  /// Run code until completion. \return the final value of the accumulator
  /// register.
  uint8_t run();

 private:
  /// Used throughout different execute cases.
  word_t effective_address = 0;
  /// Used for different execute cases in zero page mode. This ensures that we
  /// can only access the zero page.
  uint8_t zeffective_address = 0;

  /// Read a single byte from anywhere memory.
  inline uint8_t read(word_t addr) { return RAM[addr]; }
  /// Read a word from anywhere in memory.
  inline word_t read_word(word_t addr) {
    return (word_t)RAM[addr] | ((word_t)RAM[addr + 1] << 8);
  }
  /// Read a word from the zero page. Will wrap around if read at 0xFF.
  inline word_t zread_word(uint8_t addr) {
    return (word_t)RAM[addr] | ((word_t)RAM[addr + 1] << 8);
  }

  /// Write a single byte to memory.
  inline void write(word_t addr, uint8_t data) { RAM[addr] = data; }

  template <Mnemonic N, AdrMode M>
  void execute();
};

  //   while (true) {
  //     uint8_t cur_byte = RAM[PC];
  //     InstDesc idsc = decode_instr(cur_byte);
  //     switch (cur_byte) {
  // #define INST(byte, mon, mode)                \
//   case byte:                                 \
//     execute<Mnemonic::mon, AdrMode::mode>(); \
//     PC += idsc.sz;                           \
//     break;
  // #include "6502/InstructionSet/instrs.def"
  //       default:
  //         assert(false && "unhandled inst");
  //         break;
  //     }
  //   }
template <Mnemonic N, AdrMode M>
inline void Processor::execute() {
  std::cout << "from GENERIC execute of " << N << std::endl;
}

//===----------------------------------------------------------------------===//
// LDA
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::IMM>() {
  AC = read(PC + 1);
}
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::ZPG>() {
  zeffective_address = read(PC + 1);
  AC = read(zeffective_address);
}
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::ZP_X>() {
  zeffective_address = read(PC + 1) + X;
  AC = read(zeffective_address);
}
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::ABS>() {
  effective_address = read_word(PC + 1);
  AC = read(effective_address);
}
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::ABS_X>() {
  effective_address = read_word(PC + 1) + X;
  AC = read(effective_address);
}
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::ABS_Y>() {
  effective_address = read_word(PC + 1) + Y;
  AC = read(effective_address);
}
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::X_IND>() {
  zeffective_address = read(PC + 1) + X;
  zeffective_address = read_word(zeffective_address);
  AC = read(zeffective_address);
}
template <>
inline void Processor::execute<Mnemonic::LDA, AdrMode::IND_Y>() {
  zeffective_address = read(PC + 1);
  zeffective_address = read_word(zeffective_address);
  AC = read(zeffective_address + Y);
}

//===----------------------------------------------------------------------===//
// LDX
template <>
inline void Processor::execute<Mnemonic::LDX, AdrMode::IMM>() {
  X = read(PC + 1);
}
template <>
inline void Processor::execute<Mnemonic::LDX, AdrMode::ABS>() {
  effective_address = read_word(PC + 1);
  X = read(effective_address);
}

//===----------------------------------------------------------------------===//
// LDY
template <>
inline void Processor::execute<Mnemonic::LDY, AdrMode::IMM>() {
  Y = read(PC + 1);
}
template <>
inline void Processor::execute<Mnemonic::LDY, AdrMode::ABS>() {
  effective_address = read_word(PC + 1);
  Y = read(effective_address);
}

//===----------------------------------------------------------------------===//
// STA
template <>
inline void Processor::execute<Mnemonic::STA, AdrMode::ZPG>() {
  zeffective_address = read(PC + 1);
  write(zeffective_address, AC);
}
template <>
inline void Processor::execute<Mnemonic::STA, AdrMode::ZP_X>() {
  zeffective_address = read(PC + 1) + X;
  write(zeffective_address, AC);
}
template <>
inline void Processor::execute<Mnemonic::STA, AdrMode::ABS>() {
  effective_address = read_word(PC + 1);
  write(effective_address, AC);
}
template <>
inline void Processor::execute<Mnemonic::STA, AdrMode::ABS_X>() {
  effective_address = read_word(PC + 1) + X;
  write(effective_address, AC);
}
template <>
inline void Processor::execute<Mnemonic::STA, AdrMode::ABS_Y>() {
  effective_address = read_word(PC + 1) + Y;
  write(effective_address, AC);
}
template <>
inline void Processor::execute<Mnemonic::STA, AdrMode::X_IND>() {
  zeffective_address = read(PC + 1) + X;
  zeffective_address = zread_word(zeffective_address);
  write(zeffective_address, AC);
}
template <>
inline void Processor::execute<Mnemonic::STA, AdrMode::IND_Y>() {
  zeffective_address = read(PC + 1);
  zeffective_address = zread_word(zeffective_address);
  write(zeffective_address + Y, AC);
}

//===----------------------------------------------------------------------===//
// OTHER SECTION
template <>
inline void Processor::execute<Mnemonic::NOP, AdrMode::IMP>() {
  // std::cout << "nop execute" << std::endl;
}

//===----------------------------------------------------------------------===//
// CLC SECTION
template <>
inline void Processor::execute<Mnemonic::CLC, AdrMode::IMP>() {
  SR.C = 0;
}

//===----------------------------------------------------------------------===//
// SBC SECTION
template <>
inline void Processor::execute<Mnemonic::SBC, AdrMode::IMM>() {
  auto operand = read(PC + 1);
  AC -= operand;
  SR.Z = AC == 0;
}

//===----------------------------------------------------------------------===//
// CMP SECTION
template <>
inline void Processor::execute<Mnemonic::CMP, AdrMode::IMM>() {
  auto tmp = read(PC + 1);
  tmp = AC - tmp;
  SR.Z = tmp == 0;
}

//===----------------------------------------------------------------------===//
// RTS SECTION
template <>
inline void Processor::execute<Mnemonic::RTS, AdrMode::IMP>() {
  std::cout << "AC=" << std::hex << (int)AC << std::endl;
  exit(AC);
}

#endif
