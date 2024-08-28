#ifndef SIXFIVE_INSTRS_H
#define SIXFIVE_INSTRS_H

#include <cassert>
#include <cstdint>
#include <ostream>

/// Word size is 16 bits.
typedef uint16_t word_t;

enum class Mnemonic : uint8_t {
#define MON(name) name,
#include "6502/InstructionSet/mnemonics.def"
#undef MON
};

enum class AdrMode : uint8_t {
#define MODE(name) name,
#include "6502/InstructionSet/address_modes.def"
#undef MODE
};

enum class Opcode : uint8_t {
#define INST(byte, mon, mode) mon##_##mode = byte,
#include "6502/InstructionSet/instrs.def"
#undef MODE
};

static std::ostream& operator<<(std::ostream& stream,
                                const Mnemonic& mnemonic) {
  switch (mnemonic) {
#define MON(name)      \
  case Mnemonic::name: \
    return stream << #name;
#include "6502/InstructionSet/mnemonics.def"
#undef MODE
  };
}

static std::ostream& operator<<(std::ostream& stream, const AdrMode& mode) {
  switch (mode) {
#define MODE(name)    \
  case AdrMode::name: \
    return stream << #name;
#include "6502/InstructionSet/address_modes.def"
#undef MODE
  };
}

/// Instruction descriptor.
struct InstDesc {
  Mnemonic mon;
  AdrMode mode;
  uint8_t sz;
};

static std::ostream& operator<<(std::ostream& stream, const InstDesc& desc) {
  return stream << desc.mon << " " << desc.mode << " " << (int)desc.sz << "b";
}

constexpr InstDesc byte_to_inst(uint8_t inst_byte) {
  Mnemonic op = Mnemonic::INVALID;
  AdrMode mode = AdrMode::INVALID;
  switch (inst_byte) {
#define INST(byte, iop, imode) \
  case byte:                   \
    op = Mnemonic::iop;        \
    mode = AdrMode::imode;     \
    break;
#include "6502/InstructionSet/instrs.def"
    default:
      return {Mnemonic::INVALID, AdrMode::IMP, 0};
  }
  uint8_t sz = 0;
  switch (mode) {
    case AdrMode::IMP:
    case AdrMode::A:
      sz = 1;
      break;
    case AdrMode::REL:
    case AdrMode::IMM:
    case AdrMode::ZPG:
    case AdrMode::ZP_X:
    case AdrMode::ZP_Y:
    case AdrMode::X_IND:
    case AdrMode::IND_Y:
      sz = 2;
      break;
    case AdrMode::ABS:
    case AdrMode::ABS_X:
    case AdrMode::ABS_Y:
    case AdrMode::IND:
      sz = 3;
      break;
    case AdrMode::INVALID:
      assert(false && "unhandled address mode");
  }
  return {op, mode, sz};
}

