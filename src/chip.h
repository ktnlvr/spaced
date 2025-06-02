#ifndef __SPACED_H__CHIP__
#define __SPACED_H__CHIP__

#include <string.h>

#include "defs.h"

typedef enum chip_quota_policy_t {
  // Consume 1 per instruction
  CHIP_QUOTA_POLICY_FLAT,
} chip_quota_policy_t;

#define CHIP_OPCODE(name, hex) CHIP_OP_##name = 0x##hex

// Refer to https://www.masswerk.at/6502/6502_instruction_set.html#modes
typedef enum chip_addr_mode_t {
  CHIP_ADDR_MODE_UNINIT,
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

static bool addr_modes_lookup_table_ready = false;
static chip_addr_mode_t addr_modes_lookup_table[0xFF] = {};

typedef struct chip_t {
  // How the prices for the instructions are calculated
  chip_quota_policy_t quota_policy;
  // How many instructions it can afford to execute
  u32 quota;
  // The actual memory to be modified
  byte *memory;

  byte a, x, y, sp, pc, p;
} chip_t;

inline void chip_init(chip_t *self, chip_quota_policy_t policy, u32 quota,
                      byte *memory) {
  memset((void *)self, 0, sizeof(chip_t));

  self->quota_policy = policy;
  self->quota = quota;
  self->memory = memory;
}

inline byte chip_read(chip_t *self, chip_addr_mode_t mode) {
  uint16_t addr;

  switch (mode) {
  case CHIP_ADDR_MODE_IMPLIED:
    return 0;
  case CHIP_ADDR_MODE_ACCUMULATOR:
    return self->a;
  case CHIP_ADDR_MODE_RELATIVE:
  case CHIP_ADDR_MODE_IMMEDIATE:
    return self->memory[self->pc++];
  case CHIP_ADDR_MODE_ZERO_PAGE: {
    addr = self->memory[self->pc++] & 0xFF;
    return self->memory[addr];
  }
  case CHIP_ADDR_MODE_ZERO_PAGE_X: {
    addr = (self->memory[self->pc++] + self->x) & 0xFF;
    return self->memory[addr];
  }
  case CHIP_ADDR_MODE_ZERO_PAGE_Y: {
    addr = (self->memory[self->pc++] + self->y) & 0xFF;
    return self->memory[addr];
  }
  case CHIP_ADDR_MODE_ABSOLUTE: {
    byte lo = self->memory[self->pc++];
    byte hi = self->memory[self->pc++];
    addr = ((hi << 8) | lo);
    return self->memory[self->pc++];
  }
  case CHIP_ADDR_MODE_ABSOLUTE_Y: {
    byte lo = self->memory[self->pc++];
    byte hi = self->memory[self->pc++];
    addr = ((hi << 8) | lo) + self->x;
    return self->memory[self->pc++];
  }
  case CHIP_ADDR_MODE_ABSOLUTE_X: {
    byte lo = self->memory[self->pc++];
    byte hi = self->memory[self->pc++];
    addr = ((hi << 8) | lo) + self->y;
    return self->memory[self->pc++];
  }
  case CHIP_ADDR_MODE_INDIRECT_X: {
    byte indirection = (self->memory[self->pc++] + self->x) & 0xFF;
    byte lo = self->memory[indirection];
    byte hi = self->memory[(indirection + 1) % 0xFF];
    // TODO: check endianness
    addr = ((u16)hi << 8) | lo;
    return self->memory[self->pc++];
  }
  case CHIP_ADDR_MODE_INDIRECT_Y: {
    byte indirection = self->memory[self->pc++];
    byte lo = self->memory[indirection];
    byte hi = self->memory[(indirection + 1) % 0xFF];
    // TODO: check endianness
    addr = (((u16)hi << 8) | lo) + self->y;
    return self->memory[self->pc++];
  }
  case CHIP_ADDR_MODE_INDIRECT: {
    byte lo = self->memory[self->pc++];
    byte hi = self->memory[self->pc++];

    u16 indirection = ((u16)hi << 8) | lo;
    lo = self->memory[indirection];
    // TODO: error check and i think there was a bug in the original 6502?
    hi = self->memory[indirection + 1];
    byte addr = ((u16)hi << 8) | lo;
    return self->memory[addr];
  }

  case CHIP_ADDR_MODE_UNINIT:
  default:
    PANIC();
    return 0;
  }
}

#endif
