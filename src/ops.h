#ifndef __SPACED_H__OPS__
#define __SPACED_H__OPS__

#include "chip.h"
#include "defs.h"
#include "generated/lookup.h"
#include "memory.h"

static void chip_op_rts(chip_t *self) { PANIC_("DONE"); }

static void chip_op_lda(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->ac = value;
}

static void chip_op_jmp(chip_t *self, addressing_mode_t mode) {
  u16 read = chip_memory_read_addr(self, mode);
  self->pc = read % 0xFFFF;
}

static void chip_op_ldx(chip_t *self, addressing_mode_t mode) {
  byte value  = chip_memory_read_word(self, mode);
  self->x = value;
}

static void chip_step(chip_t *self) {
  byte opcode = chip_pc_inc(self);
  chip_decode_execute(self, opcode);
}

#endif
