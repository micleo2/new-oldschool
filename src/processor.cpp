#include "6502/processor.h"

#include <cstring>
#include <iostream>
#include <ostream>

#include "6502/InstructionSet/address_space.h"

void Processor::check_for_interrupts() {
  if (reset) {
    memcpy(RAM.data(), reset, ADDR_SPACE_SZ);
    init_registers();
    reset = nullptr;
  }
}

uint8_t Processor::run() {
  // std::cout << "dumping code\n";
  // word_t idx = BOOTLOADER_ADDR;
  // while (true) {
  //   uint8_t cur_byte = RAM[idx];
  //   if (!cur_byte) break;
  //   InstDesc idsc = decode_desc(cur_byte);
  //   std::cout << idsc << "\n";
  //   idx += idsc.sz;
  // }

  // Reused in many memory operations.
  word_t effective_address = 0;
  // Same thing, but for zero page address modes.
  uint8_t zeffective_address = 0;

  // Used for random scratch storage space.
  uint8_t scratch = 0;
  word_t scratch_word = 0;

  while (true) {
    check_for_interrupts();
    uint8_t cur_byte = RAM[PC];
    InstDesc idsc = decode_desc(cur_byte);
    if (false) {
      std::cout << "PC: 0x" << std::hex << PC << ", " << idsc << "\n";
    }
    switch ((Opcode)cur_byte) {
#define BREAK_INC_PC \
  PC += idsc.sz;     \
  break;

      // --- LDA
      case Opcode::LDA_IMM:
        AC = read(PC + 1);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDA_ZPG:
        zeffective_address = read(PC + 1);
        AC = read(zeffective_address);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDA_ZP_X:
        zeffective_address = read(PC + 1) + X;
        AC = read(zeffective_address);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDA_ABS:
        effective_address = read_word(PC + 1);
        AC = read(effective_address);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDA_ABS_X:
        effective_address = read_word(PC + 1) + X;
        AC = read(effective_address);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDA_ABS_Y:
        effective_address = read_word(PC + 1) + Y;
        AC = read(effective_address);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDA_X_IND:
        zeffective_address = read(PC + 1) + X;
        effective_address = read_word(zeffective_address);
        AC = read(effective_address);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDA_IND_Y:
        zeffective_address = read(PC + 1);
        effective_address = read_word(zeffective_address);
        AC = read(effective_address + Y);
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;

      // --- LDX
      case Opcode::LDX_IMM:
        X = read(PC + 1);
        SR.Z = X == 0;
        SR.N = X & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDX_ZPG:
        zeffective_address = read(PC + 1);
        X = read(zeffective_address);
        SR.Z = X == 0;
        SR.N = X & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDX_ZP_Y:
        zeffective_address = read(PC + 1) + Y;
        X = read(zeffective_address);
        SR.Z = X == 0;
        SR.N = X & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDX_ABS:
        effective_address = read_word(PC + 1);
        X = read(effective_address);
        SR.Z = X == 0;
        SR.N = X & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDX_ABS_Y:
        effective_address = read_word(PC + 1) + Y;
        X = read(effective_address);
        SR.Z = X == 0;
        SR.N = X & SIGN_BIT;
        BREAK_INC_PC;

      // --- LDY
      case Opcode::LDY_IMM:
        Y = read(PC + 1);
        SR.Z = Y == 0;
        SR.N = Y & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDY_ZPG:
        zeffective_address = read(PC + 1);
        Y = read(zeffective_address);
        SR.Z = Y == 0;
        SR.N = Y & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDY_ZP_X:
        zeffective_address = read(PC + 1) + Y;
        Y = read(zeffective_address);
        SR.Z = Y == 0;
        SR.N = Y & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDY_ABS:
        effective_address = read_word(PC + 1);
        Y = read(effective_address);
        SR.Z = Y == 0;
        SR.N = Y & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::LDY_ABS_X:
        effective_address = read_word(PC + 1) + Y;
        Y = read(effective_address);
        SR.Z = Y == 0;
        SR.N = Y & SIGN_BIT;
        BREAK_INC_PC;

      // --- STA
      case Opcode::STA_ZPG:
        zeffective_address = read(PC + 1);
        write(zeffective_address, AC);
        BREAK_INC_PC;
      case Opcode::STA_ZP_X:
        zeffective_address = read(PC + 1) + X;
        write(zeffective_address, AC);
        BREAK_INC_PC;
      case Opcode::STA_ABS:
        effective_address = read_word(PC + 1);
        write(effective_address, AC);
        BREAK_INC_PC;
      case Opcode::STA_ABS_X:
        effective_address = read_word(PC + 1) + X;
        write(effective_address, AC);
        BREAK_INC_PC;
      case Opcode::STA_ABS_Y:
        effective_address = read_word(PC + 1) + Y;
        write(effective_address, AC);
        BREAK_INC_PC;
      case Opcode::STA_X_IND:
        zeffective_address = read(PC + 1) + X;
        effective_address = zread_word(zeffective_address);
        write(effective_address, AC);
        BREAK_INC_PC;
      case Opcode::STA_IND_Y:
        zeffective_address = read(PC + 1);
        effective_address = zread_word(zeffective_address);
        write(effective_address + Y, AC);
        BREAK_INC_PC;

      // --- STX
      case Opcode::STX_ZPG:
        zeffective_address = read(PC + 1);
        write(zeffective_address, X);
        BREAK_INC_PC;
      case Opcode::STX_ZP_Y:
        zeffective_address = read(PC + 1) + Y;
        write(zeffective_address, X);
        BREAK_INC_PC;
      case Opcode::STX_ABS:
        effective_address = read_word(PC + 1);
        write(effective_address, X);
        BREAK_INC_PC;

      // --- STY
      case Opcode::STY_ZPG:
        zeffective_address = read(PC + 1);
        write(zeffective_address, Y);
        BREAK_INC_PC;
      case Opcode::STY_ZP_X:
        zeffective_address = read(PC + 1) + X;
        write(zeffective_address, Y);
        BREAK_INC_PC;
      case Opcode::STY_ABS:
        effective_address = read_word(PC + 1);
        write(effective_address, Y);
        BREAK_INC_PC;

      // --- CLC
      case Opcode::CLC_IMP:
        SR.C = 0;
        BREAK_INC_PC;

      // --- SEC
      case Opcode::SEC_IMP:
        SR.C = 1;
        BREAK_INC_PC;

      // --- ADC
      case Opcode::ADC_IMM: {
        // result
        uint8_t operand = read(PC + 1);
        scratch_word = AC + operand + SR.C;
        SR.C = scratch_word > 0xFF;
        scratch_word &= 0xFF;
        SR.Z = scratch == 0;
        SR.N = scratch & SIGN_BIT;
        SR.V = ((AC & SIGN_BIT) == (operand & SIGN_BIT)) &
               ((AC & SIGN_BIT) != (scratch & SIGN_BIT));
        AC = scratch_word;
        BREAK_INC_PC;
      }
      case Opcode::ADC_ZPG: {
        // result
        zeffective_address = read(PC + 1);
        uint8_t operand = read(zeffective_address);
        scratch_word = AC + operand + SR.C;
        SR.C = scratch_word > 0xFF;
        scratch_word &= 0xFF;
        SR.Z = scratch == 0;
        SR.N = scratch & SIGN_BIT;
        SR.V = ((AC & SIGN_BIT) == (operand & SIGN_BIT)) &
               ((AC & SIGN_BIT) != (scratch & SIGN_BIT));
        AC = scratch_word;
        BREAK_INC_PC;
      }

      // --- SBC
      case Opcode::SBC_IMM: {
        // result
        uint8_t operand = read(PC + 1);
        int16_t scratch_word = AC - operand - (!SR.C);
        SR.C = scratch_word < 0;
        scratch_word &= 0xFF;
        SR.Z = scratch_word == 0;
        SR.N = scratch_word & SIGN_BIT;
        SR.V = ((AC & SIGN_BIT) == (operand & SIGN_BIT)) &
               ((AC & SIGN_BIT) != (scratch_word & SIGN_BIT));
        AC = scratch_word;
        BREAK_INC_PC;
      }
      case Opcode::SBC_ZPG: {
        // result
        zeffective_address = read(PC + 1);
        uint8_t operand = read(zeffective_address);
        int16_t scratch_word = AC - operand - (!SR.C);
        SR.C = scratch_word < 0;
        scratch_word &= 0xFF;
        SR.Z = scratch_word == 0;
        SR.N = scratch_word & SIGN_BIT;
        SR.V = ((AC & SIGN_BIT) == (operand & SIGN_BIT)) &
               ((AC & SIGN_BIT) != (scratch_word & SIGN_BIT));
        AC = scratch_word;
        BREAK_INC_PC;
      }

      // --- Increments
      case Opcode::INX_IMP:
        X++;
        SR.N = X & SIGN_BIT;
        SR.Z = X == 0;
        BREAK_INC_PC;
      case Opcode::INY_IMP:
        Y++;
        SR.N = Y & SIGN_BIT;
        SR.Z = Y == 0;
        BREAK_INC_PC;

      // --- ORA
      case Opcode::ORA_IMM:
        AC |= read(PC + 1);
        BREAK_INC_PC;
      case Opcode::ORA_ZPG:
        zeffective_address = read(PC + 1);
        AC |= read(zeffective_address);
        BREAK_INC_PC;
      case Opcode::ORA_ZP_X:
        zeffective_address = read(PC + 1) + X;
        AC |= read(zeffective_address);
        BREAK_INC_PC;
      case Opcode::ORA_ABS:
        effective_address = read_word(PC + 1);
        AC |= read(effective_address);
        BREAK_INC_PC;
      case Opcode::ORA_ABS_X:
        effective_address = read_word(PC + 1) + X;
        AC |= read(effective_address);
        BREAK_INC_PC;
      case Opcode::ORA_ABS_Y:
        effective_address = read_word(PC + 1) + Y;
        AC |= read(effective_address);
        BREAK_INC_PC;
      case Opcode::ORA_X_IND:
        zeffective_address = read(PC + 1) + X;
        effective_address = read_word(zeffective_address);
        AC |= read(effective_address);
        BREAK_INC_PC;
      case Opcode::ORA_IND_Y:
        zeffective_address = read(PC + 1);
        effective_address = read_word(zeffective_address);
        AC |= read(effective_address + Y);
        BREAK_INC_PC;

      // --- AND
      case Opcode::AND_IMM:
        AC &= read(PC + 1);
        BREAK_INC_PC;
      case Opcode::AND_ZPG:
        zeffective_address = read(PC + 1);
        AC &= read(zeffective_address);
        BREAK_INC_PC;
      case Opcode::AND_ZP_X:
        zeffective_address = read(PC + 1) + X;
        AC &= read(zeffective_address);
        BREAK_INC_PC;
      case Opcode::AND_ABS:
        effective_address = read_word(PC + 1);
        AC &= read(effective_address);
        BREAK_INC_PC;
      case Opcode::AND_ABS_X:
        effective_address = read_word(PC + 1) + X;
        AC &= read(effective_address);
        BREAK_INC_PC;
      case Opcode::AND_ABS_Y:
        effective_address = read_word(PC + 1) + Y;
        AC &= read(effective_address);
        BREAK_INC_PC;
      case Opcode::AND_X_IND:
        zeffective_address = read(PC + 1) + X;
        effective_address = read_word(zeffective_address);
        AC &= read(effective_address);
        BREAK_INC_PC;
      case Opcode::AND_IND_Y:
        zeffective_address = read(PC + 1);
        effective_address = read_word(zeffective_address);
        AC &= read(effective_address + Y);
        BREAK_INC_PC;

      // --- LSR
      case Opcode::LSR_A:
        SR.C = AC & 0x1;
        AC >>= 1;
        SR.Z = AC == 0;
        SR.N = 0;
        BREAK_INC_PC;

      // --- ASL
      case Opcode::ASL_A:
        SR.C = AC & SIGN_BIT;
        AC <<= 1;
        SR.Z = AC == 0;
        SR.N = 0;
        BREAK_INC_PC;

      // --- ASL
      case Opcode::ASL_ZPG:
        zeffective_address = read(PC + 1);
        scratch = read(zeffective_address);
        SR.C = scratch & SIGN_BIT;
        scratch <<= 1;
        SR.Z = AC == 0;
        SR.N = 0;
        write(zeffective_address, scratch);
        BREAK_INC_PC;

      // --- CMP
      case Opcode::CMP_IMM: {
        int16_t signed_scratch = static_cast<word_t>(AC) - read(PC + 1);
        SR.N = signed_scratch & SIGN_BIT;
        SR.Z = (signed_scratch & 0xFF) == 0;
        SR.C = signed_scratch < 0;
        BREAK_INC_PC;
      }
      case Opcode::CMP_ZPG: {
        zeffective_address = read(PC + 1);
        int16_t signed_scratch =
            static_cast<word_t>(AC) - read(zeffective_address);
        SR.N = signed_scratch & SIGN_BIT;
        SR.Z = (signed_scratch & 0xFF) == 0;
        SR.C = signed_scratch < 0;
        BREAK_INC_PC;
      }

      // --- CPX
      case Opcode::CPX_IMM: {
        int16_t signed_scratch = static_cast<word_t>(X) - read(PC + 1);
        SR.N = signed_scratch & SIGN_BIT;
        SR.Z = (signed_scratch & 0xFF) == 0;
        SR.C = signed_scratch < 0;
        BREAK_INC_PC;
      }

      // --- CPY
      case Opcode::CPY_IMM: {
        int16_t signed_scratch = static_cast<word_t>(Y) - read(PC + 1);
        SR.N = signed_scratch & SIGN_BIT;
        SR.Z = (signed_scratch & 0xFF) == 0;
        SR.C = signed_scratch < 0;
        BREAK_INC_PC;
      }

      // --- BEQ
      case Opcode::BEQ_REL:
        if (SR.Z) {
          PC += (int8_t)read(PC + 1);
          PC += 2;
          break;
        }
        BREAK_INC_PC;

      // --- BNE
      case Opcode::BNE_REL:
        if (SR.Z == 0) {
          PC += (int8_t)read(PC + 1);
          PC += 2;
          break;
        }
        BREAK_INC_PC;

      // --- BCC
      case Opcode::BCC_REL:
        if (SR.C == 0) {
          PC += (int8_t)read(PC + 1);
          PC += 2;
          break;
        }
        BREAK_INC_PC;

      // --- JSR
      case Opcode::JSR_ABS:
        // The JSR instruction is 3 bytes, and we need to store the location
        // right *before* where we wish to resume.
        scratch_word = Regions::STACK.begin | SP;
        write(scratch_word, (PC + 2) >> 8);
        write(scratch_word - 1, (PC + 2) & 0x00FF);
        PC = read_word(PC + 1);
        SP -= 2;
        break;

      // --- RTS
      case Opcode::RTS_IMP:
        if (SP == 0xFF) return AC;
        // Make sure to add 1 to what we stored in the stack.
        PC = read_word(Regions::STACK.begin | SP + 1) + 1;
        SP += 2;
        break;

      // --- JMP
      case Opcode::JMP_ABS:
        PC = read_word(PC + 1);
        break;

      // --- PHA
      case Opcode::PHA_IMP:
        write(Regions::STACK.begin | SP, AC);
        --SP;
        BREAK_INC_PC;

      // --- PLA
      case Opcode::PLA_IMP:
        AC = read(Regions::STACK.begin | SP + 1);
        ++SP;
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;

      // --- Register transfers
      case Opcode::TAX_IMP:
        X = AC;
        SR.Z = X == 0;
        SR.N = X & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::TAY_IMP:
        Y = AC;
        SR.Z = Y == 0;
        SR.N = Y & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::TXA_IMP:
        AC = X;
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::TXS_IMP:
        SP = X;
        // Don't set the status flags.
        BREAK_INC_PC;
      case Opcode::TYA_IMP:
        AC = Y;
        SR.Z = AC == 0;
        SR.N = AC & SIGN_BIT;
        BREAK_INC_PC;
      case Opcode::TSX_IMP:
        X = SP;
        SR.Z = X == 0;
        SR.N = X & SIGN_BIT;
        BREAK_INC_PC;

      // --- NOP
      case Opcode::NOP_IMP:
        std::cout << "NOP dumping AC=" << std::dec << (int)AC << "\n";
        BREAK_INC_PC;

      default:
        std::cerr << "PC: 0x" << std::hex << PC << ", unhandled " << idsc
                  << std::endl;
        assert(false && "unimplemnted op");
    }
  }
  return 0;
}
