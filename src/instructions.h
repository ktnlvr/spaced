#ifndef __SPACED_H__INSTRUCTIONS__
#define __SPACED_H__INSTRUCTIONS__

#include <stdbool.h>

#include "defs.h"

// Refer to https://www.masswerk.at/6502/6502_instruction_set.html#modes
typedef enum chip_addr_mode_t {
  CHIP_ADDR_MODE_UNINIT = 0,
  CHIP_ADDR_MODE_ACCUMULATOR,
  CHIP_ADDR_MODE_IMPLIED,
  CHIP_ADDR_MODE_IMMEDIATE,
  CHIP_ADDR_MODE_ZERO_PAGE,
  CHIP_ADDR_MODE_ZERO_PAGE_X,
  CHIP_ADDR_MODE_ZERO_PAGE_Y,
  CHIP_ADDR_MODE_ABSOLUTE,
  CHIP_ADDR_MODE_ABSOLUTE_X,
  CHIP_ADDR_MODE_ABSOLUTE_Y,
  CHIP_ADDR_MODE_INDIRECT_X,
  CHIP_ADDR_MODE_INDIRECT_Y,
  CHIP_ADDR_MODE_INDIRECT,
  CHIP_ADDR_MODE_RELATIVE,
  CHIP_ADDR_MODE_ILLEGAL,
} chip_addr_mode_t;

typedef enum chip_op_t {
  CHIP_OP_LDA,
  CHIP_OP_STA,
  CHIP_OP_ADC,
} chip_op_t;

static bool instruction_lookups_ready = false;
static chip_addr_mode_t instruction_addr_mode_table[0xFF] = {};
static chip_op_t instruction_opcode_table[0xFF] = {};

static chip_addr_mode_t lookup_addr_mode(byte b) {
  return instruction_addr_mode_table[b];
}

static chip_op_t lookup_instruction(byte b) {
  return instruction_opcode_table[b];
}

static void init_instruction_lookups() {
  if (instruction_lookups_ready)
    return;

  chip_op_t op;

#define _CHIP_OP(i, addr)                                                      \
  instruction_addr_mode_table[0x##i] = CHIP_ADDR_MODE_##addr,                  \
  instruction_opcode_table[0x##i] = op

  op = CHIP_OP_LDA;
  _CHIP_OP(A9, IMMEDIATE);
  _CHIP_OP(A5, ZERO_PAGE);
  _CHIP_OP(B5, ZERO_PAGE_X);
  _CHIP_OP(AD, ABSOLUTE);
  _CHIP_OP(BD, ABSOLUTE_X);
  _CHIP_OP(B9, ABSOLUTE_Y);
  _CHIP_OP(A1, INDIRECT_X);
  _CHIP_OP(B1, INDIRECT_Y);

  op = CHIP_OP_STA;
  _CHIP_OP(85, ZERO_PAGE);
  _CHIP_OP(95, ZERO_PAGE_X);
  _CHIP_OP(8D, ABSOLUTE);
  _CHIP_OP(9D, ABSOLUTE_X);
  _CHIP_OP(99, ABSOLUTE_Y);
  _CHIP_OP(81, INDIRECT_X);
  _CHIP_OP(91, INDIRECT_Y);

  op = CHIP_OP_ADC;
  _CHIP_OP(69, IMMEDIATE);
  _CHIP_OP(65, ZERO_PAGE);
  _CHIP_OP(75, ZERO_PAGE_X);
  _CHIP_OP(6D, ABSOLUTE);
  _CHIP_OP(7D, ABSOLUTE_X);
  _CHIP_OP(79, ABSOLUTE_Y);
  _CHIP_OP(61, INDIRECT_X);
  _CHIP_OP(71, INDIRECT_Y);

  instruction_lookups_ready = true;
}

#endif
