#ifndef __SPACED_H__CHIP__
#define __SPACED_H__CHIP__

#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "instructions.h"

typedef enum chip_quota_policy_t {
  // Consume 1 per instruction
  CHIP_QUOTA_POLICY_FLAT,
} chip_quota_policy_t;

typedef struct chip_t {
  // How the prices for the instructions are calculated
  chip_quota_policy_t quota_policy;
  // How many instructions it can afford to execute
  u32 quota;
  // The actual memory to be modified
  byte *memory;

  byte a, x, y, p;
  u16 sp, pc;
} chip_t;

static void chip_init(chip_t *self, chip_quota_policy_t policy, u32 quota,
                      byte *memory) {
  memset((void *)self, 0, sizeof(chip_t));

  self->quota_policy = policy;
  self->quota = quota;
  self->memory = memory;
}

static byte chip_memory_read(chip_t *self, chip_addr_mode_t mode) {
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

static u32 chip_estimate_quota(chip_t* self) {
  byte b = self->memory[self->pc];
  if (self->quota_policy == CHIP_QUOTA_POLICY_FLAT)
    return 1;
  return 0;
}

static void chip_step(chip_t *self) {
  u32 quota = chip_estimate_quota(self);
  if (self->quota < quota)
    return;
  self->quota -= quota;

  byte b = self->memory[self->pc++];
  chip_addr_mode_t mode = lookup_addr_mode(b);
  chip_op_t op = lookup_instruction(b);

  switch (op) {
  case CHIP_OP_LDA: {
    byte value = chip_memory_read(self, mode);
    self->a = value;
    break;
  }
  case CHIP_OP_ADC: {
    byte value = chip_memory_read(self, mode);
    // TODO: check for carry bit
    self->a += value;
    break;
  }
  default:
    PANIC();
  }
}

static void chip_dump_registers_stdout(chip_t* self) {
  printf("A=%02X X=%02X Y=%02X\n", self->a, self->x, self->y);
} 

#endif
