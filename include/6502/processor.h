#ifndef SIXFIVE_MICROPROCESSOR_H
#define SIXFIVE_MICROPROCESSOR_H

#include <cstdint>
#include <cstring>
#include <vector>

#include "6502/InstructionSet/address_space.h"
#include "6502/InstructionSet/instrs.h"

class Processor {
  std::vector<uint8_t> &RAM;

  /// Program counter
  word_t PC;
  /// Accumulator
  uint8_t AC;
  /// Index register X
  uint8_t X;
  /// Index register Y
  uint8_t Y;
  /// Status register
  struct StatusRegister {
    bool C : 1;  // Carry
    bool Z : 1;  // Zero
    bool I : 1;  // Interrupt (IRQ disable)
    bool D : 1;  // Decimal (use BCD for arithmetics)
    bool B : 1;  // Break
    bool _ : 1;  // Unused
    bool V : 1;  // Overflow
    bool N : 1;  // Negative
    operator uint8_t() const {
      return *reinterpret_cast<const uint8_t *>(this);
    }
  } SR;
  static_assert(sizeof(StatusRegister) == 1);
  /// This constant can be used to quickly check if a signed byte is negative.
  /// Given that signed integers use two complement, we just need to check that
  /// the top bit is set.
  const static uint8_t SIGN_BIT = 0b10000000;
  /// Stack pointer. This stores the low-byte of an address into the processor
  /// stack. The processor stack is page 1 of the address space, so the high
  /// byte is always assumed to be 0x01.
  uint8_t SP;

  /// The new memory which will be loaded in when this interrupt is serviced.
  uint8_t *reset = nullptr;

 public:
  Processor(std::vector<uint8_t> &mem) : RAM(mem) { reset_internal_state(); };

  /// Run code until completion. \return the final value of the accumulator
  /// register. Interruptible.
  uint8_t run();

  /// Set by another thread when we should reset. This contains the pointer
  /// where we can find the new memory to copy into RAM.
  void set_reset(uint8_t *new_mem) { reset = new_mem; }

  const std::vector<uint8_t> &memory() const { return RAM; }
  std::vector<uint8_t> &memory() { return RAM; }

 private:
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

  /// Push \p val to the stack. Decrements \c SP.
  inline void push(uint8_t val) {
    write(Regions::STACK.begin | SP, val);
    --SP;
  }
  /// Pop and \return the top value from the stack. Increments \c SP.
  inline uint8_t pop() {
    auto ret = read(Regions::STACK.begin | SP + 1);
    ++SP;
    return ret;
  }

  void check_for_interrupts();

  void reset_internal_state() {
    PC = Regions::BOOTLOADER_ADDR;
    AC = 0;
    X = 0;
    Y = 0;
    // Starts high and grows towards 0.
    SP = 0xFF;
    memset(&SR, 0, sizeof(StatusRegister));
  }
};

#endif
