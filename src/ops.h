#ifndef __SPACED_H__OPS__
#define __SPACED_H__OPS__

#include "addressing.h"
#include "chip.h"
#include "defs.h"
#include "generated/lookup.h"
#include "memory.h"

static void chip_op_beq(chip_t *self) {
  i8 offset = chip_memory_read_word(self, ADDR_MODE_RELATIVE);
  if (chip_flags_get(self, FLAG_ZERO))
    self->pc += offset;
}

static void chip_op_bne(chip_t *self) {
  i8 offset = chip_memory_read_word(self, ADDR_MODE_RELATIVE);
  if (chip_flags_get(self, FLAG_ZERO) == 0)
    self->pc += offset;
}

// STore X into memory
static void chip_op_stx(chip_t *self, addressing_mode_t mode) {
  chip_memory_perform_write(self, mode, self->x);
}

// STore Y into memory
static void chip_op_sty(chip_t *self, addressing_mode_t mode) {
  chip_memory_perform_write(self, mode, self->y);
}

// STore Accumulator into memory
static void chip_op_sta(chip_t *self, addressing_mode_t mode) {
  chip_memory_perform_write(self, mode, self->ac);
}

// INCrement memory
static void chip_op_inc(chip_t *self, addressing_mode_t mode) {
  u32 read = chip_memory_perform_read(self, mode);
  u16 addr = get_memory_addr(read);
  byte value = get_memory_word(read);

  chip_memory_write_direct(self, addr, value + 1);
}

// DEcrement Y
static void chip_op_dey(chip_t *self) { self->y = (self->y - 1) & 0xFF; }

// DEcrement X
static void chip_op_dex(chip_t *self) { self->x = (self->x - 1) & 0xFF; }

// INcrement Y
static void chip_op_iny(chip_t *self) {
  self->y = (self->y - 1) & 0xFF;
}

// Jump with Saved Return
static void chip_op_jsr(chip_t *self) {
  u16 pc = (self->pc + 2) & 0xFF;

  chip_stack_push(self, (pc >> 8) & 0xFF);
  chip_stack_push(self, pc & 0xFF);

  u32 read = chip_memory_perform_read(self, ADDR_MODE_ABSOLUTE);
  u16 addr = get_memory_addr(read);
  self->pc = addr;
}

static void chip_op_pla(chip_t *self) {
  byte value = chip_stack_pull(self);
  self->ac = value;
}

static void chip_op_pha(chip_t *self) {
  chip_stack_push(self, self->ac);
}

static void chip_op_cpy(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  byte result = self->y - value;

  // FIXME: weird cast?
  chip_flags_update_zero_negative(self, value);
  chip_flags_update_carry(self, self->y >= value);
}

static void chip_op_rts(chip_t *self) { PANIC_("DONE"); }

static void chip_op_lda(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->ac = value;
}

static void chip_op_jmp(chip_t *self, addressing_mode_t mode) {
  u16 read = chip_memory_read_addr(self, mode);
  self->pc = read % 0xFFFF;
}

static void chip_op_ldy(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->y = value;
}

static void chip_op_ldx(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->x = value;
}

static void chip_op_tay(chip_t *self) {
  self->y = self->ac;
  // NOTE(Artur): Just philosophically, is it more beneficial to
  // use self->y or self->ac here?
  chip_flags_update_zero_negative(self, self->y);
}

static void chip_step(chip_t *self) {
  byte opcode = chip_pc_inc(self);
  chip_decode_execute(self, opcode);
}

#endif
