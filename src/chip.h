#ifndef __SPACED_H__CHIP__
#define __SPACED_H__CHIP__

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "defs.h"
#include "ops.h"

#define CHIP_PROGRAM_START 0x0200
#define CHIP_MEMORY_MAX 0x1000

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

  byte registers[8];
} chip_t;

static void chip_init(chip_t *self, chip_quota_policy_t quota_policy, u32 quota,
                      byte *memory) {
  memset((void *)self, 0, sizeof(chip_t));

  self->quota_policy = quota_policy;
  self->quota = quota;
  self->memory = memory;

  self->registers[R_PC_HI] = 0x02;
}

static byte chip_register_read(chip_t *self, register_t r) {
  return self->registers[r];
}

static void chip_register_write(chip_t *self, register_t r, byte v) {
  self->registers[r] = v;
}

static void chip_set_pc(chip_t *self, u16 pc) {
  self->registers[R_PC_LO] = pc & 0xFF;
  self->registers[R_PC_HI] = (pc & 0xFF00) >> 8;
}

static u16 chip_get_pc(chip_t *self) {
  return (((u16)self->registers[R_PC_HI]) << 8) | (self->registers[R_PC_LO]);
}

static u16 chip_inc_pc(chip_t *self) {
  u16 pc = chip_get_pc(self);
  chip_set_pc(self, (pc + 1) % 0xFFFF);
  return pc;
}

static u16 chip_get_ar(chip_t *self) {
  return (((u16)self->registers[R_AR_HI] << 8) | (self->registers[R_AR_LO]));
}

static void chip_load_program(chip_t *self, byte *program, u16 length) {
  if (length > (CHIP_MEMORY_MAX - CHIP_PROGRAM_START)) {
    self->memory[CHIP_PROGRAM_START] = OP_BREAK;
    return;
  };

  memcpy(self->memory, program, length);
}
static byte *chip_stack_size_ptr(chip_t *self) {
  return &self->memory[CHIP_PROGRAM_START - 1];
}

static void chip_stack_push(chip_t *self, byte value) {
  byte *ptr = chip_stack_size_ptr(self);
  self->memory[CHIP_PROGRAM_START - *ptr - 2] = value;
  ++*ptr;
}

static byte chip_stack_pop(chip_t *self) {
  byte *ptr = chip_stack_size_ptr(self);
  if (*ptr == 0) {
    // TODO: check if this is warranted
    // this is basically a hard-coded overflow
    *ptr = 0xFF;
    return 0;
  } else {
    byte value = *ptr;
    --*ptr;
    return value;
  }
}

static void chip_dbg_dump(chip_t *self) {
  printf("R_PC=0x%04X, R_X = 0x%02X, R_Y = 0x%02X, R_AR = 0x%04X, R_ACC = "
         "0x%02X\n",
         chip_get_pc(self), chip_register_read(self, R_X),
         chip_register_read(self, R_Y), chip_get_ar(self),
         chip_register_read(self, R_ACC));
}

static void chip_dbg_dump_stack(chip_t *self) {
  byte size = *chip_stack_size_ptr(self);
  printf("%d | ", size);
  for (int i = size; i > 0; i--)
    printf("%02X ", self->memory[CHIP_PROGRAM_START - i - 1]);
  printf("\n");
}

static void chip_step(chip_t *self) {
  u16 addr = chip_inc_pc(self);
  byte value = self->memory[addr];
  byte msbs = (value & 0b11000000) >> 6;
  byte arg1 = (value & 0b111000) >> 3;
  byte arg2 = (value & 0b111) >> 0;

  // Copy
  if (msbs == 0b11) {
    byte v = chip_register_read(self, arg2);
    chip_register_write(self, arg1, v);
    return;
  }

  // Swap
  if (msbs == 0b10) {
    byte v1 = chip_register_read(self, arg1);
    byte v2 = chip_register_read(self, arg2);
    chip_register_write(self, arg1, v2);
    chip_register_write(self, arg2, v1);
    return;
  }

  // Control
  if (msbs == 0b00) {
    if (value == OP_CONST) {
      addr = chip_inc_pc(self);
      value = self->memory[addr];
      chip_register_write(self, R_ACC, value);
      return;
    }

    switch (arg1) {
    case 0b100: {
      byte value = chip_register_read(self, arg2);
      chip_stack_push(self, value);
      return;
    }

    case 0b101: {
      byte value = chip_stack_pop(self);
      chip_register_write(self, arg2, value);
      return;
    }

    case 0b110:
      if (chip_register_read(self, arg2) == 0) {
        u16 ar = chip_get_ar(self);
        chip_set_pc(self, ar);
      }
      return;
    }

    if (value == OP_NOOP) {
      return;
    };
  }

  // ALU
  if (msbs == 0b01) {
    byte acc = chip_register_read(self, R_ACC);
    byte r = chip_register_read(self, arg2);

    switch (arg1) {
    case 0b000:
      chip_register_write(self, R_ACC, acc + r);
      return;
    }
  }

  // TODO: handle this with more grace
  PANIC("Unknown instruction 0x%02X", value);
}

#endif