static InstDesc INST_TABLE[256] = {
    byte_to_inst(0),   byte_to_inst(1),   byte_to_inst(2),   byte_to_inst(3),
    byte_to_inst(4),   byte_to_inst(5),   byte_to_inst(6),   byte_to_inst(7),
    byte_to_inst(8),   byte_to_inst(9),   byte_to_inst(10),  byte_to_inst(11),
    byte_to_inst(12),  byte_to_inst(13),  byte_to_inst(14),  byte_to_inst(15),
    byte_to_inst(16),  byte_to_inst(17),  byte_to_inst(18),  byte_to_inst(19),
    byte_to_inst(20),  byte_to_inst(21),  byte_to_inst(22),  byte_to_inst(23),
    byte_to_inst(24),  byte_to_inst(25),  byte_to_inst(26),  byte_to_inst(27),
    byte_to_inst(28),  byte_to_inst(29),  byte_to_inst(30),  byte_to_inst(31),
    byte_to_inst(32),  byte_to_inst(33),  byte_to_inst(34),  byte_to_inst(35),
    byte_to_inst(36),  byte_to_inst(37),  byte_to_inst(38),  byte_to_inst(39),
    byte_to_inst(40),  byte_to_inst(41),  byte_to_inst(42),  byte_to_inst(43),
    byte_to_inst(44),  byte_to_inst(45),  byte_to_inst(46),  byte_to_inst(47),
    byte_to_inst(48),  byte_to_inst(49),  byte_to_inst(50),  byte_to_inst(51),
    byte_to_inst(52),  byte_to_inst(53),  byte_to_inst(54),  byte_to_inst(55),
    byte_to_inst(56),  byte_to_inst(57),  byte_to_inst(58),  byte_to_inst(59),
    byte_to_inst(60),  byte_to_inst(61),  byte_to_inst(62),  byte_to_inst(63),
    byte_to_inst(64),  byte_to_inst(65),  byte_to_inst(66),  byte_to_inst(67),
    byte_to_inst(68),  byte_to_inst(69),  byte_to_inst(70),  byte_to_inst(71),
    byte_to_inst(72),  byte_to_inst(73),  byte_to_inst(74),  byte_to_inst(75),
    byte_to_inst(76),  byte_to_inst(77),  byte_to_inst(78),  byte_to_inst(79),
    byte_to_inst(80),  byte_to_inst(81),  byte_to_inst(82),  byte_to_inst(83),
    byte_to_inst(84),  byte_to_inst(85),  byte_to_inst(86),  byte_to_inst(87),
    byte_to_inst(88),  byte_to_inst(89),  byte_to_inst(90),  byte_to_inst(91),
    byte_to_inst(92),  byte_to_inst(93),  byte_to_inst(94),  byte_to_inst(95),
    byte_to_inst(96),  byte_to_inst(97),  byte_to_inst(98),  byte_to_inst(99),
    byte_to_inst(100), byte_to_inst(101), byte_to_inst(102), byte_to_inst(103),
    byte_to_inst(104), byte_to_inst(105), byte_to_inst(106), byte_to_inst(107),
    byte_to_inst(108), byte_to_inst(109), byte_to_inst(110), byte_to_inst(111),
    byte_to_inst(112), byte_to_inst(113), byte_to_inst(114), byte_to_inst(115),
    byte_to_inst(116), byte_to_inst(117), byte_to_inst(118), byte_to_inst(119),
    byte_to_inst(120), byte_to_inst(121), byte_to_inst(122), byte_to_inst(123),
    byte_to_inst(124), byte_to_inst(125), byte_to_inst(126), byte_to_inst(127),
    byte_to_inst(128), byte_to_inst(129), byte_to_inst(130), byte_to_inst(131),
    byte_to_inst(132), byte_to_inst(133), byte_to_inst(134), byte_to_inst(135),
    byte_to_inst(136), byte_to_inst(137), byte_to_inst(138), byte_to_inst(139),
    byte_to_inst(140), byte_to_inst(141), byte_to_inst(142), byte_to_inst(143),
    byte_to_inst(144), byte_to_inst(145), byte_to_inst(146), byte_to_inst(147),
    byte_to_inst(148), byte_to_inst(149), byte_to_inst(150), byte_to_inst(151),
    byte_to_inst(152), byte_to_inst(153), byte_to_inst(154), byte_to_inst(155),
    byte_to_inst(156), byte_to_inst(157), byte_to_inst(158), byte_to_inst(159),
    byte_to_inst(160), byte_to_inst(161), byte_to_inst(162), byte_to_inst(163),
    byte_to_inst(164), byte_to_inst(165), byte_to_inst(166), byte_to_inst(167),
    byte_to_inst(168), byte_to_inst(169), byte_to_inst(170), byte_to_inst(171),
    byte_to_inst(172), byte_to_inst(173), byte_to_inst(174), byte_to_inst(175),
    byte_to_inst(176), byte_to_inst(177), byte_to_inst(178), byte_to_inst(179),
    byte_to_inst(180), byte_to_inst(181), byte_to_inst(182), byte_to_inst(183),
    byte_to_inst(184), byte_to_inst(185), byte_to_inst(186), byte_to_inst(187),
    byte_to_inst(188), byte_to_inst(189), byte_to_inst(190), byte_to_inst(191),
    byte_to_inst(192), byte_to_inst(193), byte_to_inst(194), byte_to_inst(195),
    byte_to_inst(196), byte_to_inst(197), byte_to_inst(198), byte_to_inst(199),
    byte_to_inst(200), byte_to_inst(201), byte_to_inst(202), byte_to_inst(203),
    byte_to_inst(204), byte_to_inst(205), byte_to_inst(206), byte_to_inst(207),
    byte_to_inst(208), byte_to_inst(209), byte_to_inst(210), byte_to_inst(211),
    byte_to_inst(212), byte_to_inst(213), byte_to_inst(214), byte_to_inst(215),
    byte_to_inst(216), byte_to_inst(217), byte_to_inst(218), byte_to_inst(219),
    byte_to_inst(220), byte_to_inst(221), byte_to_inst(222), byte_to_inst(223),
    byte_to_inst(224), byte_to_inst(225), byte_to_inst(226), byte_to_inst(227),
    byte_to_inst(228), byte_to_inst(229), byte_to_inst(230), byte_to_inst(231),
    byte_to_inst(232), byte_to_inst(233), byte_to_inst(234), byte_to_inst(235),
    byte_to_inst(236), byte_to_inst(237), byte_to_inst(238), byte_to_inst(239),
    byte_to_inst(240), byte_to_inst(241), byte_to_inst(242), byte_to_inst(243),
    byte_to_inst(244), byte_to_inst(245), byte_to_inst(246), byte_to_inst(247),
    byte_to_inst(248), byte_to_inst(249), byte_to_inst(250), byte_to_inst(251),
    byte_to_inst(252), byte_to_inst(253), byte_to_inst(254), byte_to_inst(255),
};

inline InstDesc decode_desc(uint8_t inst_byte) { return INST_TABLE[inst_byte]; }

#endif
