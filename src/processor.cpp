#include "6502/processor.h"

#include <bitset>
#include <cstring>
#include <iostream>
#include <ostream>

#include "6502/InstructionSet/address_space.h"

void Processor::check_for_interrupts() {
  if (reset) {
    memcpy(RAM.data(), reset, ADDR_SPACE_SZ);
    reset_internal_state();
    reset = nullptr;
  }
}

uint8_t Processor::run() {
  // Used in operations that read from memory.
  word_t effective_address = 0;
  // This holds the result of a memory read.
  uint8_t memory = 0;
  // Used for random scratch storage space.
  uint8_t scratch = 0;

  while (true) {
    check_for_interrupts();
    uint8_t cur_byte = RAM[PC];
    InstDesc idsc = decode_desc(cur_byte);
    if (false) {
      std::cout << "PC: 0x" << std::hex << (PC - Regions::BOOTLOADER_ADDR)
                << ", " << idsc << "\n";
    }
    switch ((Opcode)cur_byte) {
#define BREAK_INC_PC \
  PC += idsc.sz;     \
  break;

      // Update the Z & N flags on the status register.
#define UPDATE_NZ(n)   \
  SR.N = n & SIGN_BIT; \
  SR.Z = n == 0;

      // Macros for reading memory for all address modes. They leave memory and
      // effective_address useful.
#define READ_IMM memory = read(PC + 1);
#define READ_ZPG                    \
  effective_address = read(PC + 1); \
  memory = read(effective_address);
#define READ_ZP_X                       \
  effective_address = read(PC + 1) + X; \
  memory = read(effective_address);
#define READ_ZP_Y                       \
  effective_address = read(PC + 1) + Y; \
  memory = read(effective_address);
#define READ_ABS                         \
  effective_address = read_word(PC + 1); \
  memory = read(effective_address);
#define READ_ABS_X                           \
  effective_address = read_word(PC + 1) + X; \
  memory = read(effective_address);
#define READ_ABS_Y                           \
  effective_address = read_word(PC + 1) + Y; \
  memory = read(effective_address);
#define READ_X_IND                                  \
  effective_address = read(PC + 1) + X;             \
  effective_address = read_word(effective_address); \
  memory = read(effective_address)
#define READ_IND_Y                                  \
  effective_address = read(PC + 1);                 \
  effective_address = read_word(effective_address); \
  memory = read(effective_address + Y);

      // --- LDA
      case Opcode::LDA_IMM:
        READ_IMM;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LDA_ZPG:
        READ_ZPG;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LDA_ZP_X:
        READ_ZP_X;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LDA_ABS:
        READ_ABS;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LDA_ABS_X:
        READ_ABS_X;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LDA_ABS_Y:
        READ_ABS_Y;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LDA_X_IND:
        READ_X_IND;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LDA_IND_Y:
        READ_IND_Y;
        AC = memory;
        UPDATE_NZ(AC);
        BREAK_INC_PC;

      // --- LDX
      case Opcode::LDX_IMM:
        READ_IMM;
        X = memory;
        UPDATE_NZ(X);
        BREAK_INC_PC;
      case Opcode::LDX_ZPG:
        READ_ZPG;
        X = memory;
        UPDATE_NZ(X);
        BREAK_INC_PC;
      case Opcode::LDX_ZP_Y:
        READ_ZP_Y;
        X = memory;
        UPDATE_NZ(X);
        BREAK_INC_PC;
      case Opcode::LDX_ABS:
        READ_ABS;
        X = memory;
        UPDATE_NZ(X);
        BREAK_INC_PC;
      case Opcode::LDX_ABS_Y:
        READ_ABS_Y;
        X = memory;
        UPDATE_NZ(X);
        BREAK_INC_PC;

      // --- LDY
      case Opcode::LDY_IMM:
        READ_IMM;
        Y = memory;
        UPDATE_NZ(Y);
        BREAK_INC_PC;
      case Opcode::LDY_ZPG:
        READ_ZPG;
        Y = memory;
        UPDATE_NZ(Y);
        BREAK_INC_PC;
      case Opcode::LDY_ZP_X:
        READ_ZP_X;
        Y = memory;
        UPDATE_NZ(Y);
        BREAK_INC_PC;
      case Opcode::LDY_ABS:
        READ_ABS;
        Y = memory;
        UPDATE_NZ(Y);
        BREAK_INC_PC;
      case Opcode::LDY_ABS_X:
        READ_ABS_X;
        Y = memory;
        UPDATE_NZ(Y);
        BREAK_INC_PC;

#define ST_ZPG(n)                   \
  effective_address = read(PC + 1); \
  write(effective_address, n)
#define ST_ZP_X(n)                      \
  effective_address = read(PC + 1) + X; \
  write(effective_address, n);
#define ST_ZP_Y(n)                      \
  effective_address = read(PC + 1) + Y; \
  write(effective_address, n);
#define ST_ABS(n)                        \
  effective_address = read_word(PC + 1); \
  write(effective_address, n);
#define ST_ABS_X(n)                          \
  effective_address = read_word(PC + 1) + X; \
  write(effective_address, n);
#define ST_ABS_Y(n)                          \
  effective_address = read_word(PC + 1) + Y; \
  write(effective_address, n);
#define ST_X_IND(n)                                  \
  effective_address = read(PC + 1) + X;              \
  effective_address = zread_word(effective_address); \
  write(effective_address, n);
#define ST_IND_Y(n)                                  \
  effective_address = read(PC + 1);                  \
  effective_address = zread_word(effective_address); \
  write(effective_address + Y, n);

      // --- STA
      case Opcode::STA_ZPG:
        ST_ZPG(AC);
        BREAK_INC_PC;
      case Opcode::STA_ZP_X:
        ST_ZP_X(AC);
        BREAK_INC_PC;
      case Opcode::STA_ABS:
        ST_ABS(AC);
        BREAK_INC_PC;
      case Opcode::STA_ABS_X:
        ST_ABS_X(AC);
        BREAK_INC_PC;
      case Opcode::STA_ABS_Y:
        ST_ABS_Y(AC);
        BREAK_INC_PC;
      case Opcode::STA_X_IND:
        ST_X_IND(AC);
        BREAK_INC_PC;
      case Opcode::STA_IND_Y:
        ST_IND_Y(AC);
        BREAK_INC_PC;

      // --- STX
      case Opcode::STX_ZPG:
        ST_ZPG(X);
        BREAK_INC_PC;
      case Opcode::STX_ZP_Y:
        ST_ZP_Y(X);
        BREAK_INC_PC;
      case Opcode::STX_ABS:
        ST_ABS(X);
        BREAK_INC_PC;

      // --- STY
      case Opcode::STY_ZPG:
        ST_ZPG(Y);
        BREAK_INC_PC;
      case Opcode::STY_ZP_X:
        ST_ZP_X(Y);
        BREAK_INC_PC;
      case Opcode::STY_ABS:
        ST_ABS(Y);
        BREAK_INC_PC;

      // --- Inter-register transfers
      case Opcode::TAX_IMP:
        X = AC;
        UPDATE_NZ(X);
        BREAK_INC_PC;
      case Opcode::TAY_IMP:
        Y = AC;
        UPDATE_NZ(Y);
        BREAK_INC_PC;
      case Opcode::TXA_IMP:
        AC = X;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::TXS_IMP:
        SP = X;
        // Don't set the status flags.
        BREAK_INC_PC;
      case Opcode::TYA_IMP:
        AC = Y;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::TSX_IMP:
        X = SP;
        UPDATE_NZ(X);
        BREAK_INC_PC;

      // --- PHA
      case Opcode::PHA_IMP:
        push(AC);
        BREAK_INC_PC;
      // --- PHP
      case Opcode::PHP_IMP: {
        StatusRegister to_push = SR;
        to_push.B = 1;
        to_push._ = 1;
        push(to_push);
        BREAK_INC_PC;
      }

      // --- PLA
      case Opcode::PLA_IMP:
        AC = pop();
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      // --- PLP
      case Opcode::PLP_IMP: {
        StatusRegister old = SR;
        SR = (StatusRegister)pop();
        SR.B = old.B;
        SR._ = old._;
        BREAK_INC_PC;
      }

      // --- DEC
      case Opcode::DEC_ZPG:
        READ_ZPG;
        memory--;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::DEC_ZP_X:
        READ_ZP_X;
        memory--;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::DEC_ABS:
        READ_ABS;
        memory--;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::DEC_ABS_X:
        READ_ABS_X;
        memory--;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      // --- DEX
      case Opcode::DEX_IMP:
        X--;
        BREAK_INC_PC;
      // --- DEY
      case Opcode::DEY_IMP:
        Y--;
        BREAK_INC_PC;

      // --- INC
      case Opcode::INC_ZPG:
        READ_ZPG;
        memory++;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::INC_ZP_X:
        READ_ZP_X;
        memory++;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::INC_ABS:
        READ_ABS;
        memory++;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::INC_ABS_X:
        READ_ABS_X;
        memory++;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      // --- INX
      case Opcode::INX_IMP:
        X++;
        SR.N = X & SIGN_BIT;
        SR.Z = X == 0;
        BREAK_INC_PC;
      // --- INY
      case Opcode::INY_IMP:
        Y++;
        SR.N = Y & SIGN_BIT;
        SR.Z = Y == 0;
        BREAK_INC_PC;

        // --- ADC
#define ADC(inp)                                            \
  int8_t scratch = AC + inp + SR.C;                         \
  SR.C = scratch < AC;                                      \
  SR.Z = scratch == 0;                                      \
  SR.N = scratch & SIGN_BIT;                                \
  SR.V = (!((AC ^ inp) & 0x80) && ((AC ^ scratch) & 0x80)); \
  AC = scratch;
      case Opcode::ADC_IMM: {
        READ_IMM;
        ADC(memory);
        BREAK_INC_PC;
      }
      case Opcode::ADC_ZPG: {
        READ_ZPG;
        ADC(memory);
        BREAK_INC_PC;
      }
      case Opcode::ADC_ZP_X: {
        READ_ZP_X;
        ADC(memory);
        BREAK_INC_PC;
      }
      case Opcode::ADC_ABS: {
        READ_ABS;
        ADC(memory);
        BREAK_INC_PC;
      }
      case Opcode::ADC_ABS_X: {
        READ_ABS_X;
        ADC(memory);
        BREAK_INC_PC;
      }
      case Opcode::ADC_ABS_Y: {
        READ_ABS_Y;
        ADC(memory);
        BREAK_INC_PC;
      }
      case Opcode::ADC_X_IND: {
        READ_X_IND;
        ADC(memory);
        BREAK_INC_PC;
      }
      case Opcode::ADC_IND_Y: {
        READ_IND_Y;
        ADC(memory);
        BREAK_INC_PC;
      }

      // --- SBC
#define SBC(inp)                             \
  int16_t scratch = AC - inp - (!SR.C);      \
  SR.C = scratch >= 0;                       \
  scratch &= 0xFF;                           \
  SR.Z = scratch == 0;                       \
  SR.N = scratch & SIGN_BIT;                 \
  SR.V = (AC ^ scratch) & (AC ^ inp) & 0x80; \
  AC = scratch & 0xFF;
      case Opcode::SBC_IMM: {
        READ_IMM;
        SBC(memory);
        BREAK_INC_PC;
      }
      case Opcode::SBC_ZPG: {
        READ_ZPG;
        SBC(memory);
        BREAK_INC_PC;
      }
      case Opcode::SBC_ZP_X: {
        READ_ZP_X;
        SBC(memory);
        BREAK_INC_PC;
      }
      case Opcode::SBC_ABS: {
        READ_ABS;
        SBC(memory);
        BREAK_INC_PC;
      }
      case Opcode::SBC_ABS_X: {
        READ_ABS_X;
        SBC(memory);
        BREAK_INC_PC;
      }
      case Opcode::SBC_ABS_Y: {
        READ_ABS_Y;
        SBC(memory);
        BREAK_INC_PC;
      }
      case Opcode::SBC_X_IND: {
        READ_X_IND;
        SBC(memory);
        BREAK_INC_PC;
      }
      case Opcode::SBC_IND_Y: {
        READ_IND_Y;
        SBC(memory);
        BREAK_INC_PC;
      }

#define LOGICAL_OP(mon, oper) \
  case Opcode::mon##_IMM:     \
    READ_IMM;                 \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;             \
  case Opcode::mon##_ZPG:     \
    READ_ZPG;                 \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;             \
  case Opcode::mon##_ZP_X:    \
    READ_ZP_X;                \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;             \
  case Opcode::mon##_ABS:     \
    READ_ABS;                 \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;             \
  case Opcode::mon##_ABS_X:   \
    READ_ABS_X;               \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;             \
  case Opcode::mon##_ABS_Y:   \
    READ_ABS_Y;               \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;             \
  case Opcode::mon##_X_IND:   \
    READ_X_IND;               \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;             \
  case Opcode::mon##_IND_Y:   \
    READ_IND_Y;               \
    AC = AC oper memory;      \
    UPDATE_NZ(AC);            \
    BREAK_INC_PC;
        // --- AND
        LOGICAL_OP(AND, &);
        // --- EOR
        LOGICAL_OP(EOR, ^);
        // --- ORA
        LOGICAL_OP(ORA, |);

      // --- ASL
      case Opcode::ASL_A:
        SR.C = AC & SIGN_BIT;
        AC <<= 1;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::ASL_ZPG:
        READ_ZPG;
        SR.C = memory & SIGN_BIT;
        memory <<= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::ASL_ZP_X:
        READ_ZP_X;
        SR.C = memory & SIGN_BIT;
        memory <<= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::ASL_ABS:
        READ_ABS;
        SR.C = memory & SIGN_BIT;
        memory <<= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::ASL_ABS_X:
        READ_ABS_X;
        SR.C = memory & SIGN_BIT;
        memory <<= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;

      // --- LSR
      case Opcode::LSR_A:
        SR.C = AC & 0x1;
        AC >>= 1;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::LSR_ZPG:
        READ_ZPG;
        SR.C = memory & 0x1;
        memory >>= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::LSR_ZP_X:
        READ_ZP_X;
        SR.C = memory & 0x1;
        memory >>= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::LSR_ABS:
        READ_ABS;
        SR.C = memory & 0x1;
        memory >>= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;
      case Opcode::LSR_ABS_X:
        READ_ABS_X;
        SR.C = memory & 0x1;
        memory >>= 1;
        UPDATE_NZ(memory);
        write(effective_address, memory);
        BREAK_INC_PC;

        // --- ROL
#define ROL_MEM                     \
  scratch = memory;                 \
  memory = (memory << 1) | SR.C;    \
  SR.C = scratch & SIGN_BIT;        \
  UPDATE_NZ(memory);                \
  write(effective_address, memory); \
  BREAK_INC_PC;
      case Opcode::ROL_A:
        scratch = AC;
        AC = (AC << 1) | SR.C;
        SR.C = scratch & SIGN_BIT;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::ROL_ZPG:
        READ_ZPG;
        ROL_MEM;
      case Opcode::ROL_ZP_X:
        READ_ZP_X;
        ROL_MEM;
      case Opcode::ROL_ABS:
        READ_ABS;
        ROL_MEM;
      case Opcode::ROL_ABS_X:
        READ_ABS_X;
        ROL_MEM;

        // --- ROR
#define ROR_MEM                         \
  scratch = memory;                     \
  memory = (memory >> 1) | (SR.C << 7); \
  SR.C = scratch & 0x1;                 \
  UPDATE_NZ(memory);                    \
  write(effective_address, memory);     \
  BREAK_INC_PC;
      case Opcode::ROR_A:
        scratch = AC;
        AC = (AC >> 1) | (SR.C << 7);
        SR.C = scratch & 0x1;
        UPDATE_NZ(AC);
        BREAK_INC_PC;
      case Opcode::ROR_ZPG:
        READ_ZPG;
        ROR_MEM;
      case Opcode::ROR_ZP_X:
        READ_ZP_X;
        ROR_MEM;
      case Opcode::ROR_ABS:
        READ_ABS;
        ROR_MEM;
      case Opcode::ROR_ABS_X:
        READ_ABS_X;
        ROL_MEM;

      // --- Clear instructions
      case Opcode::CLC_IMP:
        SR.C = 0;
        BREAK_INC_PC;
      case Opcode::CLD_IMP:
        SR.D = 0;
        BREAK_INC_PC;
      case Opcode::CLI_IMP:
        SR.I = 0;
        BREAK_INC_PC;
      case Opcode::CLV_IMP:
        SR.V = 0;
        BREAK_INC_PC;

      // --- Set instructions
      case Opcode::SEC_IMP:
        SR.C = 1;
        BREAK_INC_PC;
      case Opcode::SED_IMP:
        assert(false && "Decimal mode unsupported.");
        SR.D = 1;
        BREAK_INC_PC;
      case Opcode::SEI_IMP:
        SR.I = 1;
        BREAK_INC_PC;

#define CMP(reg, inp)  \
  scratch = reg - inp; \
  UPDATE_NZ(scratch);  \
  SR.C = reg >= inp;
        // --- CMP
      case Opcode::CMP_IMM:
        READ_IMM;
        CMP(AC, memory);
        BREAK_INC_PC;
      case Opcode::CMP_ZPG:
        READ_ZPG;
        CMP(AC, memory);
        BREAK_INC_PC;
      case Opcode::CMP_ZP_X:
        READ_ZP_X;
        CMP(AC, memory);
        BREAK_INC_PC;
      case Opcode::CMP_ABS:
        READ_ABS;
        CMP(AC, memory);
        BREAK_INC_PC;
      case Opcode::CMP_ABS_X:
        READ_ABS_X;
        CMP(AC, memory);
        BREAK_INC_PC;
      case Opcode::CMP_ABS_Y:
        READ_ABS_Y;
        CMP(AC, memory);
        BREAK_INC_PC;
      case Opcode::CMP_X_IND:
        READ_X_IND;
        CMP(AC, memory);
        BREAK_INC_PC;
      case Opcode::CMP_IND_Y:
        READ_IND_Y;
        CMP(AC, memory);
        BREAK_INC_PC;

      // --- CPX
      case Opcode::CPX_IMM:
        READ_IMM;
        CMP(X, memory);
        BREAK_INC_PC;
      case Opcode::CPX_ZPG:
        READ_ZPG;
        CMP(X, memory);
        BREAK_INC_PC;
      case Opcode::CPX_ABS:
        READ_ABS;
        CMP(X, memory);
        BREAK_INC_PC;

      // --- CPY
      case Opcode::CPY_IMM:
        READ_IMM;
        CMP(Y, memory);
        BREAK_INC_PC;
      case Opcode::CPY_ZPG:
        READ_ZPG;
        CMP(Y, memory);
        BREAK_INC_PC;
      case Opcode::CPY_ABS:
        READ_ABS;
        CMP(Y, memory);
        BREAK_INC_PC;

#define BRANCH_IF(REG, test)    \
  if (SR.REG == test) {         \
    PC += (int8_t)read(PC + 1); \
    PC += 2;                    \
    break;                      \
  }                             \
  BREAK_INC_PC;
      // --- BEQ
      case Opcode::BEQ_REL:
        BRANCH_IF(Z, 1);
      // --- BCS
      case Opcode::BCS_REL:
        BRANCH_IF(C, 1);
      // --- BMI
      case Opcode::BMI_REL:
        BRANCH_IF(N, 1);
      // --- BVS
      case Opcode::BVS_REL:
        BRANCH_IF(V, 1);

      // --- BNE
      case Opcode::BNE_REL:
        BRANCH_IF(Z, 0);
      // --- BCC
      case Opcode::BCC_REL:
        BRANCH_IF(C, 0);
      // --- BPL
      case Opcode::BPL_REL:
        BRANCH_IF(N, 0);
      // --- BVC
      case Opcode::BVC_REL:
        BRANCH_IF(V, 0);

      // --- JMP
      case Opcode::JMP_ABS:
        PC = read_word(PC + 1);
        break;
      case Opcode::JMP_IND:
        effective_address = read_word(PC + 1);
        PC = read_word(effective_address);
        break;

      // --- JSR
      case Opcode::JSR_ABS: {
        // The JSR instruction is 3 bytes, and we need to store the location
        // right *before* where we wish to resume.
        word_t target_PC = PC + 2;
        push(target_PC >> 8);
        push(target_PC & 0x00FF);
        PC = read_word(PC + 1);
        break;
      }

      // --- RTS
      case Opcode::RTS_IMP: {
        if (SP == 0xFF) return AC;
        PC = pop();
        PC |= static_cast<word_t>(pop()) << 8;
        // Make sure to add 1 to what we stored in the stack.
        ++PC;
        break;
      }

      // --- RTI
      case Opcode::RTI_IMP: {
        StatusRegister pulled =
            (StatusRegister)read(Regions::STACK.begin | SP + 1);
        pulled.B = 0;
        pulled._ = 0;
        SR = (StatusRegister)(pulled | SR);
        ++SP;
        PC = read_word(Regions::STACK.begin | SP + 1) + 1;
        SP += 2;
        BREAK_INC_PC;
      }

      // --- BTT
      case Opcode::BIT_ZPG:
        READ_ZPG;
        SR.Z = (AC & memory) == 0;
        SR.V = memory & (1 << 7);
        SR.N = memory & (1 << 8);
        BREAK_INC_PC;
      case Opcode::BIT_ABS:
        READ_ABS;
        SR.Z = (AC & memory) == 0;
        SR.V = memory & (1 << 7);
        SR.N = memory & (1 << 8);
        BREAK_INC_PC;

      // --- NOP
      case Opcode::NOP_IMP: {
        std::cout << "NOP:AC=" << std::dec << +(uint8_t)AC << "/" << +(int8_t)AC
                  << "\n";
        uint8_t &sr = *reinterpret_cast<uint8_t *>(&SR);
        std::cout << "    SR=" << std::bitset<8>(sr) << std::endl;
        std::cout << "       " << "NV_BDIZC" << std::endl;
        BREAK_INC_PC;
      }

      default:
        std::cerr << "PC: 0x" << std::hex << PC << ", unhandled " << idsc
                  << std::endl;
        assert(false && "unimplemnted op");
    }
  }
  return 0;
}
